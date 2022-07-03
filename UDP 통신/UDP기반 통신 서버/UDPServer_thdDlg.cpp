
// UDPServer_thdDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "UDPServer_thd.h"
#include "UDPServer_thdDlg.h"
#include "afxdialogex.h"
#include "DataSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCriticalSection tx_cs;
CCriticalSection rx_cs;

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUDPServer_thdDlg ��ȭ ����



CUDPServer_thdDlg::CUDPServer_thdDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_UDPSERVER_THD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUDPServer_thdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_tx_edit_short);
	DDX_Control(pDX, IDC_EDIT3, m_tx_edit);
	DDX_Control(pDX, IDC_EDIT2, m_rx_edit);
}

BEGIN_MESSAGE_MAP(CUDPServer_thdDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND, &CUDPServer_thdDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_DISCONNECT, &CUDPServer_thdDlg::OnBnClickedDisconnect)
END_MESSAGE_MAP()


// CUDPServer_thdDlg �޽��� ó����
UINT RXThread(LPVOID arg)
{
	ThreadArg* pArg = (ThreadArg*)arg; // ���޹��� �μ� arg�� ThreadArg*�� pArg�� ����
	CStringList*plist = pArg->pList; // CStringList*�� plist�� pArg->pList�� ����
	CUDPServer_thdDlg *pDlg = (CUDPServer_thdDlg*)pArg->pDlg; // CTCPServer_thdDlg*�� pDlg�� (CTCPServer_thdDlg*)pArg->pDlg�� ����
	while (pArg->Thread_run) { // Thread_run�� 1�ϵ��� �ݺ�
		POSITION pos = plist->GetHeadPosition(); // pos����
		POSITION current_pos;
		while (pos != NULL) // pos �� NULL�� �ƴҵ��� �ݺ�
		{
			current_pos = pos; // pos�� current_pos�� �־���
			rx_cs.Lock();  // �Ӱ豸�� ����
			CString str = plist->GetNext(pos); // plist->GetNext(pos)�� str�� ����
			rx_cs.Unlock(); // �Ӱ豸�� ������

			CString message;

			pDlg->m_rx_edit.GetWindowText(message); // m_rx_edit�� �о� message�� ���� 
			message += str;
			message += "\n";
			pDlg->m_rx_edit.SetWindowText(message); // message�� m_rx_edit�� ���
			pDlg->m_rx_edit.LineScroll(pDlg->m_rx_edit.GetLineCount());

			plist->RemoveAt(current_pos);
		}
		Sleep(10);
	}

	return 0;
}

UINT TXThread(LPVOID arg)
{
	ThreadArg* pArg = (ThreadArg*)arg;	// ���޹��� �μ� arg�� ThreadArg*�� pArg�� ����
	CStringList* plist = pArg->pList; // CStringList*�� plist�� pArg->pList�� ����
	CUDPServer_thdDlg* pDlg = (CUDPServer_thdDlg*)pArg->pDlg; // CTCPServer_thdDlg*�� pDlg�� (CTCPServer_thdDlg*)pArg->pDlg�� ����

	UINT port; // ��Ʈ ����
	CString addr = _T("127.0.0.1"); // addr����

	while (pArg->Thread_run) // Thread_run�� 1�ϵ��� �ݺ�
	{
		POSITION pos = plist->GetHeadPosition(); // pos����
		POSITION current_pos;

		while (pos != NULL) { // pos �� NULL�� �ƴҵ��� �ݺ�
			current_pos = pos; // pos�� current_pos�� �־���
			pDlg->m_pDataSocket->GetPeerName(addr, port); // ����� ������ addr�� port������ ����

			rx_cs.Lock(); // �Ӱ豸�� ����
			CString str = plist->GetNext(pos); // plist->GetNext(pos)�� str�� ����
			rx_cs.Unlock();	// �Ӱ豸�� ������

			CString message;
			pDlg->m_tx_edit.GetWindowText(message);	// m_tx_edit�� �о� message�� ���� 
													//message += "\r\n";

			pDlg->m_tx_edit.SetWindowTextW(message); // message�� m_tx_edit�� ���
			pDlg->m_pDataSocket->SendToEx(str, (str.GetLength()) * sizeof(TCHAR),port,addr,0); // str�� sendtoex�Լ������� �ش���Ʈ�� �ּ��� ���Ͽ� ����
			pDlg->m_tx_edit.LineScroll(pDlg->m_tx_edit.GetLineCount());

			plist->RemoveAt(current_pos); // current_pos�� ���� ����
		}
		Sleep(10);
	}
	return 0;
}
BOOL CUDPServer_thdDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	CStringList* newlist = new CStringList;
	arg1.pList = newlist;
	arg1.Thread_run = 1;
	arg1.pDlg = this;

	CStringList* newlist2 = new CStringList;
	arg2.pList = newlist2;
	arg2.Thread_run = 1;
	arg2.pDlg = this;

	WSADATA wsa;
	int error_code;
	if((error_code = WSAStartup(MAKEWORD(2, 2), &wsa)) != 0) {
		TCHAR buffer[256];
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 256, NULL);
		AfxMessageBox(buffer, MB_ICONERROR);
	}

	m_pDataSocket = new CDataSocket(this);
	if (m_pDataSocket->Create(8000, SOCK_DGRAM)) // datasocket udp�������� ����
	{
		AfxMessageBox(_T("������ �����մϴ�."), MB_ICONINFORMATION);
		m_pDataSocket->Connect(_T("127.0.0.1"), 8001);
		pThread1 = AfxBeginThread(TXThread, (LPVOID)&arg1); // tx������ ����
		pThread2 = AfxBeginThread(RXThread, (LPVOID)&arg2); // rx������ ���� 
		return TRUE;
	}

	return FALSE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CUDPServer_thdDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CUDPServer_thdDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CUDPServer_thdDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUDPServer_thdDlg::ProcessReceive(CDataSocket* pSocket, int nErrorCode)
{
	TCHAR pBuf[1024 + 1];
	CString strData;
	int nbytes;

	UINT port = 8000; // addr����
	CString addr = _T("127.0.0.1"); // port����

	nbytes = pSocket->ReceiveFromEx(pBuf, sizeof(TCHAR)*1024,addr,port,0); // �ش� ��Ʈ�� �ּҸ� ���� ���Ͽ��� �����͹޾ƿ���
	pBuf[nbytes] = NULL;
	strData = (LPCTSTR)pBuf;

	rx_cs.Lock();
	arg2.pList->AddTail((LPCTSTR)strData);
	rx_cs.Unlock();
}



void CUDPServer_thdDlg::ProcessClose(CDataSocket* pSocket, int nErrorCode)
{
	pSocket->Close();
	delete m_pDataSocket;
	m_pDataSocket = NULL;

	int len = m_rx_edit.GetWindowTextLengthW();
	CString message = _T("### ���� ���� ###\r\n\r\n");
	m_rx_edit.SetSel(len, len);
	m_rx_edit.ReplaceSel(message);
}


void CUDPServer_thdDlg::OnBnClickedSend()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString tx_message;
	m_tx_edit_short.GetWindowText(tx_message);
	tx_message += _T("\r\n");
	tx_cs.Lock();
	arg1.pList->AddTail(tx_message);
	tx_cs.Unlock();
	m_tx_edit_short.SetWindowText(_T(""));
	m_tx_edit_short.SetFocus();

	int len = m_tx_edit.GetWindowTextLengthW();
	m_tx_edit.SetSel(len, len);
	m_tx_edit.ReplaceSel(tx_message);

}


void CUDPServer_thdDlg::OnBnClickedDisconnect()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_pDataSocket == NULL) {
		AfxMessageBox(_T("�̹� ���� ����"));
	}
	else {
		arg1.Thread_run = 0;
		arg2.Thread_run = 0;
		m_pDataSocket->Close();
		delete m_pDataSocket;
		m_pDataSocket = NULL;
	}
}
