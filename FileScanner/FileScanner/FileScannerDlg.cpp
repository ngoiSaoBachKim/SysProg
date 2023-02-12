
// FileScannerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "FileScanner.h"
#include "FileScannerDlg.h"
#include "afxdialogex.h"
#include <stack>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFileScannerDlg dialog



CFileScannerDlg::CFileScannerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILESCANNER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileScannerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, fileList);
	DDX_Control(pDX, IDC_EDIT_PATH, editPath);
	DDX_Control(pDX, IDC_EDIT_NAME, editName);
	DDX_Control(pDX, IDC_BUTTON, button);
}

BEGIN_MESSAGE_MAP(CFileScannerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON, &CFileScannerDlg::OnBnClickedButton)
END_MESSAGE_MAP()


// CFileScannerDlg message handlers

BOOL CFileScannerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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
	fileList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	fileList.InsertColumn(0, _T("File Name"), LVCFMT_LEFT, 176);
	fileList.InsertColumn(1, _T("Size(Byte)"), LVCFMT_LEFT, 90);
	fileList.InsertColumn(2, _T("Full path"), LVCFMT_LEFT, 380);
	fileList.InsertColumn(3, _T("Modified date"), LVCFMT_LEFT, 120);
	fileList.InsertColumn(3, _T("Created date"), LVCFMT_LEFT, 120);
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFileScannerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFileScannerDlg::OnPaint()
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
HCURSOR CFileScannerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFileScannerDlg::OnBnClickedButton()
{
	UpdateData(TRUE);
	fileList.DeleteAllItems();

	CString neededFilePath;
	CString neededFileName;
	CString currentFolder;
	CString outputBuffer;

	editPath.GetWindowText(neededFilePath);
	editName.GetWindowText(neededFileName);

	if (neededFilePath.IsEmpty()) {
		MessageBox((LPCWSTR)L"File path empty, try again.", (LPCWSTR)L"Error", MB_ICONWARNING | MB_DEFBUTTON2);
		UpdateData(FALSE);
		return;
	}
	std::stack<CString> folderStack;
	folderStack.push(neededFilePath);
	WIN32_FIND_DATA findData;
	HANDLE hInitFind = FindFirstFile(neededFilePath + _T("\\*"), &findData);
	if (hInitFind == INVALID_HANDLE_VALUE) {
		MessageBox((LPCWSTR)L"File path not exist, try again.", (LPCWSTR)L"Error", MB_ICONWARNING | MB_DEFBUTTON2);
		UpdateData(FALSE);
		return;
	}
	while (!folderStack.empty()) {
		currentFolder = folderStack.top();
		folderStack.pop();

		CString searchPath = currentFolder + _T("\\*");
		WIN32_FIND_DATA findData;
		HANDLE hFind = FindFirstFile(searchPath, &findData);
		{
			do {
				CString fileName = findData.cFileName;
				if (fileName == _T(".") || fileName == _T("..")) {
					continue;
				}

				CString fullPath = currentFolder + _T("\\") + fileName;
				if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					folderStack.push(fullPath);

				}
				else if (fileName.Find(neededFileName) != -1) {
					fileList.InsertItem(0, fileName);
					outputBuffer.Format(_T("%ld"), ((__int64)findData.nFileSizeLow + ((__int64)findData.nFileSizeHigh << 32)));
					fileList.SetItemText(0, 1, outputBuffer);
					fileList.SetItemText(0, 2, fullPath);

					FILETIME ft = findData.ftLastWriteTime;
					WCHAR dateTime[100];
					SHFormatDateTimeW(&ft, NULL, dateTime, 100);
					fileList.SetItemText(0, 4, dateTime);

					ft = findData.ftCreationTime;
					SHFormatDateTimeW(&ft, NULL, dateTime, 100);
					fileList.SetItemText(0, 3, dateTime);

				}
			} while (FindNextFile(hFind, &findData));

			FindClose(hFind);
		}
		//MessageBox((LPCWSTR)fileList.GetEmptyText(), (LPCWSTR)L"Error", MB_ICONWARNING | MB_DEFBUTTON2);
		UpdateData(FALSE);
	}
}
/*
stack of path;
push path

while stack !empty
	currentPath = stack pop
	hFile = findfirstfile
	if valid
		do
			if directory
				push currentPath + directory name
				continue
			else
				if currentFileName != neededFileName
					continue
				currentFileName.printNeededInfomation
		while FindNextFile(hFile)
	else error
*/