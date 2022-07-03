
// ARQClient_thdDlg.h : ��� ����
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
}Frame; // �����ӱ��� ����

struct ThreadArg
{
	CList<Frame>* fList; // ������ ����Ʈ
	CStringList* pList;
	CDialogEx* pDlg;
	int Thread_run;
};

class CDataSocket;


// CARQClient_thdDlg ��ȭ ����
class CARQClient_thdDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CARQClient_thdDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ARQCLIENT_THD_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
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
