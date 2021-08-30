#ifndef _CONTROLLER
#define _CONTOROLLER

class Controller : public CDialogEx
{
	

public:
	Controller(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~Controller();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	
	
};

#endif