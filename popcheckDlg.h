#if !defined(AFX_POPCHECKDLG_H__1C5A6B1E_4E8C_473D_A3D3_C2B39DB89757__INCLUDED_)
#define AFX_POPCHECKDLG_H__1C5A6B1E_4E8C_473D_A3D3_C2B39DB89757__INCLUDED_

//#include "Pop31.h"	// Added by ClassView
#include "Registry.h"	// Added by ClassView
#include "NewMenu.h"
#include "MailDialog.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPopcheckDlg dialog

class CPopcheckDlg : public CNewDialog
{
// Construction
public:
	BOOL bEnableOff;
	~CPopcheckDlg();
	void CheckMail(BOOL bAll=FALSE);
	void Dispatch(LONG Param);
	CPopcheckDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPopcheckDlg)
	enum { IDD = IDD_POPCHECK_DIALOG };
	CListCtrl	m_maillist;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPopcheckDlg)
	public:
	virtual int DoModal();
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPopcheckDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTrayNotification(WPARAM wp, LPARAM lp);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExit();
	afx_msg void OnCheckmail();
	afx_msg void OnDrop();
	afx_msg void OnAbout();
	afx_msg void OnOptions();
	afx_msg void OnOpen();
	afx_msg void OnStopsounds();
	afx_msg void OnCheckall();
	afx_msg void OnDblclkMails(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnRclickMails(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPopupView();
	afx_msg void OnPopipRe();
	afx_msg void OnItemclickMails(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int iLastSelectedItem;
	CImageList cilPriority;
	CToolBar m_wndToolBar;
	CMailDialog cmdMailView;
	CAppRegistry* pReg;
	BOOL bStart;
	CTrayIcon* GetTray();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POPCHECKDLG_H__1C5A6B1E_4E8C_473D_A3D3_C2B39DB89757__INCLUDED_)
