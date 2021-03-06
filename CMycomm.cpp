
#include "pch.h"
#include "MFCSerial.h"
#include "CMycomm.h"

CMycomm::CMycomm()
{
	
}

CMycomm::~CMycomm()
{
	
	if (m_bIsOpenned)
		Close();

	delete m_pEvent;
}


CMycomm::CMycomm(CString port, CString baudrate, CString parity, CString databit, CString stopbit) 
{ 
	m_sComPort = port; 
	m_sBaudRate = baudrate; 
	m_sParity = "None"; 
	m_sDataBit = databit; 
	m_sStopBit = stopbit; 
	m_bFlowChk = 1; 
	m_bIsOpenned = FALSE; 
	m_nLength = 0; 
	memset(m_sInBuf, 0, MAXBUF * 2); 
	m_pEvent = new CEvent(FALSE, TRUE); 
}

void CMycomm::ResetSerial()
{
	DCB dcb; 
	DWORD DErr; 
	COMMTIMEOUTS CommTimeOuts; 
	if (!m_bIsOpenned) 
		return; 

	ClearCommError(m_hComDev, &DErr, NULL); 
	SetupComm(m_hComDev, InBufSize, OutBufSize); 
	PurgeComm(m_hComDev, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR); 
	
	// set up for overlapped I/O 
	CommTimeOuts.ReadIntervalTimeout = MAXDWORD; 
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0; 
	CommTimeOuts.ReadTotalTimeoutConstant = 0; 
	// CBR_9600 is approximately 1byte/ms. For our purposes, allow 
	// double the expected time per character for a fudge factor. 
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0; 
	CommTimeOuts.WriteTotalTimeoutConstant = 1000; 
	SetCommTimeouts(m_hComDev, &CommTimeOuts); 
	memset(&dcb, 0, sizeof(DCB)); 
	dcb.DCBlength = sizeof(DCB); 

	GetCommState(m_hComDev, &dcb); 

	dcb.fBinary = TRUE; 
	dcb.fParity = TRUE; 

	if (m_sBaudRate == "300") 
		dcb.BaudRate = CBR_300; 
	else if (m_sBaudRate == "9600") 
		dcb.BaudRate = CBR_9600; 
	else if (m_sBaudRate == "19200") 
		dcb.BaudRate = CBR_19200; 
	else if (m_sBaudRate == "38400") 
		dcb.BaudRate = CBR_38400; 

	if (m_sParity == "None") 
		dcb.Parity = NOPARITY; 
	else if (m_sParity == "Even") 
		dcb.Parity = EVENPARITY; 
	else if (m_sParity == "Odd") 
		dcb.Parity = ODDPARITY; 

	if (m_sDataBit == "7 Bit") 
		dcb.ByteSize = 7; 
	else if (m_sDataBit == "8 Bit") 
		dcb.ByteSize = 8; 

	if (m_sStopBit == "1 Bit") 
		dcb.StopBits = ONESTOPBIT; 
	else if (m_sStopBit == "1.5 Bit") 
		dcb.StopBits = ONE5STOPBITS; 
	else if (m_sStopBit == "2 Bit") 
		dcb.StopBits = TWOSTOPBITS; 

	dcb.fRtsControl = RTS_CONTROL_ENABLE; 
	dcb.fDtrControl = DTR_CONTROL_ENABLE; 
	dcb.fOutxDsrFlow = FALSE; 

	if (m_bFlowChk) 
	{ 
		dcb.fOutX = FALSE; 
		dcb.fInX = FALSE; 
		dcb.XonLim = 2048; 
		dcb.XoffLim = 1024; 
	} 
	else 
	{ 
		dcb.fOutxCtsFlow = TRUE; 
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE; 
	} 
	
	SetCommState(m_hComDev, &dcb); 
	SetCommMask(m_hComDev, EV_RXCHAR); 
}

void CMycomm::Close()
{ 
	if (!m_bIsOpenned) 
		return; 
	
	m_bIsOpenned = FALSE; 
	SetCommMask(m_hComDev, 0); 
	EscapeCommFunction(m_hComDev, CLRDTR); 
	PurgeComm(m_hComDev, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR); 
	Sleep(500); 
}

UINT CommThread(LPVOID lpData) 
{
	extern short g_nRemoteStatus; 
	DWORD ErrorFlags; 
	COMSTAT ComStat; 
	DWORD EvtMask; 
	char buf[MAXBUF]; 
	DWORD Length; 
	int size; 
	int insize = 0; 

	CMycomm* Comm = (CMycomm*)lpData;
	
	while (Comm->m_bIsOpenned) 
	{
		EvtMask = 0; 
		Length = 0; 
		insize = 0; 
		memset(buf, '\0', MAXBUF); 
		// 포트에 수신된 정보가 있을때까지 대기 
		WaitCommEvent(Comm->m_hComDev, &EvtMask, NULL); 
		ClearCommError(Comm->m_hComDev, &ErrorFlags, &ComStat); 
		if ((EvtMask & EV_RXCHAR) && ComStat.cbInQue) 
		{ 
			if (ComStat.cbInQue > MAXBUF) 
				size = MAXBUF; 
			else 
				size = ComStat.cbInQue; 
			do 
			{ 
				ClearCommError(Comm->m_hComDev, &ErrorFlags, &ComStat); 
				if (!ReadFile(Comm->m_hComDev, buf + insize, size, &Length, &(Comm->m_OLR))) 
				{ 
					// 에러 
					TRACE("Error in ReadFile\n"); 
					if (GetLastError() == ERROR_IO_PENDING) 
					{ 
						if (WaitForSingleObject(Comm->m_OLR.hEvent, 1000) != WAIT_OBJECT_0) 
							Length = 0; 
						else 
							GetOverlappedResult(Comm->m_hComDev, &(Comm->m_OLR), &Length, FALSE); } 
					else
						Length = 0; } 
				insize += Length; 
			} 
			while ((Length != 0) && (insize<size)); 
			ClearCommError(Comm->m_hComDev, &ErrorFlags, &ComStat); 
			
			if (Comm->m_nLength + insize > MAXBUF * 2) 
				insize = (Comm->m_nLength + insize) - MAXBUF * 2; 
			
			Comm->m_pEvent->ResetEvent(); 
			memcpy(Comm->m_sInBuf + Comm->m_nLength, buf, insize); 
			Comm->m_nLength += insize; 
			Comm->m_pEvent->SetEvent(); 
			LPARAM temp = (LPARAM)Comm; 
			
			SendMessage(Comm->m_hWnd, WM_MYRECEIVE, Comm->m_nLength, temp); 
		} 
	} 
	PurgeComm(Comm->m_hComDev, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR); 
	LPARAM temp = (LPARAM)Comm; 
	SendMessage(Comm->m_hWnd, WM_MYCLOSE, 0, temp); 
	return 0; 
}

void CMycomm::HandleClose() 
{ 
	CloseHandle(m_hComDev); 
	CloseHandle(m_OLR.hEvent); 
	CloseHandle(m_OLW.hEvent); 
}

BOOL CMycomm::Create(HWND hWnd) 
{ 
	m_hWnd = hWnd; 
	m_hComDev = CreateFile(m_sComPort, GENERIC_READ | GENERIC_WRITE, 
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
		NULL); 
	
	if (m_hComDev != INVALID_HANDLE_VALUE)
		m_bIsOpenned = TRUE; 
	else 
		return FALSE; 
	
	ResetSerial(); 
	
	m_OLW.Offset = 0; 
	m_OLW.OffsetHigh = 0;
	m_OLR.Offset = 0; 
	m_OLR.OffsetHigh = 0; 

	m_OLR.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); 
	
	if (m_OLR.hEvent == NULL) 
	{ 
		CloseHandle(m_OLR.hEvent); 
		return FALSE; 
	} 
	
	m_OLW.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); 
	
	if (m_OLW.hEvent == NULL) 
	{ 
		CloseHandle(m_OLW.hEvent); 
		return FALSE; 
	} 
	
	AfxBeginThread(CommThread, (LPVOID)this); 
	EscapeCommFunction(m_hComDev, SETDTR); 
	return TRUE; 
}

BOOL CMycomm::Send(LPCTSTR outbuf, int len)
{ 
	BOOL bRet = TRUE; 
	DWORD ErrorFlags; 
	COMSTAT ComStat; 
	DWORD BytesWritten; 
	DWORD BytesSent = 0; 

	ClearCommError(m_hComDev, &ErrorFlags, &ComStat); 
	
	char* temp; //변환한 wchar 저장

	int strSize = WideCharToMultiByte(CP_ACP, 0, outbuf, -1, NULL, 0, NULL, NULL);//유니코드->멀티바이트, wchar 사이즈구함

	temp = new char[strSize];//사이즈만큼 할당

	WideCharToMultiByte(CP_ACP, 0, outbuf, -1, temp, strSize, 0, 0);//형 변환
	

	if (!WriteFile(m_hComDev, temp, strlen(temp), &BytesWritten, &m_OLW)) //성공이면 true(1), 에러거나 비동기면 false(0)반환
	{ 
		if (GetLastError() == ERROR_IO_PENDING)//비동기 입출력 완료일 경우
		{ 
			if (WaitForSingleObject(m_OLW.hEvent, 1000) != WAIT_OBJECT_0) //시그널 상태가 되었을때 건너뜀
				bRet = FALSE; 
			else 
				GetOverlappedResult(m_hComDev, &m_OLW, &BytesWritten, FALSE); // 바이트수 얻는 함수,성공시 0이아닌 반환값, 실패시 0반환, write의 인수인 overlapped 구조체를 인수로 하여 write의 결과를 얻게해줌
		} 
		else 
			bRet = FALSE; //ignore error 
	} 
	
	ClearCommError(m_hComDev, &ErrorFlags, &ComStat); 
	
	return bRet; 
}

int CMycomm::Receive(LPSTR inbuf, int len) 
{
	CSingleLock lockObj((CSyncObject*)m_pEvent, FALSE); 

	if (len == 0) 
		return -1; 
	else if (len > MAXBUF) 
		return -1; 

	if (m_nLength == 0) 
	{ 
		inbuf[0] = '\0'; 
		return 0; 
	} 
	else if (m_nLength <= len) 
	{ 
		lockObj.Lock(); 
		memcpy(inbuf, m_sInBuf, m_nLength); 
		memset(m_sInBuf, 0, MAXBUF * 2); 
		int tmp = m_nLength; 
		m_nLength = 0; 
		lockObj.Unlock(); 
		return tmp; 
	} 
	else 
	{ 
		lockObj.Lock(); 
		memcpy(inbuf, m_sInBuf, len); 
		memmove(m_sInBuf, m_sInBuf + len, MAXBUF * 2 - len); 
		m_nLength -= len; 
		lockObj.Unlock(); 
		return len; 
	} 
} 

void CMycomm::Clear() 
{ 
	PurgeComm(m_hComDev, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR); 
	memset(m_sInBuf, 0, MAXBUF * 2); 
	m_nLength = 0; 
}


