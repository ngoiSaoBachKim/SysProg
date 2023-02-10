
// MFCApplication1Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCApplication1Dlg dialog



CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NAME, edit_name_);
	DDX_Control(pDX, IDC_EDIT_AGE, edit_age_);
	DDX_Control(pDX, IDC_COMBO_SEX, combo_sex_);
	DDX_Control(pDX, IDC_LIST, list_box_info_);
	DDX_Control(pDX, IDC_LIST_CTRL_INFO, list_ctrl_info);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CMFCApplication1Dlg::OnBnClickedButtonAdd)
END_MESSAGE_MAP()


// CMFCApplication1Dlg message handlers

BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	combo_sex_.AddString(_T("Male"));
	combo_sex_.AddString(_T("Female"));
	combo_sex_.SetCurSel(0);

	list_ctrl_info.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	list_ctrl_info.InsertColumn(0, _T("Sex"), LVCFMT_LEFT, 50);
	list_ctrl_info.InsertColumn(0, _T("Age"), LVCFMT_LEFT, 50);
	list_ctrl_info.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 150);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCApplication1Dlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCApplication1Dlg::OnBnClickedButtonAdd()
{
	UpdateData(TRUE);
	CString str_name_;
	edit_name_.GetWindowText(str_name_);

	CString str_age_;
	edit_age_.GetWindowText(str_age_);

	CString str_sex_;
	int idx = combo_sex_.GetCurSel();
	combo_sex_.GetLBText(idx, str_sex_);

	CString str_info;
	str_info = str_name_ + _T(" ");
	str_info += str_age_ + _T(" ");
	str_info += str_sex_;

	list_box_info_.AddString(str_info);

	list_ctrl_info.InsertItem(0, str_name_);
	list_ctrl_info.SetItemText(0, 1, str_age_);
	list_ctrl_info.SetItemText(0, 2, str_sex_);
	UpdateData(FALSE);
}
