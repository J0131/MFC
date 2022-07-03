
// ARQServer_thdDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "afxcoll.h"
#include "afxtempl.h"

typedef struct Frame
{
	int seq_num;
	int ack_num;
	int checksum;
	TCHAR p_buffer[256];
	Frame() {}
}Frame;

struct ThreadArg
{
	CList<Frame>* fList;
	CStringList* pList;
	CDialogEx* pDlg;
	int Thread_run;
};


class CDataSocket;

// CARQServer_thdDlg 대화 상자
class CARQServer_thdDlg : public CDialogEx
{
// 생성입니다.
public:
	CARQServer_thdDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ARQSERVER_THD_DIALOG };
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
	void ProcessClose(CDataSocket* pSocket, int nErrorCode);
	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedClose();
	CEdit m_edit4;
};
