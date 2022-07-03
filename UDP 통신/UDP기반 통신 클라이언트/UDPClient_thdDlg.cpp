
// UDPClient_thdDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "UDPClient_thd.h"
#include "UDPClient_thdDlg.h"
#include "afxdialogex.h"
#include "DataSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCriticalSection tx_cs;
CCriticalSection rx_cs;

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CUDPClient_thdDlg 대화 상자



CUDPClient_thdDlg::CUDPClient_thdDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_UDPCLIENT_THD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUDPClient_thdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_tx_edit_short);
	DDX_Control(pDX, IDC_EDIT3, m_tx_edit);
	DDX_Control(pDX, IDC_EDIT2, m_rx_edit);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ipaddr);
}

BEGIN_MESSAGE_MAP(CUDPClient_thdDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND, &CUDPClient_thdDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_Close, &CUDPClient_thdDlg::OnBnClickedClose)
END_MESSAGE_MAP()


// CUDPClient_thdDlg 메시지 처리기
UINT RXThread(LPVOID arg)
{
	ThreadArg *pArg = (ThreadArg *)arg; // 전달받은 인수 arg를 ThreadArg*형 pArg에 저장
	CStringList *plist = pArg->pList; // CStringList*형 plist에 pArg->pList를 저장
	CUDPClient_thdDlg *pDlg = (CUDPClient_thdDlg *)pArg->pDlg; // CTCPServer_thdDlg*형 pDlg에 (CTCPServer_thdDlg*)pArg->pDlg를 저장
	while (pArg->Thread_run) { // Thread_run이 1일동안 반복
		POSITION pos = plist->GetHeadPosition(); // pos저장
		POSITION current_pos; 
		while (pos != NULL) // pos 가 NULL이 아닐동안 반복
		{
			current_pos = pos; // pos를 current_pos에 넣어줌
			rx_cs.Lock(); // 임계구역 들어가기
			CString str = plist->GetNext(pos); // plist->GetNext(pos)를 str에 저장
			rx_cs.Unlock(); // 임계구역 나오기

			CString message;

			pDlg->m_rx_edit.GetWindowText(message);  // m_rx_edit를 읽어 message에 저장 
			message += str;
			pDlg->m_rx_edit.SetWindowTextW(message); // message를 m_rx_edit에 출력
			pDlg->m_rx_edit.LineScroll(pDlg->m_rx_edit.GetLineCount());

			plist->RemoveAt(current_pos);
		}
		Sleep(10);
	}
	return 0;
}

UINT TXThread(LPVOID arg)
{
	ThreadArg *pArg = (ThreadArg *)arg; // 전달받은 인수 arg를 ThreadArg*형 pArg에 저장
	CStringList *plist = pArg->pList; // CStringList*형 plist에 pArg->pList를 저장
	CUDPClient_thdDlg *pDlg = (CUDPClient_thdDlg *)pArg->pDlg; // CTCPServer_thdDlg*형 pDlg에 (CTCPServer_thdDlg*)pArg->pDlg를 저장

	UINT port = 8000; // 포트 선언
	CString addr = _T("127.0.0.1"); // addr선언

	while (pArg->Thread_run) // Thread_run이 1일동안 반복
	{
		POSITION pos = plist->GetHeadPosition(); // pos저장
		POSITION current_pos;

		while (pos != NULL) // pos 가 NULL이 아닐동안 반복
		{
			current_pos = pos; // pos를 current_pos에 넣어줌

			tx_cs.Lock(); // 임계구역 들어가기
			CString str = plist->GetNext(pos); // plist->GetNext(pos)를 str에 저장
			tx_cs.Unlock();	// 임계구역 나오기

			CString message;
			pDlg->m_tx_edit.GetWindowText(message); // m_tx_edit를 읽어 message에 저장 
			message += "\n";

			pDlg->m_tx_edit.SetWindowTextW(message); // message를 m_tx_edit에 출력
			pDlg->m_pDataSocket->SendToEx(str, (str.GetLength() + 1) * sizeof(TCHAR),port,addr,0); // str을 sendtoex함수를통해 해당포트와 주소의 소켓에 전송
			pDlg->m_tx_edit.LineScroll(pDlg->m_tx_edit.GetLineCount());

			plist->RemoveAt(current_pos); // current_pos의 원소 삭제
		}
		Sleep(10);
	}
	return 0;
}


BOOL CUDPClient_thdDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_pDataSocket = new CDataSocket(this);
	m_pDataSocket->Create(8001, SOCK_DGRAM); // datasocket udp형식으로 생성
	m_ipaddr.SetWindowTextW(_T("127.0.0.1")); // 

	CStringList* newlist = new CStringList;
	arg1.pList = newlist;
	arg1.Thread_run = 1;
	arg1.pDlg = this;

	CStringList* newlist2 = new CStringList;
	arg2.pList = newlist2;
	arg2.Thread_run = 1;
	arg2.pDlg = this; // 스레드 인수 초기화

	WSADATA wsa;
	int error_code;
	if ((error_code = WSAStartup(MAKEWORD(2, 2), &wsa)) != 0) {

		TCHAR buffer[256];
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 256, NULL);
		AfxMessageBox(buffer, MB_ICONERROR);
	}

	pThread1 = AfxBeginThread(TXThread, (LPVOID)&arg1); // tx스레드 시작
	pThread2 = AfxBeginThread(RXThread, (LPVOID)&arg2); // rx스레드 시작 

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CUDPClient_thdDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CUDPClient_thdDlg::OnPaint()
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
HCURSOR CUDPClient_thdDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUDPClient_thdDlg::ProcessReceive(CDataSocket* pSocket, int nErrorCode)
{
	TCHAR pBuf[1024 + 1];
	CString strData;
	int nbytes = 0;
	memset(pBuf, 0, 1025 * 2);

	CString addr; // addr선언
	UINT port = 8001; // port선언
	m_ipaddr.GetWindowTextW(addr);

	nbytes = pSocket->ReceiveFromEx(pBuf,sizeof(TCHAR)*1024,addr,port,0); // 해당 포트와 주소를 가진 소켓에서 데이터받아오기
	pBuf[nbytes] = NULL;
	strData = (LPCTSTR)pBuf;

	rx_cs.Lock();
	arg2.pList->AddTail((LPCTSTR)strData);
	rx_cs.Unlock();
}


void CUDPClient_thdDlg::OnBnClickedSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pDataSocket == NULL)	//m_pDataSocket이 NULL 일때
	{
		MessageBox(_T("서버에 접속 안 함!"), _T("알림"), MB_ICONERROR);
	}
	else
	{
		CString message;
		m_tx_edit_short.GetWindowTextW(message);
		message += _T("\r\n");
		tx_cs.Lock();
		arg1.pList->AddTail(message);
		tx_cs.Unlock();
		m_tx_edit_short.SetWindowTextW(_T(""));
		m_tx_edit_short.SetFocus();

		int len = m_tx_edit.GetWindowTextLengthW();
		m_tx_edit.SetSel(len, len);
		m_tx_edit.ReplaceSel(message);
	}
}

void CUDPClient_thdDlg::OnBnClickedClose()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pDataSocket == NULL)
	{
		MessageBox(_T("서버에 접속 안함!"), _T("알림"), MB_ICONINFORMATION);
	}
	else
	{
		arg1.Thread_run = 0;
		arg2.Thread_run = 0;

		m_pDataSocket->Close();
		delete m_pDataSocket;
		m_pDataSocket = NULL;

		int len = m_rx_edit.GetWindowTextLengthW();
		CString message = _T("\n### 접속 종료 ###\r\n\r\n");

		m_rx_edit.SetSel(len, len);
		m_rx_edit.ReplaceSel(message);
	}

}
