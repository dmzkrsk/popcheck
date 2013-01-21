// nmenu_dll.h : main header file for the NMENU_DLL DLL
//

#if !defined(AFX_NMENU_DLL_H__0CD7C71A_A993_4F6F_9C79_5608722ED6DA__INCLUDED_)
#define AFX_NMENU_DLL_H__0CD7C71A_A993_4F6F_9C79_5608722ED6DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNmenu_dllApp
// See nmenu_dll.cpp for the implementation of this class
//

class CNmenu_dllApp : public CWinApp
{
public:
	CNmenu_dllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNmenu_dllApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CNmenu_dllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NMENU_DLL_H__0CD7C71A_A993_4F6F_9C79_5608722ED6DA__INCLUDED_)
