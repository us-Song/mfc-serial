


#ifndef _MFCSERIALDLG_H
#define _MFCSERIALDLG_H
#include "CMycomm.h"
#endif

class CMFCSerialDlg : public CDialogEx
{

public:
	CMFCSerialDlg(CWnd* pParent = NULL);

	CMycomm* m_comm;
	LRESULT OnThreadClosed(WPARAM length, LPARAM lpara);
	LRESULT OnReceive(WPARAM length, LPARAM lpara);

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	


protected:
	HICON m_hIcon;

	
	virtual BOOL OnInitDialog();

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
	afx_msg void OnBnClickedBtOn();
	afx_msg void OnBnClickedBtOff();
};
