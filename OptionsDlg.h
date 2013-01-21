#if !defined(AFX_OPTIONSDLG_H__F6EE05FB_266A_4BB9_92A8_9CE175F74D48__INCLUDED_)
#define AFX_OPTIONSDLG_H__F6EE05FB_266A_4BB9_92A8_9CE175F74D48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog

class COptionsDlg : public CDialog
{
// Construction
public:
	COptionsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsDlg)
	enum { IDD = IDD_OPTIONS };
	CButton	m_stop;
	CButton	m_play;
	CButton	m_choose;
	CSpinButtonCtrl	m_spin;
	CString	m_login;
	CString	m_password;
	CString	m_server;
	BOOL	m_checkrestore;
	BOOL	m_checkstart;
	int		m_notifymethod;
	int		m_showmethod;
	UINT	m_timervalue;
	CString	m_wavename;
	BOOL	m_autodrop;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnOK();
	afx_msg void OnChoosewave();
	afx_msg void OnRadioIcon();
	afx_msg void OnRadioOpen();
	afx_msg void OnRadioWave();
	afx_msg void OnPlaywav();
	afx_msg void OnStopwav();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void EnableWaveChoose(BOOL bEnable=TRUE);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDLG_H__F6EE05FB_266A_4BB9_92A8_9CE175F74D48__INCLUDED_)
