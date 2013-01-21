#include "stdafx.h"
#include <atlbase.h>
#define regLinkage __declspec (dllexport)
#include "Registry.h"

CRegistry::CRegistry()
{
	pRegKey=new CRegKey;
}

CRegistry::~CRegistry()
{
	delete pRegKey;
}

int CRegistry::SetValue(HKEY Root, CString Path, CString Key, CString Value)
{
	long Error;
	if(pRegKey->Create(Root,Path.GetBuffer(Path.GetLength()))==ERROR_SUCCESS)
	{
		Error=pRegKey->SetValue(Value,Key);
		pRegKey->Close();
		return (Error==0)?1:0;
	}
	else
		return 0;
}

int CRegistry::SetValue(HKEY Root, CString Path, CString Key, DWORD Value)
{
	long Error;
	if(pRegKey->Create(Root,Path.GetBuffer(Path.GetLength()))==ERROR_SUCCESS)
	{
		Error=pRegKey->SetValue(Value, Key);
		pRegKey->Close();
		return (Error==0)?1:0;
	}
	else
		return 0;
}

CString CRegistry::GetValue(HKEY Root, CString Path, CString Key)
{
	char Result[16384];
	unsigned long a=16384;
	long Error;

	if(pRegKey->Open(Root,Path.GetBuffer(Path.GetLength()))==ERROR_SUCCESS)
	{
		Error=pRegKey->QueryValue(Result,_T(Key),&a);
		pRegKey->Close();
		if(Error==ERROR_SUCCESS)
			return CString(Result);
		else
			return CString("Key doesn't exist");
	}
	else
		return CString("Key doesn't exist");
}

BOOL CRegistry::IsKeyExist(HKEY Root, CString Path, CString Key)
{
	if(GetValue(Root, Path, Key)!="Key doesn't exist")
		return TRUE;
	else
		return FALSE;
}

BOOL CRegistry::IsNumericKeyExist(HKEY Root, CString Path, CString Key)
{
	if(GetNumericValue(Root, Path, Key)!=-1)
		return TRUE;
	else
		return FALSE;
}

int CRegistry::DeleteValue(HKEY Root, CString Path, CString Key)
{
	long Error;

	if(pRegKey->Open(Root,Path.GetBuffer(Path.GetLength()))==ERROR_SUCCESS)
	{
		Error=pRegKey->DeleteValue(Key);
		pRegKey->Close();
		return (Error==0)?1:0;
	}
	else
		return 0;
}

int CRegistry::DeleteKey(HKEY Root, CString Path, CString Key)
{
	long Error;

	if(pRegKey->Open(Root,Path.GetBuffer(Path.GetLength()))==ERROR_SUCCESS)
	{
		Error=pRegKey->DeleteSubKey(Key);
		pRegKey->Close();
		return (Error==0)?1:0;
	}
	else
		return 0;
}

long CRegistry::GetNumericValue(HKEY Root, CString Path, CString Key)
{
	unsigned long Result;
	long Error;

	if(pRegKey->Open(Root,Path.GetBuffer(Path.GetLength()))==ERROR_SUCCESS)
	{
		Error=pRegKey->QueryValue(Result,_T(Key));
		pRegKey->Close();
		if(Error==ERROR_SUCCESS) return Result;
		else return -1;
	}
	else
		return -1;
}

CAppRegistry::CAppRegistry(CString Company, CString AppName, BOOL ToLocalMachine)
{
	if(ToLocalMachine) RegRoot=HKEY_LOCAL_MACHINE;
	else RegRoot=HKEY_CURRENT_USER;

	RegPath.Format("Software\\%s\\%s", Company, AppName);
}

long CAppRegistry::AppGetNumericValue(CString Key, long DefaultValue)
{
	if(AppIsNumericKeyExist(Key)) return GetNumericValue(RegRoot, RegPath, Key);
	else return DefaultValue;
}

CString CAppRegistry::AppGetValue(CString Key, CString DefaultValue)
{
	if(AppIsKeyExist(Key)) return GetValue(RegRoot, RegPath, Key);
	else return DefaultValue;
}
