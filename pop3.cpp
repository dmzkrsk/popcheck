/*
Module : POP3.CPP
Purpose: Implementation for a MFC class encapsulation of the POP3 protocol
Created: PJN / 04-05-1998
History: PJN / 27-06-1998 1) Fixed a potential buffer overflow problem in Delete
                          and Retrieve functions when a large message number was
                          specified.
                          2) Improve the ReadResponse code by a) passing the 
                          readability check onto the scoket class and b) Sleeping
                          for 100 ms prior to looping around waiting for new 
                          response data
                          3) Classes are now fully Unicode compliant. Unicode
                          build configurations are also now included.
                          4) Now supports the TOP POP3 command which can be
                          issued using the GetHeader function.
                          
                             

Copyright (c) 1998 by PJ Naughter.  
All rights reserved.

*/

//////////////// Includes ////////////////////////////////////////////
#include "stdafx.h"
#include <afxpriv.h>
#define popLinkage __declspec (dllexport)
#include "pop3.h"



//////////////// Macros //////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//////////////// Implementation //////////////////////////////////////
CPop3Message::CPop3Message()
{
  m_pszMessage = NULL;
}

CPop3Message::~CPop3Message()
{
  if (m_pszMessage)
  {
    delete [] m_pszMessage;
    m_pszMessage = NULL;
  }
}


CPop3Socket::CPop3Socket()
{
  m_hSocket = INVALID_SOCKET; //default to an invalid scoket descriptor
}

CPop3Socket::~CPop3Socket()
{
  Close();
}

BOOL CPop3Socket::Create()
{
  m_hSocket = socket(AF_INET, SOCK_STREAM, 0);
  return (m_hSocket != INVALID_SOCKET);
}

BOOL CPop3Socket::Connect(LPCSTR pszHostAddress, int nPort)
{
  //must have been created first
  ASSERT(m_hSocket != INVALID_SOCKET);
  
	//Determine if the address is in dotted notation
	SOCKADDR_IN sockAddr;
	ZeroMemory(&sockAddr, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons((u_short)nPort);
	sockAddr.sin_addr.s_addr = inet_addr(pszHostAddress);

	//If the address is not dotted notation, then do a DNS 
	//lookup of it.
	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		LPHOSTENT lphost;
		lphost = gethostbyname(pszHostAddress);
		if (lphost != NULL)
			sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		else
		{
			WSASetLastError(WSAEINVAL); 
			return FALSE;
		}
	}

	//Call the protected version which takes an address 
	//in the form of a standard C style struct.
	return Connect((SOCKADDR*)&sockAddr, sizeof(sockAddr));
}

BOOL CPop3Socket::Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{
	return (connect(m_hSocket, lpSockAddr, nSockAddrLen) != SOCKET_ERROR);
}

BOOL CPop3Socket::Send(LPCSTR pszBuf, int nBuf)
{
  //must have been created first
  ASSERT(m_hSocket != INVALID_SOCKET);

  return (send(m_hSocket, pszBuf, nBuf, 0) != SOCKET_ERROR);
}

int CPop3Socket::Receive(LPSTR pszBuf, int nBuf)
{
  //must have been created first
  ASSERT(m_hSocket != INVALID_SOCKET);

  return recv(m_hSocket, pszBuf, nBuf, 0); 
}

void CPop3Socket::Close()
{
	if (m_hSocket != INVALID_SOCKET)
	{
		VERIFY(SOCKET_ERROR != closesocket(m_hSocket));
		m_hSocket = INVALID_SOCKET;
	}
}

BOOL CPop3Socket::IsReadible(BOOL& bReadible)
{
  timeval timeout = {0, 0};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(m_hSocket, &fds);
  int nStatus = select(0, &fds, NULL, NULL, &timeout);
  if (nStatus == SOCKET_ERROR)
  {
    return FALSE;
  }
  else
  {
    bReadible = !(nStatus == 0);
    return TRUE;
  }
}






CPop3Connection::CPop3Connection()
{
  m_nNumberOfMails = 0;
  m_bListRetrieved = FALSE;
  m_bStatRetrieved = FALSE;
  m_bUIDLRetrieved = FALSE;
  m_msgSizes.RemoveAll();
  m_bConnected = FALSE;
#ifdef _DEBUG
  m_dwTimeout = 20000; //default timeout of 20 seconds when debugging
#else
  m_dwTimeout = 2000;  //default timeout of 2 seconds for normal release code
#endif
}

CPop3Connection::~CPop3Connection()
{
  if (m_bConnected)
    Disconnect();
}

BOOL CPop3Connection::Connect(LPCSTR pszHostName, LPCSTR pszUser, LPCSTR pszPassword, int nPort)
{
	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

  //Create the socket
  if (!m_Pop.Create())
  {
    TRACE(_T("Failed to create client socket\n"));
    return FALSE;
  }

  //Connect to the POP3 Host
  if (!m_Pop.Connect(pszHostName, nPort))
  {
    TRACE(_T("Could not connect to the POP3 mailbox\n"));
    return FALSE;
  }
  else
  {
    //We're now connected !!
    m_bConnected = TRUE;

    //check the response
    if (!ReadCommandResponse())
    {
      Disconnect();
      return FALSE;
    }

    //Send the POP3 username and check the response
    char sBuf[128];
    char* pszAsciiUser = T2A((LPTSTR) pszUser);
    ASSERT(strlen(pszAsciiUser) < 100); 
    sprintf(sBuf, "USER %s\r\n", pszAsciiUser);
    int nCmdLength = strlen(sBuf);
    if (!m_Pop.Send(sBuf, nCmdLength))
    {
      Disconnect();
      return FALSE;
    }
    if (!ReadCommandResponse())
    {
      Disconnect();
      return FALSE;
    } 

    //Send the POP3 password and check the response
    char* pszAsciiPassword = T2A((LPTSTR) pszPassword);
    ASSERT(strlen(pszAsciiPassword) < 100);
    sprintf(sBuf, "PASS %s\r\n", pszAsciiPassword);
    nCmdLength = strlen(sBuf);
    if (!m_Pop.Send(sBuf, nCmdLength))
    {
      Disconnect();
      return FALSE;
    }
    if (!ReadCommandResponse())
    {
      Disconnect();
      return FALSE;
    }
    
    return TRUE;
  }
}

BOOL CPop3Connection::Disconnect()
{          
  BOOL bSuccess = FALSE;      

  //disconnect from the POP3 server if connected 
  if (m_bConnected)
  {
    char sBuf[10];
    strcpy(sBuf, "QUIT\r\n");
    int nCmdLength = strlen(sBuf);
    if (!m_Pop.Send(sBuf, nCmdLength))
      TRACE(_T("Failed in call to send QUIT command\n"));

    //Check the reponse
    bSuccess = ReadCommandResponse();

    //Reset all the state variables
    m_bConnected = FALSE;
    m_bListRetrieved = FALSE;
    m_bStatRetrieved = FALSE;
    m_bUIDLRetrieved = FALSE;
  }
  else
    TRACE(_T("Already disconnected\n"));
 
  //free up our socket
  m_Pop.Close();
 
  return bSuccess;
}

BOOL CPop3Connection::Delete(int nMsg)
{
  //Must be connected to perform a delete
  ASSERT(m_bConnected);

  //if we haven't executed the LIST command then do it now
  if (!m_bListRetrieved)
    List();

  //Send the DELE command along with the message ID
	char sBuf[20];
 	sprintf(sBuf, "DELE %d\r\n", nMsg);
  int nCmdLength = strlen(sBuf);
	if (!m_Pop.Send(sBuf, nCmdLength))
  {
    TRACE(_T("Failed in call to send DELE command\n"));
    return FALSE;
  }

	return ReadCommandResponse();
}

BOOL CPop3Connection::Statistics(int& nNumberOfMails, int& nTotalMailSize)
{
  //Must be connected to perform a "STAT"
  ASSERT(m_bConnected);

  //Send the STAT command
	char sBuf[10];
 	strcpy(sBuf, "STAT\r\n");
  int nCmdLength = strlen(sBuf);
	if (!m_Pop.Send(sBuf, nCmdLength))
  {
    TRACE(_T("Failed in call to send STAT command\n"));
    return FALSE;
  }

	return ReadStatResponse(nNumberOfMails, nTotalMailSize);
}

BOOL CPop3Connection::GetMessageSize(int nMsg, DWORD& dwSize)
{
  BOOL bSuccess = TRUE;

  //if we haven't executed the LIST command then do it now
  if (!m_bListRetrieved)
    bSuccess = List();

  //nMsg must be in the correct range
  ASSERT((nMsg > 0) && (nMsg <= m_msgSizes.GetSize()));

  //retrieve the size from the message size array
  dwSize = m_msgSizes.GetAt(nMsg - 1);

  return bSuccess;
}

BOOL CPop3Connection::GetMessageID(int nMsg, CString& sID)
{
  BOOL bSuccess = TRUE;

  //if we haven't executed the UIDL command then do it now
  if (!m_bUIDLRetrieved)
    bSuccess = UIDL();

  //nMsg must be in the correct range
  ASSERT((nMsg > 0) && (nMsg <= m_msgIDs.GetSize()));

  //retrieve the size from the message size array
  sID = m_msgIDs.GetAt(nMsg - 1);

  return bSuccess;
}

BOOL CPop3Connection::List()
{
  //Must be connected to perform a "LIST"
  ASSERT(m_bConnected);

  //if we haven't executed the STAT command then do it now
  int nNumberOfMails = m_nNumberOfMails;
  int nTotalMailSize;
  if (!m_bStatRetrieved)
  {
    if (!Statistics(nNumberOfMails, nTotalMailSize))
      return FALSE;
    else
      m_bStatRetrieved = TRUE;
  }

  //Send the LIST command
  char sBuf[10];
	strcpy(sBuf, "LIST\r\n");
  int nCmdLength = strlen(sBuf);
	if (!m_Pop.Send(sBuf, nCmdLength))
  {
    TRACE(_T("Failed in call to send LIST command\n"));
    return FALSE;
  }
  //And check the response
	m_bListRetrieved = ReadListResponse(nNumberOfMails);
  return m_bListRetrieved;
}

BOOL CPop3Connection::UIDL()
{
  //Must be connected to perform a "UIDL"
  ASSERT(m_bConnected);

  //if we haven't executed the STAT command then do it now
  int nNumberOfMails = m_nNumberOfMails;
  int nTotalMailSize;
  if (!m_bStatRetrieved)
  {
    if (!Statistics(nNumberOfMails, nTotalMailSize))
      return FALSE;
    else
      m_bStatRetrieved = TRUE;
  }

  //Send the UIDL command
  char sBuf[10];
	strcpy(sBuf, "UIDL\r\n");
  int nCmdLength = strlen(sBuf);
	if (!m_Pop.Send(sBuf, nCmdLength))
  {
    TRACE(_T("Failed in call to send UIDL command\n"));
    return FALSE;
  }
  //And check the response
	m_bUIDLRetrieved = ReadUIDLResponse(nNumberOfMails);
  return m_bUIDLRetrieved;
}


BOOL CPop3Connection::Reset()
{
  //Must be connected to perform a "RSET"
  ASSERT(m_bConnected);

  //Send the RSET command
	char sBuf[10];
 	strcpy(sBuf, "RSET\r\n");
  int nCmdLength = strlen(sBuf);
	if (!m_Pop.Send(sBuf, nCmdLength))
  {
    TRACE(_T("Failed in call to send RSET command\n"));
    return FALSE;
  }

  //And check the command
	return ReadCommandResponse();
}

BOOL CPop3Connection::Noop()
{
  //Must be connected to perform a "NOOP"
  ASSERT(m_bConnected);

  //Send the NOOP command
	char sBuf[10];
 	strcpy(sBuf, "NOOP\r\n");
  int nCmdLength = strlen(sBuf);
	if (!m_Pop.Send(sBuf, nCmdLength))
  {
    TRACE(_T("Failed in call to send NOOP command\n"));
    return FALSE;
  }

  //And check the response
	return ReadCommandResponse();
}

BOOL CPop3Connection::Retrieve(int nMsg, CPop3Message& message)
{
  //Must be connected to retrieve a message
  ASSERT(m_bConnected);

  //work out the size of the message to retrieve
  DWORD dwSize;
  if (GetMessageSize(nMsg, dwSize))
  {
    //Send the RETR command
	  char sBuf[20];
	  sprintf(sBuf, "RETR %d\r\n", nMsg);	
    int nCmdLength = strlen(sBuf);
	  if (!m_Pop.Send(sBuf, nCmdLength))
    {
      TRACE(_T("Failed in call to send RETR command\n"));
      return FALSE;
    }
    
		//And check the command
	  return ReadReturnResponse(message, dwSize);
  }
  else
    return FALSE;
}

BOOL CPop3Connection::GetMessageHeader(int nMsg, CPop3Message& message)
{
  // Must be connected to retrieve a message
  ASSERT(m_bConnected);

  // make sure the message actually exists
  DWORD dwSize;
  if (GetMessageSize(nMsg, dwSize))
  {
    // Send the TOP command
    char sBuf[16];
    sprintf(sBuf, "TOP %d 0\r\n", nMsg);
    int nCmdLength = strlen(sBuf);
    if (!m_Pop.Send(sBuf, nCmdLength))
    {
      TRACE(_T("Failed in call to send TOP command\n"));
      return FALSE;
    }

    // And check the command
    return ReadReturnResponse(message, dwSize);
  }
  else
      return FALSE;
}

BOOL CPop3Connection::ReadCommandResponse()
{
	char sBuf[1000];
	return ReadResponse(sBuf, sizeof(sBuf), "\r\n");
}

BOOL CPop3Connection::ReadReturnResponse(CPop3Message& message, DWORD dwSize)
{
  //Must be connected to perform a "RETR"
  ASSERT(m_bConnected);

  //We need a flexible sized receiver buffer here
  int nSize = dwSize + 100;
  char* sBuf = new char[nSize];
	if (!ReadResponse(sBuf, nSize, "\r\n.\r\n"))
	{
		TRACE(_T("Error retrieving the RETR response"));
		return FALSE;
	}

  //determine if the response is an error
  if (strnicmp(sBuf,"+OK", 3) != 0)
	{
    SetLastError(WSAEPROTONOSUPPORT);
		TRACE(_T("POP3 server did not respond correctly to the RETR response\n"));
    delete []sBuf;
		return FALSE;
	}
	else
	{  
    //remove the first line which contains the +OK from the message
    char* pszFirst = GetFirstCharInResponse(sBuf);
		VERIFY(pszFirst);

    //transfer the message contents to the message class
    int nMessageSize = sBuf - pszFirst + strlen(sBuf);
    message.m_pszMessage = new char[nMessageSize + 1];
    memcpy(message.m_pszMessage, pszFirst, nMessageSize);
    message.m_pszMessage[nMessageSize] = '\0';
	}
  delete []sBuf;
  return TRUE; 
}

BOOL CPop3Connection::ReadListResponse(int nNumberOfMails)
{
  //Must be connected to perform a "LIST"
  ASSERT(m_bConnected);

  //We need a flexible sized receiver buffer here
  int nSize = 14 * nNumberOfMails + 100;
  char* sBuf = new char[nSize];

  //retrieve the reponse
	if (!ReadResponse(sBuf, nSize, "\r\n.\r\n"))
	{
		TRACE(_T("Error retrieving the LIST response\n"));
		delete []sBuf;
		return FALSE;
	}

  //determine if the response is an error
  if (strnicmp(sBuf,"+OK", 3) != 0)
	{
    SetLastError(WSAEPROTONOSUPPORT);
		TRACE(_T("POP3 server did not respond correctly to the LIST response\n"));
    delete[] sBuf;
		return FALSE;
	}
	else
	{  
    //Retrieve the message sizes and put them
    //into the m_msgSizes array
    m_msgSizes.RemoveAll();
    m_msgSizes.SetSize(0, nNumberOfMails);

    //then parse the LIST response
		char* pszSize = GetFirstCharInResponse(sBuf);
		VERIFY(pszSize);
		for (; *pszSize != '.'; pszSize++)
			if (*pszSize == '\t' || *pszSize == ' ')
				m_msgSizes.Add(atoi(pszSize));
	}
  delete[] sBuf;
  return TRUE; 
}


BOOL CPop3Connection::ReadUIDLResponse(int nNumberOfMails)
{
  //Must be connected to perform a "LIST"
  ASSERT(m_bConnected);

  //We need a flexible sized receiver buffer here
  int nSize = 1000 * nNumberOfMails + 100;
  char* sBuf = new char[nSize];

  //retrieve the reponse
	if (!ReadResponse(sBuf, nSize, "\r\n.\r\n"))
	{
		TRACE(_T("Error retrieving the UIDL response\n"));
		delete []sBuf;
		return FALSE;
	}

  //determine if the response is an error
  if (strnicmp(sBuf,"+OK", 3) != 0)
	{
    SetLastError(WSAEPROTONOSUPPORT);
		TRACE(_T("POP3 server did not respond correctly to the UIDL response\n"));
    delete[] sBuf;
		return FALSE;
	}
	else
	{  
    //Retrieve the message ID's and put them
    //into the m_msgIDs array
    m_msgIDs.RemoveAll();
    m_msgIDs.SetSize(0, nNumberOfMails);

    //then parse the UIDL response
		char* pszSize = GetFirstCharInResponse(sBuf);
		VERIFY(pszSize);
		for (; *pszSize != '.'; pszSize++)
    {
      char* pszBegin = pszSize;
      while (*pszSize != '\n' && *pszSize != '.')
      {
        ++pszSize;
      }
      if (*pszSize == '.')
        continue;

      char sMsg[15];
      char sID[1000];
      *pszSize = '\0';
      sscanf(pszBegin, "%s %s", sMsg, sID);

      m_msgIDs.Add(CString(sID));
    }
	}
  delete[] sBuf;
  return TRUE; 
}

LPSTR CPop3Connection::GetFirstCharInResponse(LPSTR pszData) const
{
  while ((*pszData != '\n') && *pszData)
    ++pszData;

  //skip over the "\n" onto the next line
  if (*pszData)
    ++pszData;

  return pszData;
}

BOOL CPop3Connection::ReadStatResponse(int& nNumberOfMails, int& nTotalMailSize)
{
  //Must be connected to perform a "STAT"
  ASSERT(m_bConnected);

  //retrieve the reponse
	char sBuf[100];
	if (!ReadResponse(sBuf, sizeof(sBuf), "\r\n"))
	{
		TRACE(_T("Error retrieving the STAT response"));
		return FALSE;
	}

  //determine if the response is an error
  if (strncmp(sBuf,"+OK", 3) != 0)
	{
		TRACE(_T("POP3 server did not respond correctly to the STAT response\n"));
		return FALSE;
	}
	else
	{                                          
    //Parse out the Number of Mails and Total mail size values
		BOOL bGetNumber = TRUE;
		for (char* pszNum=sBuf; *pszNum!='\0'; pszNum++)
		{
			if (*pszNum=='\t' || *pszNum==' ')
			{						
				if (bGetNumber)
				{
					nNumberOfMails = atoi(pszNum);
          m_nNumberOfMails = nNumberOfMails;
					bGetNumber = FALSE;
				}
				else
				{
					nTotalMailSize = atoi(pszNum);
					return TRUE;
				}
			}
		}
	}
  return FALSE; 
}

BOOL CPop3Connection::ReadResponse(LPSTR pszBuffer, int nBuf, LPSTR pszTerminator)
{
	//paramater validity checking
	ASSERT(pszBuffer);
	ASSERT(nBuf);

  //must have been created first
  ASSERT(m_bConnected);

  //retrieve the reponse using until we
	//get the terminator or a timeout occurs
	BOOL bFoundTerminator = FALSE;
	int nReceived = 0;
	DWORD dwStartTicks = ::GetTickCount();
	while (!bFoundTerminator)
	{
		//timeout has occured
		if ((::GetTickCount() - dwStartTicks) >	m_dwTimeout)
		{
		  pszBuffer[nReceived] = '\0';
			SetLastError(WSAETIMEDOUT);
      m_sLastCommandResponse = pszBuffer; //Hive away the last command reponse
			return FALSE;
		}

    //check the socket for readability
    BOOL bReadible;
    if (!m_Pop.IsReadible(bReadible))
    {
	    pszBuffer[nReceived] = '\0';
			m_sLastCommandResponse = pszBuffer; //Hive away the last command reponse
			return FALSE;
    }
    else if (!bReadible) //no data to receive, just loop around
    {
      Sleep(100); //Wait for 100 ms prior to looping around, 
                  //helps to improve performance of system
      continue;
    }

		//receive the data from the socket
	  int nData = m_Pop.Receive(pszBuffer+nReceived, nBuf-nReceived);
		if (nData == SOCKET_ERROR)
		{
		  pszBuffer[nReceived] = '\0';
      m_sLastCommandResponse = pszBuffer; //Hive away the last command reponse
		  return FALSE; 
		}
		else
		{
      if (nData)
			  dwStartTicks = ::GetTickCount(); //Reset the idle timeout
		  nReceived += nData;							 //Increment the count of data received
		}

		pszBuffer[nReceived] = '\0';	//temporarily NULL terminate the string
																	//so that strstr works
		bFoundTerminator = (strstr(pszBuffer, pszTerminator) != NULL);
	}

	//Remove the terminator from the response data
  pszBuffer[nReceived - strlen(pszTerminator)] = '\0';

  //determine if the response is an error
	BOOL bSuccess = (strnicmp(pszBuffer,"+OK", 3) == 0);

  if (!bSuccess)
  {
    SetLastError(WSAEPROTONOSUPPORT);
    m_sLastCommandResponse = pszBuffer; //Hive away the last command reponse
  }

  return bSuccess;
}
