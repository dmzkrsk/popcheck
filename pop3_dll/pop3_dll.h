// pop3_dll.h : main header file for the POP3_DLL DLL
//

#if !defined(AFX_POP3_DLL_H__4D702A8D_53F0_485A_AD36_8A898D2653B4__INCLUDED_)
#define AFX_POP3_DLL_H__4D702A8D_53F0_485A_AD36_8A898D2653B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPop3_dllApp
// See pop3_dll.cpp for the implementation of this class
//

class CPop3_dllApp : public CWinApp
{
public:
	CPop3_dllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPop3_dllApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPop3_dllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POP3_DLL_H__4D702A8D_53F0_485A_AD36_8A898D2653B4__INCLUDED_)
