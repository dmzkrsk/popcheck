/*
Module : POP3.H
Purpose: Defines the interface for a MFC class encapsulation of the POP3 protocol
Created: PJN / 04-05-1998
History: None

Copyright (c) 1998 by PJ Naughter.  
All rights reserved.

*/

#ifndef popLinkage
#define popLinkage __declspec (dllimport)
#endif

/////////////////////////////// Defines ///////////////////////////////////////
#ifndef __POP3_H__
#define __POP3_H__

#ifndef __AFXTEMPL_H__
#pragma message("POP3 classes require afxtempl.h in your PCH")                                                                                
#endif

#ifndef _WINSOCKAPI_
#pragma message("POP3 classes require afxsock.h or winsock.h in your PCH")
#endif
  

/////////////////////////////// Classes ///////////////////////////////////////

////// forward declaration
class popLinkage CPop3Connection;

//Encapsulation of a POP3 message
class popLinkage CPop3Message
{
public:
//Constructors / Destructors
  CPop3Message();
  ~CPop3Message();

//Methods
  LPCSTR GetMessageText() const { return m_pszMessage; };

protected:
  char* m_pszMessage;

  friend class CPop3Connection;
};


//Simple Socket wrapper class
class popLinkage CPop3Socket
{
public:
//Constructors / Destructors
  CPop3Socket();
  ~CPop3Socket();

//methods
  BOOL  Create();
  BOOL  Connect(LPCSTR pszHostAddress, int nPort = 110);
  BOOL  Send(LPCSTR pszBuf, int nBuf);
  void  Close();
  int   Receive(LPSTR pszBuf, int nBuf);
  BOOL  IsReadible(BOOL& bReadible);

protected:
  BOOL   Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen);
  SOCKET m_hSocket;

	friend class CPop3Connection;
};
  

//The main class which encapsulates the POP3 connection
class popLinkage CPop3Connection
{
public:
//Constructors / Destructors
  CPop3Connection();
  ~CPop3Connection();

//Methods
  BOOL    Connect(LPCSTR pszHostName, LPCSTR pszUser, LPCSTR pszPassword, int nPort = 110);
  BOOL    Disconnect();
  BOOL    Statistics(int& nNumberOfMails, int& nTotalMailSize);
  BOOL    Delete(int nMsg);
  BOOL    GetMessageSize(int nMsg, DWORD& dwSize);
  BOOL    GetMessageID(int nMsg, CString& sID);
  BOOL    Retrieve(int nMsg, CPop3Message& message);
  BOOL    GetMessageHeader(int nMsg, CPop3Message& message);
  BOOL    Reset();
  BOOL    UIDL();
  BOOL    Noop();
  CString GetLastCommandResponse() const { return m_sLastCommandResponse; };
  DWORD   GetTimeout() const { return m_dwTimeout; };
  void    SetTimeout(DWORD dwTimeout) { m_dwTimeout = dwTimeout; };

protected:
  BOOL  ReadStatResponse(int& nNumberOfMails, int& nTotalMailSize);
	BOOL  ReadResponse(LPSTR pszBuffer, int nBuf, LPSTR pszTerminator);
	BOOL  ReadCommandResponse();
  BOOL  ReadListResponse(int nNumberOfMails);
  BOOL  ReadUIDLResponse(int nNumberOfMails);
  BOOL  ReadReturnResponse(CPop3Message& message, DWORD dwSize);
  BOOL  List();
  LPSTR GetFirstCharInResponse(LPSTR pszData) const;

  CPop3Socket  m_Pop;
  int          m_nNumberOfMails;
  BOOL         m_bListRetrieved;
  BOOL         m_bStatRetrieved;
  BOOL         m_bUIDLRetrieved;
  CDWordArray  m_msgSizes;
  CStringArray m_msgIDs;
  BOOL         m_bConnected;
  CString      m_sLastCommandResponse;
	DWORD        m_dwTimeout;
};


#endif //__POP3_H__

