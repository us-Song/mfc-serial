
#pragma once
#include "afxwin.h" 
#define MAXBUF 50000 
#define InBufSize 50000 
#define OutBufSize 50000 
#define ASCII_XON 0x11 
#define ASCII_XOFF 0x13 
#define WM_MYRECEIVE (WM_USER+1) //데이터 수신 메시지 
#define WM_MYCLOSE (WM_USER+2) // 종료 메시지



class CMycomm : public CCmdTarget
{
public: 
	CMycomm();
	~CMycomm();

public: 
	HANDLE m_hComDev; 
	HWND m_hWnd; 
	CString m_sComPort; 
	CString m_sBaudRate; 
	CString m_sParity; 
	CString m_sDataBit; 
	CString m_sStopBit; 
	BOOL m_bFlowChk; 
	BOOL m_bIsOpenned;
	OVERLAPPED m_OLR, m_OLW; 
	char m_sInBuf[MAXBUF * 2]; 
	int m_nLength; 
	CEvent* m_pEvent; 
public: 
	void Clear(); 
	int Receive(LPSTR inbuf, int len); 
	BOOL Send(LPCTSTR outbuf, int len); 
	BOOL Create(HWND hWnd); 
	void HandleClose(); void Close(); 
	void ResetSerial(); 
	CMycomm(CString port, CString baudrate, CString parity, CString databit, CString stopbit);
};




