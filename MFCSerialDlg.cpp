
#include "pch.h"
#include "framework.h"
#include "MFCSerial.h"
#include "MFCSerialDlg.h"
#include "afxdialogex.h"
#include "CMycomm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CMFCSerialDlg::CMFCSerialDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCSERIAL_DIALOG, pParent)
	, m_Str_Comport(_T(""))
	, m_Str_Baudrate(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCSerialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1_COMPORT, m_Combo_Comport_List);
	DDX_CBString(pDX, IDC_COMBO1_COMPORT, m_Str_Comport);
	DDX_Control(pDX, IDC_COMBO2_BAUDRATE, m_Combo_Baudrate_List);
	DDX_CBString(pDX, IDC_COMBO2_BAUDRATE, m_Str_Baudrate);
	DDX_Control(pDX, IDC_EDIT_RCV_VIEW, m_Edit_Rcv_View);
	DDX_Control(pDX, IDC_EDIT_SEND_DATA, m_Edit_Send_Data);
}

BEGIN_MESSAGE_MAP(CMFCSerialDlg, CDialogEx)
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BT_CONNECT, &CMFCSerialDlg::OnBnClickedBtConnect)
	ON_BN_CLICKED(IDC_BT_SEND, &CMFCSerialDlg::OnBnClickedBtSend)

	ON_MESSAGE(WM_MYCLOSE, &CMFCSerialDlg::OnThreadClosed)
	ON_MESSAGE(WM_MYRECEIVE, &CMFCSerialDlg::OnReceive)
	ON_BN_CLICKED(IDC_BT_ON, &CMFCSerialDlg::OnBnClickedBtOn)
	ON_BN_CLICKED(IDC_BT_OFF, &CMFCSerialDlg::OnBnClickedBtOff)
END_MESSAGE_MAP()


BOOL CMFCSerialDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Combo_Comport_List.AddString(_T("COM3"));
	m_Combo_Comport_List.AddString(_T("COM4"));
	m_Combo_Baudrate_List.AddString(_T("9600"));
	m_Combo_Baudrate_List.AddString(_T("19200"));
	m_Combo_Baudrate_List.AddString(_T("38400"));

	comport_state = false;

	GetDlgItem(IDC_BT_CONNECT)->SetWindowTextW(_T("OPEN"));

	m_Str_Comport = _T("COM3");
	m_Str_Baudrate = _T("38400");

	UpdateData(FALSE);

	return TRUE; 
}


HCURSOR CMFCSerialDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CMFCSerialDlg::OnThreadClosed(WPARAM length, LPARAM lpara)
{
	((CMycomm*)lpara);
	delete ((CMycomm*)lpara);
	return 0;
}


LRESULT CMFCSerialDlg::OnReceive(WPARAM length, LPARAM lpara)
{
	CString str;
	char data[10000];
	if (m_comm)
	{
		m_comm->Receive(data, length);
		data[length] = _T('\0');
		str += _T("\r\n");
		for (int i = 0; i < length; i++)
		{
			str += data[i];
		}
		m_Edit_Rcv_View.ReplaceSel(str);
		m_Edit_Rcv_View.LineScroll(m_Edit_Rcv_View.GetLineCount());
		str = "";
		
	}
}


void CMFCSerialDlg::OnBnClickedBtConnect()
{

	if (comport_state)
	{
		if (m_comm)
		{
			m_comm->Close();
			m_comm = NULL;
			AfxMessageBox(_T("COM 포트닫힘"));
			comport_state = false;
			GetDlgItem(IDC_BT_CONNECT)->SetWindowTextW(_T("Open"));
			GetDlgItem(IDC_BT_SEND)->EnableWindow(false);
		}
	}
	else
	{
		m_comm = new CMycomm(m_Str_Comport, m_Str_Baudrate, _T("NONE"), _T("8 Bit"), _T("1 Bit"));
		if (m_comm->Create(GetSafeHwnd()) != 0)
		{
			AfxMessageBox(_T("COM 포트열림"));
			comport_state = true;
			GetDlgItem(IDC_BT_CONNECT)->SetWindowTextW(_T("CLOSE"));
			GetDlgItem(IDC_BT_SEND)->EnableWindow(true);
		}
		else
		{
			AfxMessageBox(_T("ERROR!"));
		}
	}
}


void CMFCSerialDlg::OnBnClickedBtSend()
{
	CString str;
	GetDlgItem(IDC_EDIT_SEND_DATA)->GetWindowText(str);
	str += "\r\n";
	m_comm->Send(str, str.GetLength());
	AfxMessageBox(str);
}

void CMFCSerialDlg::OnBnClickedBtOn()
{
	CString str;
	str = "setbrightness 0 50\r\n";
	m_comm->Send(str, str.GetLength());

}

void CMFCSerialDlg::OnBnClickedBtOff()
{
	CString str;
	str = "setbrightness 0 0\r\n";
	m_comm->Send(str, str.GetLength());

}
