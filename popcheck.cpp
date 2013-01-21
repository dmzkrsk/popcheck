// popcheck.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "popcheck.h"
#include "popcheckDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPopcheckApp

BEGIN_MESSAGE_MAP(CPopcheckApp, CWinApp)
	//{{AFX_MSG_MAP(CPopcheckApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPopcheckApp construction

CPopcheckApp::CPopcheckApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPopcheckApp object

CPopcheckApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPopcheckApp initialization

BOOL CPopcheckApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	pctiIcon=new CTrayIcon(IDR_TRAYICON);

	CPopcheckDlg dlg;
	m_pMainWnd = &dlg;
	
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CPopcheckApp::ExitInstance() 
{
	pctiIcon->SetIcon(NULL);
	delete pctiIcon;
	return CWinApp::ExitInstance();
}

CTrayIcon* CPopcheckApp::GetTray()
{
	return pctiIcon;
}
