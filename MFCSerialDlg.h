
// MFCSerialDlg.h: 헤더 파일
//

#pragma once
#include "CMycomm.h"

// CMFCSerialDlg 대화 상자
class CMFCSerialDlg : public CDialogEx
{
// 생성입니다.
public:
	CMFCSerialDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	CMycomm* m_comm;
	LRESULT OnThreadClosed(WPARAM length, LPARAM lpara);
	LRESULT OnReceive(WPARAM length, LPARAM lpara);
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCSERIAL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	BOOL comport_state;
	CComboBox m_Combo_Comport_List;
	CString m_Str_Comport;
	CComboBox m_Combo_Baudrate_List;
	CString m_Str_Baudrate;
	CEdit m_Edit_Rcv_View;
	CEdit m_Edit_Send_Data;
	afx_msg void OnBnClickedBtConnect();
	afx_msg void OnBnClickedBtSend();
};
