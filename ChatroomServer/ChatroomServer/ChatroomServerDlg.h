
// ChatroomServerDlg.h : header file
//

#pragma once


// CChatroomServerDlg dialog
class CChatroomServerDlg : public CDialogEx
{
// Construction
public:
	CChatroomServerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATROOMSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit3();
protected:
	CString editIP;
	CString editPort;
	CButton button1;
	CListCtrl listUser;
	CListCtrl listLog;
public:
	afx_msg void OnBnClickedButton1();
};
