// popcheck.h : main header file for the POPCHECK application
//

#if !defined(AFX_POPCHECK_H__A219294E_0DFB_42DE_B42E_A3D84B0F498A__INCLUDED_)
#define AFX_POPCHECK_H__A219294E_0DFB_42DE_B42E_A3D84B0F498A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "trayicon.h"

/////////////////////////////////////////////////////////////////////////////
// CPopcheckApp:
// See popcheck.cpp for the implementation of this class
//

#define WM_TRAY_NOTIFY WM_USER+101

class CPopcheckApp : public CWinApp
{
private:
	CTrayIcon* pctiIcon;
public:
	CTrayIcon* GetTray();
	CPopcheckApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPopcheckApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPopcheckApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POPCHECK_H__A219294E_0DFB_42DE_B42E_A3D84B0F498A__INCLUDED_)
