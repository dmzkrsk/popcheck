// trayicon_dll.h : main header file for the TRAYICON_DLL DLL
//

#if !defined(AFX_TRAYICON_DLL_H__A2D9EA31_2631_45F9_881D_FB8BF67EA511__INCLUDED_)
#define AFX_TRAYICON_DLL_H__A2D9EA31_2631_45F9_881D_FB8BF67EA511__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTrayicon_dllApp
// See trayicon_dll.cpp for the implementation of this class
//

class CTrayicon_dllApp : public CWinApp
{
public:
	CTrayicon_dllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrayicon_dllApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CTrayicon_dllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAYICON_DLL_H__A2D9EA31_2631_45F9_881D_FB8BF67EA511__INCLUDED_)
