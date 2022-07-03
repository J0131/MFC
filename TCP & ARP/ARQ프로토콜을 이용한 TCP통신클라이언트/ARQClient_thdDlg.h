
// ARQClient_thdDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
typedef struct Frame
{
	int seq_num=0;
	int ack_num=-1;
	int checksum;
	TCHAR p_buffer[256];
	Frame() {}
}Frame; // 프레임구조 정의

struct ThreadArg
{
	CList<Frame>* fList; // 프레임 리스트
	CStringList* pList;
	CDialogEx* pDlg;
	int Thread_run;
};

class CDataSocket;


// CARQClient_thdDlg 대화 상자
class CARQClient_thdDlg : public CDialogEx
{
// 생성입니다.
public:
	CARQClient_thdDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ARQCLIENT_THD_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edit1;
	CEdit m_edit3;
	CEdit m_edit2;
	CWinThread *pThread1, *pThread2;
	ThreadArg arg1, arg2;
	CDataSocket* m_pDataSocket;
	void ProcessReceive(CDataSocket* pSocket, int nErrorCode);
	CIPAddressCtrl m_ipaddr;
	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedClose();
	CEdit m_edit4;
};
