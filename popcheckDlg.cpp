// popcheckDlg.cpp : implementation file
//

#include "stdafx.h"
#include "popcheck.h"
#include "popcheckDlg.h"
#include "pop3.h"

#include "optionsdlg.h"
#define FROM "from:"
#define SUBJ "subject:"
#define DATE "date:"
#define PRIORITY "x-priority:"

#define CHECK_TIMER 454500

#define RWIDTH 48
#define FWIDTH 54

//#define APP_NAME "PopCheckerByTA"

#define DEFAULT_SERVER "mymail.ru"
#define DEFAULT_USER "vasya_pupkin"
#define DEFAULT_PASSWORD "very_complicated_password"
#define DEFAULT_TIMER 10
#define DEFAULT_CHECKSTARTUP 1
#define DEFAULT_CHECKRESTORE 1
#define DEFAULT_NOTIFY 0
#define DEFAULT_FILTER 1
#define DEFAULT_ADROP 1

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PER_WIDTH 0.9
#define PER_HEIGHT 0.4
#define MPER_WIDTH .97
#define MPER_HEIGHT 0.8

typedef struct {
	LPTSTR pszLastName;
	LPTSTR pszFirstName;
	LPTSTR pszTerm;
} ITEMDATA, *PITEMDATA; 

int CALLBACK Comparator(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   CListCtrl* pListCtrl = (CListCtrl*) lParamSort;
   CString    strItem1 = pListCtrl->GetItemText(lParam1, 0);
   CString    strItem2 = pListCtrl->GetItemText(lParam2, 0);

   TRACE("%x == %x\n", lParam1, lParam2);

   return strcmp(strItem2, strItem1);
} 

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CNewDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CNewDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CNewDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CNewDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPopcheckDlg dialog

CPopcheckDlg::CPopcheckDlg(CWnd* pParent /*=NULL*/)
	: CNewDialog(CPopcheckDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPopcheckDlg)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pReg=new CAppRegistry("PhysGluck", "PopChecker");
}

void CPopcheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CNewDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPopcheckDlg)
	DDX_Control(pDX, IDC_MAILS, m_maillist);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPopcheckDlg, CNewDialog)
	//{{AFX_MSG_MAP(CPopcheckDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_TRAY_NOTIFY, OnTrayNotification)
	ON_WM_SIZE()
	ON_COMMAND(IDM_EXIT, OnExit)
	ON_COMMAND(IDM_CHECKMAIL, OnCheckmail)
	ON_COMMAND(IDM_DROP, OnDrop)
	ON_COMMAND(IDM_ABOUT, OnAbout)
	ON_COMMAND(IDM_OPTIONS, OnOptions)
	ON_COMMAND(IDM_OPEN, OnOpen)
	ON_COMMAND(IDC_STOPSOUNDS, OnStopsounds)
	ON_COMMAND(ID_CHECKALL, OnCheckall)
	ON_NOTIFY(NM_DBLCLK, IDC_MAILS, OnDblclkMails)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_RCLICK, IDC_MAILS, OnRclickMails)
	ON_COMMAND(IDM_POPUP_VIEW, OnPopupView)
	ON_COMMAND(IDM_POPIP_RE, OnPopipRe)
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnItemclickMails)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPopcheckDlg message handlers

BOOL CPopcheckDlg::OnInitDialog()
{
#ifdef _DEBUG
	SetWindowText("==DEBUG==");
#endif
	GetTray()->SetNotificationWnd(this, WM_TRAY_NOTIFY);
	OnDrop();
	CNewDialog::OnInitDialog();

	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MENU_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	cilPriority.Create(16, 16, ILC_COLOR, 3, 1);
	cilPriority.Add(AfxGetApp()->LoadIcon(IDI_NORMP));
	cilPriority.Add(AfxGetApp()->LoadIcon(IDI_HIGHP));
	cilPriority.Add(AfxGetApp()->LoadIcon(IDI_LOWP));

	m_maillist.SetImageList(&cilPriority, LVSIL_SMALL);

	CBitmap tbpic;
	CImageList Images;
	tbpic.LoadBitmap(IDB_TOOLBARBW);
	Images.Create(RWIDTH,RWIDTH,ILC_COLOR24|ILC_MASK,1,1);
	Images.Add(&tbpic, RGB(192,192,192));
	m_wndToolBar.GetToolBarCtrl().SetImageList(&Images);
	Images.Detach();
	tbpic.Detach();
	tbpic.LoadBitmap(IDB_TOOLBAR);
	Images.Create(RWIDTH,RWIDTH,ILC_COLOR24|ILC_MASK,1,1);
	Images.Add(&tbpic, RGB(192,192,192));
	m_wndToolBar.GetToolBarCtrl().SetHotImageList(&Images);
	Images.Detach();
	BITMAP bmp;
	tbpic.GetBitmap(&bmp);
	long lTWidth=bmp.bmWidth;
	tbpic.Detach();

	m_wndToolBar.GetToolBarCtrl().SetButtonSize(CSize(FWIDTH,FWIDTH));
	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT);

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY );

	CRect rcClientStart;
	CRect rcClientNow;
	GetClientRect(rcClientStart);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 
				   0, reposQuery, rcClientNow);
	
	// Now move all the controls so they are in the same relative
	// position within the remaining client area as they would be
	// with no control bars.
	CPoint ptOffset(rcClientNow.left - rcClientStart.left,
					rcClientNow.top - rcClientStart.top); 

	CRect  rcChild;					
	CWnd* pwndChild = GetWindow(GW_CHILD);
	while (pwndChild)
	{                               
		pwndChild->GetWindowRect(rcChild);
		ScreenToClient(rcChild);
		rcChild.OffsetRect(ptOffset);
		pwndChild->MoveWindow(rcChild, FALSE);
		pwndChild = pwndChild->GetNextWindow();
	}

	// Adjust the dialog window dimensions
/*	CRect rcWindow;
	GetWindowRect(rcWindow);
	rcWindow.right += rcClientStart.Width() - rcClientNow.Width();
	rcWindow.bottom += rcClientStart.Height() - rcClientNow.Height();
	MoveWindow(rcWindow, FALSE);
*/	
	// And position the control bars
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	int sWidth=::GetSystemMetrics(SM_CXSCREEN);
	int sHeight=::GetSystemMetrics(SM_CYSCREEN);

	int rLeft=(int)((double)sWidth*(1-PER_WIDTH)/2);
	int rTop=(int)((double)sHeight*(1-PER_HEIGHT)/2);
	int rWidth=(int)(sWidth*PER_WIDTH);
	int rHeight=(int)(sHeight*PER_HEIGHT);
	MoveWindow(rLeft, rTop, rWidth, rHeight+ptOffset.y);

	int mLeft=(int)((double)rWidth*(1-MPER_WIDTH)/2);
	int mWidth=(int)(rWidth*MPER_WIDTH);
	int mHeight=(int)(rHeight*MPER_HEIGHT);
	m_maillist.MoveWindow(mLeft, ptOffset.y+4, mWidth, mHeight);
	long rTWidth=lTWidth*FWIDTH/RWIDTH+32;
	m_wndToolBar.MoveWindow((mWidth-rTWidth)/2, 0, rTWidth, ptOffset.y);

	DWORD dwStyle=m_maillist.GetExtendedStyle();
	dwStyle|=LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP;
	m_maillist.SetExtendedStyle(dwStyle);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_maillist.InsertColumn(0,"", LVCFMT_LEFT, (int)(mWidth/20));
	m_maillist.InsertColumn(1,"Отправитель", LVCFMT_LEFT, (int)(mWidth/4.2));
	m_maillist.InsertColumn(2,"Почтовый ящик", LVCFMT_LEFT, (int)(mWidth/4.2));
	m_maillist.InsertColumn(3,"Тема", LVCFMT_LEFT, (int)(mWidth/4.2));
	m_maillist.InsertColumn(4,"Дата", LVCFMT_LEFT, (int)(mWidth/4.2));

	if(pReg->AppGetNumericValue("onstartup", DEFAULT_CHECKRESTORE)) CheckMail();

	UINT nElapse=pReg->AppGetNumericValue("timervalue", DEFAULT_TIMER)*60*1000;
	SetTimer(CHECK_TIMER, nElapse, NULL);
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPopcheckDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CNewDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPopcheckDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);

	}
	else
	{
		CNewDialog::OnPaint();
	}

	bEnableOff=TRUE;
}

HCURSOR CPopcheckDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPopcheckDlg::Dispatch(LONG Param)
{
}

void CPopcheckDlg::CheckMail(BOOL bAll)
{
	GetTray()->SetIcon(AfxGetApp()->LoadIcon(IDI_MAILCHECK), "Проверяю почту");
	int mailn=0, mails=0;
	CPop3Connection mail;
//	mail.SetTimeout(60);

	CString sServer, sLogin, sPassword;

	sServer=pReg->AppGetValue("server", DEFAULT_SERVER);
	sLogin=pReg->AppGetValue("login", DEFAULT_USER);
	sPassword=pReg->AppGetValue("password", DEFAULT_PASSWORD);

	m_maillist.DeleteAllItems();

	if(mail.Connect(sServer, sLogin, sPassword))
	{
		long nPrevMails=pReg->AppGetNumericValue("Total");
		pReg->AppDeleteValue("Total");
		CStringArray csaIDs;
		for(long n=1;n<=nPrevMails;n++)
		{
				CString sKey;
				sKey.Format("MessageID%d", n);
				csaIDs.Add(pReg->AppGetValue(sKey,""));
				pReg->AppDeleteValue(sKey);
		}

		mail.Statistics(mailn, mails);
		CString msg;
		msg.Format("You have %d messages. Total size is %d", mailn, mails);
		int nFiltered=0;
		if(mailn>0)
		{
			for(int nm=1;nm<=mailn;nm++)
			{
				CPop3Message message;
				mail.GetMessageHeader(nm, message);
				CString header=message.GetMessageText();
				header.MakeLower();

				int from=header.Find(FROM)+lstrlen(FROM);
				int fromend=header.Find("\n", from);
				CString sfrom=header.Mid(from, fromend-from);
				sfrom.TrimLeft();sfrom.TrimRight();

				int qStart=sfrom.Find('"');
				int qEnd=sfrom.ReverseFind('"');
				int bStart=sfrom.ReverseFind('<');
				int bEnd=sfrom.ReverseFind('>');
				CString sSender=sfrom.Mid(qStart+1, qEnd-qStart-1);
				CString sMail=sfrom.Mid(bStart+1, bEnd-bStart-1);
				sSender.Replace("\\\"", "\"");
				sSender.Replace("\\\\", "\\");
				sSender.Replace("\\(", "(");
				sSender.Replace("\\)", ")");

				char* t=PRIORITY;
				int prior=header.Find(PRIORITY)+lstrlen(PRIORITY);
				int priorend=header.Find("\n", prior);
				CString sprior=header.Mid(prior, priorend-prior);
				sprior.TrimLeft();sprior.TrimRight();
				int nPrior=atoi(sprior.GetBuffer(sprior.GetLength()));

				int date=header.Find(DATE)+lstrlen(DATE);
				int dateend=header.Find("\n", date);
				CString sdate=header.Mid(date, dateend-date);
				sdate.TrimLeft();sdate.TrimRight();
				int subj=header.Find(SUBJ)+lstrlen(SUBJ);
				int subjend=header.Find("\n", subj);
				CString ssubj=header.Mid(subj, subjend-subj);
				ssubj.TrimLeft();ssubj.TrimRight();

				BOOL bFiltered=TRUE;

				CString sID, sKey;
				mail.GetMessageID(nm, sID);
				
				int nFilter=pReg->AppGetNumericValue("filter", DEFAULT_CHECKSTARTUP);
				if(nFilter==1)
				{
					for(int nI=0; nI<nPrevMails;nI++)
						if(!csaIDs.GetAt(nI).Compare(sID))
						{
							bFiltered=FALSE;
							break;
						}
				}

				if(bFiltered || bAll)
				{
					nFiltered++;
					int nItems=m_maillist.GetItemCount();
					CString sItems;
					sItems.Format("%d", nm);
					int nIcon=0;
					if(nPrior!=0 && nPrior!=3)
					{
						if(nPrior<3) nIcon=1;
						if(nPrior>3) nIcon=2;
					}
					m_maillist.InsertItem(nItems,sItems, nIcon);
					m_maillist.SetItemText(nItems,1,sSender.GetBuffer(sSender.GetLength()));
					m_maillist.SetItemText(nItems,2,sMail.GetBuffer(sMail.GetLength()));
					m_maillist.SetItemText(nItems,3,ssubj.GetBuffer(ssubj.GetLength()));
					m_maillist.SetItemText(nItems,4,sdate.GetBuffer(sdate.GetLength()));
				}

				if(nFiltered)
				{
					sKey.Format("MessageID%d", nFiltered);
					pReg->AppSetValue(sKey, sID);
				}
			}

			if(nFiltered || bAll)
			{
				CString str;
				if(mailn==1 || (mailn>20 && mailn%10==1)) str.Format("У вас %d непрочитаное письмо", mailn);
				if((mailn>=2 && mailn<=4) || (mailn>20 && (mailn>=2 && mailn<=4))) str.Format("У вас %d непрочитаных письма", mailn);
				if(mailn>=5 || (mailn>20 && mailn%10>=1)) str.Format("У вас %d непрочитаных писем", mailn);
				GetTray()->SetIcon(AfxGetApp()->LoadIcon(IDI_MAILON), str.GetBuffer(str.GetLength()));

				switch(pReg->AppGetNumericValue("notify", DEFAULT_CHECKSTARTUP))
				{
				case 1:
					ShowWindow(SW_SHOW);
					ShowWindow(SW_RESTORE);
					break;
				case 2:
					::PlaySound(pReg->AppGetValue("wave","") ,NULL, SND_FILENAME|SND_ASYNC);
					break;
				}
			}
		}
		else GetTray()->SetIcon(AfxGetApp()->LoadIcon(IDI_MAILOFF), "Жду почту");
		pReg->AppSetValue("Total", nFiltered);
		mail.Disconnect();
	}
	else GetTray()->SetIcon(AfxGetApp()->LoadIcon(IDI_MAILERROR), "Невозможно соедениться с сервером");
}

CPopcheckDlg::~CPopcheckDlg()
{
	delete pReg;
}

int CPopcheckDlg::DoModal() 
{
	return CNewDialog::DoModal();
}

LRESULT CPopcheckDlg::OnTrayNotification(WPARAM wp, LPARAM lp)
{
	if (lp!=WM_RBUTTONUP && lp!=WM_LBUTTONDBLCLK) return GetTray()->OnTrayNotification(wp, lp);

	// If there's a resource menu with the same ID as the icon, use it as 
	// the right-button popup menu. CTrayIcon will interprets the first
	// item in the menu as the default command for WM_LBUTTONDBLCLK
	// 
	CMenu menu;
	if (!menu.LoadMenu(IDR_TRAYICON))
		return 0;
	CMenu* pSubMenu = menu.GetSubMenu(0);
	if (!pSubMenu) 
		return 0;

	if (lp==WM_RBUTTONUP) {

		// Make first menu item the default (bold font)
		::SetMenuDefaultItem(pSubMenu->m_hMenu, 0, TRUE);

		// Display the menu at the current mouse location. There's a "bug"
		// (Microsoft calls it a feature) in Windows 95 that requires calling
		// SetForegroundWindow. To find out more, search for Q135788 in MSDN.
		//
		CPoint mouse;
		GetCursorPos(&mouse);
		::SetForegroundWindow(m_hWnd);	
		::TrackPopupMenu(pSubMenu->m_hMenu, 0, mouse.x, mouse.y, 0,
			m_hWnd, NULL);

	} else  // double click: execute first menu item
		::SendMessage(m_hWnd, WM_COMMAND, pSubMenu->GetMenuItemID(0), 0);

	return 1; // handled
}

CTrayIcon* CPopcheckDlg::GetTray()
{
	return ((CPopcheckApp*)AfxGetApp())->GetTray();
}

void CPopcheckDlg::OnSize(UINT nType, int cx, int cy) 
{
	WINDOWPLACEMENT wndpl;
	GetWindowPlacement(&wndpl);

	switch(nType)
	{
	case SIZE_MINIMIZED:
//		if(wndpl.showCmd!=SW_SHOWMINIMIZED)
		{
			ShowWindow(SW_MINIMIZE);
			ShowWindow(SW_HIDE);
		}
	break;
	case SIZE_MAXIMIZED:
//		if(wndpl.showCmd!=SW_SHOWMAXIMIZED)
		{
			ShowWindow(SW_SHOW);
			ShowWindow(SW_RESTORE);
		}
		break;
	}
	CNewDialog::OnSize(nType, cx, cy);
}

void CPopcheckDlg::OnExit() 
{
	OnCancel();
}

void CPopcheckDlg::OnCheckmail() 
{
	GetTray()->SetIcon(AfxGetApp()->LoadIcon(IDI_MAILOFF), "Проверяю почту");
	CheckMail();
}

void CPopcheckDlg::OnDrop() 
{
	GetTray()->SetIcon(AfxGetApp()->LoadIcon(IDI_MAILOFF), "Жду почту");
}

void CPopcheckDlg::OnAbout() 
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CPopcheckDlg::OnOptions() 
{
	HICON OldIcon=GetTray()->GetIcon();
	CString OldTip=GetTray()->GetTip();
	GetTray()->SetIcon(AfxGetApp()->LoadIcon(IDI_MAILOPEN), "Настройки");

	COptionsDlg options;
	
	options.m_login=pReg->AppGetValue("login", DEFAULT_USER);
	options.m_server=pReg->AppGetValue("server", DEFAULT_SERVER);
	options.m_password=pReg->AppGetValue("password", DEFAULT_PASSWORD);

	options.m_timervalue=pReg->AppGetNumericValue("timervalue", DEFAULT_TIMER);

	options.m_checkrestore=pReg->AppGetNumericValue("onrestore", DEFAULT_CHECKRESTORE);
	options.m_checkstart=pReg->AppGetNumericValue("onstartup", DEFAULT_CHECKSTARTUP);
	options.m_notifymethod=pReg->AppGetNumericValue("notify", DEFAULT_NOTIFY);
	options.m_showmethod=pReg->AppGetNumericValue("filter", DEFAULT_FILTER);
	options.m_autodrop=pReg->AppGetNumericValue("autodrop", DEFAULT_ADROP);

	options.m_wavename=pReg->AppGetValue("wave", "");

	if(options.DoModal()==IDOK)
	{
		pReg->AppSetValue("server", options.m_server);
		pReg->AppSetValue("login", options.m_login);
		pReg->AppSetValue("password", options.m_password);

		pReg->AppSetValue("timervalue", options.m_timervalue);

		pReg->AppSetValue("onrestore", options.m_checkrestore);
		pReg->AppSetValue("onstartup", options.m_checkstart);
		pReg->AppSetValue("notify", options.m_notifymethod);
		pReg->AppSetValue("filter", options.m_showmethod);
		pReg->AppSetValue("autodrop", options.m_autodrop);

		pReg->AppSetValue("wave", options.m_wavename);
	}
	GetTray()->SetIcon(OldIcon, OldTip);
}

void CPopcheckDlg::OnOpen() 
{
	ShowWindow(SW_SHOW);
	ShowWindow(SW_RESTORE);
	if(pReg->AppGetNumericValue("onrestore", DEFAULT_CHECKSTARTUP)) CheckMail();
	if(pReg->AppGetNumericValue("autodrop", DEFAULT_ADROP)) OnDrop();
}

void CPopcheckDlg::OnStopsounds() 
{
	::PlaySound(NULL,NULL, SND_FILENAME|SND_ASYNC);
}

BOOL CAboutDlg::OnInitDialog() 
{
	CNewDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPopcheckDlg::OnCheckall() 
{
	CheckMail(TRUE);
}

void CPopcheckDlg::OnDblclkMails(NMHDR* pNMHDR, LRESULT* pResult) 
{
	iLastSelectedItem=((NMLISTVIEW*)pNMHDR)->iItem;
	OnPopupView();
	*pResult = 0;
}

void CPopcheckDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==CHECK_TIMER) CheckMail();
	CNewDialog::OnTimer(nIDEvent);
}

BOOL CPopcheckDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class

	KillTimer(CHECK_TIMER);
	return CNewDialog::DestroyWindow();
}

HBRUSH CPopcheckDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr;
	switch(CTLCOLOR_DLG)
	{
	case CTLCOLOR_DLG:
//		hbr=CreateSolidBrush(RGB(255,255,255));
//		break;
	default:
		hbr = CNewDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BOOL CPopcheckDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CNewDialog::PreTranslateMessage(pMsg);
}

void CPopcheckDlg::OnRclickMails(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NMLISTVIEW* pNMList=(NMLISTVIEW*)pNMHDR;
	int iItem=pNMList->iItem;
	iLastSelectedItem=iItem;

	if(iItem>=0)
	{
		CMenu popMenu;
		popMenu.LoadMenu(IDM_POPUP);
		CMenu *pMenu=popMenu.GetSubMenu(0);
		POINT mouse;
		::SetMenuDefaultItem(pMenu->m_hMenu, 0, TRUE);
		::SetForegroundWindow(m_maillist.m_hWnd);	
		::GetCursorPos(&mouse);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN, mouse.x, mouse.y, this);
	}
	
	*pResult = 0;
}

void CPopcheckDlg::OnPopupView() 
{
	int iItem=iLastSelectedItem;
	char sNum[9];
	m_maillist.GetItemText(iItem, 0, sNum, 8);
	char sSender[1024];
	char sSubj[1024];
	m_maillist.GetItemText(iItem, 1, sSender, 1023);
	m_maillist.GetItemText(iItem, 3, sSubj, 1023);
	int iNum=atoi(sNum);

	CPop3Connection mail;

	CString sServer, sLogin, sPassword;
	sServer=pReg->AppGetValue("server", DEFAULT_SERVER);
	sLogin=pReg->AppGetValue("login", DEFAULT_USER);
	sPassword=pReg->AppGetValue("password", DEFAULT_PASSWORD);

	CPop3Message msg;
	if(mail.Connect(sServer, sLogin, sPassword))
	{
		cmdMailView.m_Title.Format("Письмо: \"%s\" от %s", sSubj, sSender);
		mail.GetMessageHeader(iNum, msg);
		cmdMailView.m_header=msg.GetMessageText();
		mail.Retrieve(iNum, msg);
		cmdMailView.m_message=msg.GetMessageText();
		cmdMailView.m_message.Replace(cmdMailView.m_header, "");
		cmdMailView.m_message.TrimLeft();
		cmdMailView.m_message.TrimRight();
		if(cmdMailView.DoModal()==IDOK)
			mail.Delete(iNum);
	}
}

void CPopcheckDlg::OnPopipRe() 
{
	int iItem=iLastSelectedItem;
	char sMail[1024];
	char sSubj[1024];
	m_maillist.GetItemText(iItem, 2, sMail, 1023);
	m_maillist.GetItemText(iItem, 3, sSubj, 1023);
	if(strlen(sSubj)==1023) strcpy(sSubj+1020, "...");
	
	CString sComm;
	sComm.Format("mailto:%s?subject=Re:%s", sMail, sSubj);
	ShellExecute(NULL, "open", sComm, NULL, NULL, SW_SHOWNORMAL);

}

void CPopcheckDlg::OnItemclickMails(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW *pLV = (NM_LISTVIEW *) pNMHDR;
	m_maillist.SortItems((PFNLVCOMPARE)Comparator, (LPARAM)&m_maillist);

	*pResult = 0;
}
