// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "popcheck.h"
#include "OptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog


COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDlg)
	m_login = _T("");
	m_password = _T("");
	m_server = _T("");
	m_checkrestore = FALSE;
	m_checkstart = FALSE;
	m_notifymethod = -1;
	m_showmethod = -1;
	m_timervalue = 0;
	m_wavename = _T("");
	m_autodrop = FALSE;
	//}}AFX_DATA_INIT
}


void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
	DDX_Control(pDX, IDC_STOPWAV, m_stop);
	DDX_Control(pDX, IDC_PLAYWAV, m_play);
	DDX_Control(pDX, IDC_CHOOSEWAVE, m_choose);
	DDX_Control(pDX, IDC_TSPIN, m_spin);
	DDX_Text(pDX, IDC_LOGIN, m_login);
	DDX_Text(pDX, IDC_PASSWORD, m_password);
	DDX_Text(pDX, IDC_SERVER, m_server);
	DDX_Check(pDX, IDC_CHECKRESTORE, m_checkrestore);
	DDX_Check(pDX, IDC_CHECKSTART, m_checkstart);
	DDX_Radio(pDX, IDC_RADIO_ICON, m_notifymethod);
	DDX_Radio(pDX, IDC_SHOWALL, m_showmethod);
	DDX_Text(pDX, IDC_TIMER, m_timervalue);
	DDX_Text(pDX, IDC_WAVENAME, m_wavename);
	DDX_Check(pDX, IDC_AUTODROP, m_autodrop);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_CHOOSEWAVE, OnChoosewave)
	ON_BN_CLICKED(IDC_RADIO_ICON, OnRadioIcon)
	ON_BN_CLICKED(IDC_RADIO_OPEN, OnRadioOpen)
	ON_BN_CLICKED(IDC_RADIO_WAVE, OnRadioWave)
	ON_BN_CLICKED(IDC_PLAYWAV, OnPlaywav)
	ON_BN_CLICKED(IDC_STOPWAV, OnStopwav)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers

BOOL COptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	((CEdit*)GetDlgItem(IDC_SERVER))->SetLimitText(2048);
	((CEdit*)GetDlgItem(IDC_PASSWORD))->SetLimitText(64);
	((CEdit*)GetDlgItem(IDC_LOGIN))->SetLimitText(64);

	m_spin.SetRange(1,240);
	m_choose.SetIcon(AfxGetApp()->LoadIcon(IDI_WAVE));
	m_play.SetIcon(AfxGetApp()->LoadIcon(IDI_PLAY));
	m_stop.SetIcon(AfxGetApp()->LoadIcon(IDI_STOP));

	EnableWaveChoose(m_notifymethod==2);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CString sNum;
	sNum.Format("%d", nPos);
	((CSpinButtonCtrl*)pScrollBar)->GetBuddy()->SetWindowText(sNum);
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void COptionsDlg::OnOK() 
{
	int nTimer=GetDlgItemInt(IDC_TIMER);
	if(nTimer<1 || nTimer>240)
	{
		MessageBox("Время проверки должно лежать в диапазоне от одной до 240 минут!", "Ошибка", MB_OK|MB_ICONWARNING);
		return;
	}
	CDialog::OnOK();
}

void COptionsDlg::OnChoosewave() 
{
	CString WavName;
	GetDlgItemText(IDC_WAVENAME, WavName);
	CFileDialog cfd(TRUE, NULL, WavName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Microsoft PCM Wave (*.wav)|*.wav|Все файлы|*.*||");
	if(cfd.DoModal()==IDOK)
		SetDlgItemText(IDC_WAVENAME,cfd.GetPathName());
	// TODO: Add your control notification handler code here
}

void COptionsDlg::OnRadioIcon() {EnableWaveChoose(FALSE);}
void COptionsDlg::OnRadioOpen() {EnableWaveChoose(FALSE);}
void COptionsDlg::OnRadioWave() {EnableWaveChoose();}

void COptionsDlg::EnableWaveChoose(BOOL bEnable)
{
	if(bEnable)
	{
		GetDlgItem(IDC_WAVENAME)->EnableWindow();
		GetDlgItem(IDC_CHOOSEWAVE)->EnableWindow();
		GetDlgItem(IDC_PLAYWAV)->EnableWindow();
		GetDlgItem(IDC_STOPWAV)->EnableWindow();
	}
	else
	{
		GetDlgItem(IDC_WAVENAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHOOSEWAVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_PLAYWAV)->EnableWindow(FALSE);
		GetDlgItem(IDC_STOPWAV)->EnableWindow(FALSE);
	}
}

void COptionsDlg::OnPlaywav() 
{
	CString csWave;
	GetDlgItemText(IDC_WAVENAME, csWave);
	::PlaySound(csWave ,NULL, SND_FILENAME|SND_ASYNC);
}

void COptionsDlg::OnStopwav() 
{
	::PlaySound(NULL ,NULL, SND_FILENAME|SND_ASYNC);
}
