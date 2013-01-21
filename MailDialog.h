#if !defined(AFX_MAILDIALOG_H__C3318BC0_5BD7_49A7_B1F4_713E0FC58500__INCLUDED_)
#define AFX_MAILDIALOG_H__C3318BC0_5BD7_49A7_B1F4_713E0FC58500__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MailDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMailDialog dialog

class CMailDialog : public CDialog
{
// Construction
public:
	CMailDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMailDialog)
	enum { IDD = IDD_MAILVIEW };
	CButton	m_ok;
	CButton	m_cancel;
	CEdit	m_msgctrl;
	CEdit	m_headerctrl;
	CString	m_header;
	CString	m_message;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMailDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMailDialog)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString m_Title;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAILDIALOG_H__C3318BC0_5BD7_49A7_B1F4_713E0FC58500__INCLUDED_)
