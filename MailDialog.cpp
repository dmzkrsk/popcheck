// MailDialog.cpp : implementation file
//

#include "stdafx.h"
#include "popcheck.h"
#include "MailDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PERC 40

/////////////////////////////////////////////////////////////////////////////
// CMailDialog dialog


CMailDialog::CMailDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMailDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMailDialog)
	m_header = _T("");
	m_message = _T("");
	//}}AFX_DATA_INIT
}


void CMailDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMailDialog)
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDCANCEL, m_cancel);
	DDX_Control(pDX, IDC_EDIT2, m_msgctrl);
	DDX_Control(pDX, IDC_EDIT1, m_headerctrl);
	DDX_Text(pDX, IDC_EDIT1, m_header);
	DDX_Text(pDX, IDC_EDIT2, m_message);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMailDialog, CDialog)
	//{{AFX_MSG_MAP(CMailDialog)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMailDialog message handlers

void CMailDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if(::IsWindow(m_hWnd) && ::IsWindow(m_msgctrl.m_hWnd) && ::IsWindow(m_headerctrl.m_hWnd) && ::IsWindow(m_ok.m_hWnd) && ::IsWindow(m_cancel.m_hWnd))
	{
		CRect Rect, okRect, ccRect;
		GetClientRect(&Rect);

		m_ok.GetClientRect(&okRect);
		m_cancel.GetClientRect(&ccRect);
		m_ok.MoveWindow(10, Rect.Height()-okRect.Height()-10, okRect.Width(), okRect.Height());
		m_cancel.MoveWindow(Rect.Width()-ccRect.Width()-10, Rect.Height()-ccRect.Height()-10, ccRect.Width(), ccRect.Height());

		m_headerctrl.MoveWindow(10, 10, Rect.Width()-20, Rect.Height()*PERC/100);
		m_msgctrl.MoveWindow(10, Rect.Height()*PERC/100+30, Rect.Width()-20, Rect.Height()-50-Rect.Height()*PERC/100-okRect.Height());
	}
	
	// TODO: Add your message handler code here
	
}

BOOL CMailDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowText(m_Title);
	SetIcon(AfxGetApp()->LoadIcon(IDI_MAILOPEN), FALSE);
	SetIcon(AfxGetApp()->LoadIcon(IDI_MAILOPEN), TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMailDialog::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	lpMMI->ptMinTrackSize = CPoint(300, 250);
    //lpMMI->ptMaxTrackSize = CPoint(300, 300);

	CDialog::OnGetMinMaxInfo(lpMMI);
}
