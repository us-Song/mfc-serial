
// MFCSerialDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFCSerial.h"
#include "MFCSerialDlg.h"
#include "afxdialogex.h"
#include "CMycomm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCSerialDlg 대화 상자



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
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BT_CONNECT, &CMFCSerialDlg::OnBnClickedBtConnect)
	ON_BN_CLICKED(IDC_BT_SEND, &CMFCSerialDlg::OnBnClickedBtSend)

	ON_MESSAGE(WM_MYCLOSE, &CMFCSerialDlg::OnThreadClosed)
	ON_MESSAGE(WM_MYRECEIVE, &CMFCSerialDlg::OnReceive)
	ON_BN_CLICKED(IDC_BT_ON, &CMFCSerialDlg::OnBnClickedBtOn)
	ON_BN_CLICKED(IDC_BT_OFF, &CMFCSerialDlg::OnBnClickedBtOff)
END_MESSAGE_MAP()


// CMFCSerialDlg 메시지 처리기

BOOL CMFCSerialDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

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

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCSerialDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
