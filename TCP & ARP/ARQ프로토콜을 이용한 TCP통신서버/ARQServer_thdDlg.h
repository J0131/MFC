
// ARQServer_thdDlg.h : ��� ����
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

// CARQServer_thdDlg ��ȭ ����
class CARQServer_thdDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CARQServer_thdDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ARQSERVER_THD_DIALOG };
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
	void ProcessClose(CDataSocket* pSocket, int nErrorCode);
	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedClose();
	CEdit m_edit4;
};
