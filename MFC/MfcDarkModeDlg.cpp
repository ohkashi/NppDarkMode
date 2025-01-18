
// MfcDarkModeDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MfcDarkMode.h"
#include "MfcDarkModeDlg.h"
#include "NppDarkMode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMfcDarkModeDlg dialog



CMfcDarkModeDlg::CMfcDarkModeDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_MFCDARKMODE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMfcDarkModeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMfcDarkModeDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CMfcDarkModeDlg message handlers

BOOL CMfcDarkModeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CString str;
	auto pCombo = (CComboBox*)GetDlgItem(IDC_COMBO1);
	auto pList = (CListBox*)GetDlgItem(IDC_LIST1);
	auto pListView = (CListCtrl*)GetDlgItem(IDC_LIST2);
	pListView->SetExtendedStyle(pListView->GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	pListView->InsertColumn(0, _T("Col:01"), LVCFMT_LEFT, 100);
	pListView->InsertColumn(1, _T("Col:02"), LVCFMT_LEFT, 100);
	for (int i = 0; i < 10; i++) {
		str.Format(_T("Item:%02d"), i + 1);
		pCombo->AddString(str);
		pList->AddString(str);
		pListView->InsertItem(i, str, 0);
		pListView->SetItem(i, 1, LVIF_TEXT, str, 0, 0, 0, 0);
	}
	pCombo->SetCurSel(0);
	auto pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN1);
	pSpin->SetRange(0, 999);
	auto pTree = (CTreeCtrl*)GetDlgItem(IDC_TREE2);
	pTree->SetExtendedStyle(TVS_EX_DOUBLEBUFFER, TVS_EX_DOUBLEBUFFER);
	HTREEITEM hRoot = pTree->InsertItem(_T("Root"));
	HTREEITEM hItem = pTree->InsertItem(_T("First"), hRoot);
	hItem = pTree->InsertItem(_T("Second"), hRoot);
	pTree->InsertItem(_T("Second:01"), hItem);
	pTree->InsertItem(_T("Second:02"), hItem);
	pTree->Expand(hItem, TVE_EXPAND);
	hItem = pTree->InsertItem(_T("Third"), hRoot);
	pTree->InsertItem(_T("Child"), hItem);
	pTree->Expand(hItem, TVE_EXPAND);
	pTree->Expand(hRoot, TVE_EXPAND);
	auto pProgress = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
	pProgress->SetRange(0, 100);
	pProgress->SetPos(70);
	pProgress->SetMarquee(TRUE, 20);

	if (NppDarkMode::isEnabled()) {
		NppDarkMode::setDarkTitleBar(m_hWnd);
		NppDarkMode::autoSubclassAndThemeChildControls(m_hWnd);
		NppDarkMode::autoSubclassAndThemeWindowNotify(m_hWnd);
		NppDarkMode::setDarkScrollBar(pTree->m_hWnd);
		SetWindowTheme(pProgress->m_hWnd, L"Explorer", nullptr);
		pProgress->SetBkColor(NppTheme::getInstance().getCurrentDefaultBgColor() + 0x080808);
		//PostMessage(NPPM_INTERNAL_REFRESHDARKMODE);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CMfcDarkModeDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::PreTranslateMessage(pMsg);
}

LRESULT CMfcDarkModeDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!NppDarkMode::isEnabled())
		return CDialog::DefWindowProc(message, wParam, lParam);

	switch (message) {
	case WM_ERASEBKGND:
		{
			CRect rc;
			GetClientRect(rc);
			HBRUSH hbr = NppDarkMode::getDarkerBackgroundBrush();
			::FillRect(reinterpret_cast<HDC>(wParam), &rc, hbr);
			return TRUE;
		}
		break;

	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORSTATIC:
		return NppDarkMode::onCtlColorDarker(reinterpret_cast<HDC>(wParam));

	case WM_PRINTCLIENT:
		return TRUE;

	case NPPM_INTERNAL_REFRESHDARKMODE:
		NppDarkMode::autoThemeChildControls(m_hWnd);
		return TRUE;
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CMfcDarkModeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMfcDarkModeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMfcDarkModeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
