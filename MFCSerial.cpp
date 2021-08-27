
#include "pch.h"
#include "framework.h"
#include "MFCSerial.h"
#include "MFCSerialDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CMFCSerialApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CMFCSerialApp::CMFCSerialApp()
{
	
}

CMFCSerialApp theApp;

BOOL CMFCSerialApp::InitInstance()
{
	CWinApp::InitInstance();

	// MFC 컨트롤의 테마를 사용하기 위해 "Windows 원형" 비주얼 관리자 활성화
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	CMFCSerialDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	return FALSE;
}

