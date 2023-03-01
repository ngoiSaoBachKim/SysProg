
// ProcessViewerDlg.cpp : implementation file
//

#include <Windows.h>
#include "pch.h"
#include "framework.h"
#include "ProcessViewer.h"
#include "ProcessViewerDlg.h"
#include "afxdialogex.h"
#include <tlhelp32.h>
#include <processthreadsapi.h>
#include <winternl.h>
#include <psapi.h>

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


// CProcessViewerDlg dialog



CProcessViewerDlg::CProcessViewerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROCESSVIEWER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcessViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON, buttonRefresh);
	DDX_Control(pDX, IDC_LIST, listCtrlProcess);
}

BEGIN_MESSAGE_MAP(CProcessViewerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON, &CProcessViewerDlg::OnBnClickedButton)
END_MESSAGE_MAP()


// CProcessViewerDlg message handlers

BOOL CProcessViewerDlg::OnInitDialog()
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
	listCtrlProcess.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	listCtrlProcess.InsertColumn(0, _T("PID"), LVCFMT_LEFT, 64);
	listCtrlProcess.InsertColumn(1, _T("Process Name"), LVCFMT_LEFT, 200);
	listCtrlProcess.InsertColumn(2, _T("Command line"), LVCFMT_LEFT, 400);
	listCtrlProcess.InsertColumn(3, _T("Full path"), LVCFMT_LEFT, 400);
	FetchProcess();
	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CProcessViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CProcessViewerDlg::OnPaint()
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
HCURSOR CProcessViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

typedef NTSTATUS(NTAPI* FuncPointer)(
	HANDLE ProcessHandle,
	DWORD ProcessInformationClass,
	PVOID ProcessInformation,
	DWORD ProcessInformationLength,
	PDWORD ReturnLength
	);

void CProcessViewerDlg::FetchProcess() {
	PROCESSENTRY32 processEntry;
	int listIndex = 0;
	CString pID;
	HANDLE processHandle;
	PROCESS_BASIC_INFORMATION processBasicInfo;
	PVOID rtlUserProcParamsAddress;
	UNICODE_STRING commandLineUnicodeString;
	TCHAR* commandLineBuffer;
	CString commandLineString;
	TCHAR filePath[256];
	CString filePathString;

	HANDLE snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshotHandle == INVALID_HANDLE_VALUE) {
		return;
	}

	processEntry.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(snapshotHandle, &processEntry)) {
		CloseHandle(snapshotHandle);
		return;
	}

	do {
		listIndex = listCtrlProcess.GetItemCount();
		pID.Format(_T("%d"), processEntry.th32ProcessID);
		listCtrlProcess.InsertItem(listIndex, pID);
		//print current processHandle PID
		listCtrlProcess.SetItemText(listIndex, 1, processEntry.szExeFile);

		processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processEntry.th32ProcessID);

		//get NTQueryInformationProcess function from ntdll.dll
		FuncPointer ntQueryInformationProcess = (FuncPointer)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
		ntQueryInformationProcess(processHandle, ProcessBasicInformation, &processBasicInfo, sizeof(processBasicInfo), NULL);

		//get and print current processHandle command line 
		if (ReadProcessMemory(processHandle, &(((_PEB*)processBasicInfo.PebBaseAddress)->ProcessParameters), &rtlUserProcParamsAddress, sizeof(PVOID), NULL)) {
			if (ReadProcessMemory(processHandle, &(((_RTL_USER_PROCESS_PARAMETERS*)rtlUserProcParamsAddress)->CommandLine), &commandLineUnicodeString, sizeof(commandLineUnicodeString), NULL)) {
				commandLineBuffer = (WCHAR*)malloc(commandLineUnicodeString.Length);
				if (ReadProcessMemory(processHandle, commandLineUnicodeString.Buffer, commandLineBuffer, commandLineUnicodeString.Length, NULL)) {
					commandLineString.Format(_T("%.*s"), commandLineUnicodeString.Length / 2, commandLineBuffer);
					listCtrlProcess.SetItemText(listIndex, 2, commandLineString);
				}
				free(commandLineBuffer);
			}
		}
		filePathString.Format(_T("%.*s"), GetModuleFileNameEx(processHandle, 0, filePath, 255), filePath);
		//print current processHandle path
		listCtrlProcess.SetItemText(listIndex, 3, filePathString);
		CloseHandle(processHandle);

	} while (Process32Next(snapshotHandle, &processEntry));

	CloseHandle(snapshotHandle);
}


void CProcessViewerDlg::OnBnClickedButton()
{
	UpdateData(TRUE);
	listCtrlProcess.DeleteAllItems();
	FetchProcess();
	UpdateData(FALSE);
}
