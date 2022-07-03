
// ARQClient_thdDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ARQClient_thd.h"
#include "ARQClient_thdDlg.h"
#include "afxdialogex.h"
#include "DataSocket.h"
#include <stdlib.h>
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCriticalSection tx_cs;
CCriticalSection rx_cs;
CString message3,message4;
int ack=-1;
// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
int find_checksum(TCHAR* t,int chk);
CString re;
char send_flag=0;
int seq;
CString co1,co2,con2,con_2,con3,con_3,co4,co5;

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


// CARQClient_thdDlg 대화 상자



CARQClient_thdDlg::CARQClient_thdDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ARQCLIENT_THD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CARQClient_thdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit1);
	DDX_Control(pDX, IDC_EDIT3, m_edit3);
	DDX_Control(pDX, IDC_EDIT2, m_edit2);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ipaddr);
	DDX_Control(pDX, IDC_EDIT4, m_edit4);
}

BEGIN_MESSAGE_MAP(CARQClient_thdDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND, &CARQClient_thdDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_CLOSE, &CARQClient_thdDlg::OnBnClickedClose)
END_MESSAGE_MAP()

UINT RXThread(LPVOID arg)
{
	ThreadArg *pArg = (ThreadArg *)arg; // 전달받은 인수 arg를 ThreadArg*형 pArg에 저장
	CList<Frame>* flist = pArg->fList; // CList<Frame>*형 flist에 pArg->pList를 저장
	CARQClient_thdDlg *pDlg = (CARQClient_thdDlg *)pArg->pDlg; // CARQClient_thdDlg*형 pDlg에 (CARQClient_thdDlg *)pArg->pDlg를 저장
	UINT port = 8000; // 포트 선언
	CString addr = _T("127.0.0.1"); // addr선언
	while (pArg->Thread_run) { // Thread_run이 1일동안 반복
		POSITION pos = flist->GetHeadPosition(); // flist의 처음position을 pos에저장
		POSITION current_pos;
		while (pos != NULL) // pos 가 NULL이 아닐동안 반복
		{
			current_pos = pos; // pos를 current_pos에 넣어줌
			rx_cs.Lock();  // 임계구역 들어가기
			Frame frame = flist->GetNext(pos); // flist의 pos위치 원소를 frame에저장후 pos증가
			TCHAR* message1 = frame.p_buffer; // frame의 p_buffer를 message1에 저장
			CString str = (LPCTSTR)message1; // message1을 CString형으로 바꾸어 str에저장
			rx_cs.Unlock(); // 임계구역 나오기

			CString message;

			if (frame.ack_num != frame.seq_num) // frame의 ack_num와 seq_num가 같지않다는것은 상대가 전송한 프레임이라는것을 의미함
			{ // 이때는 상대방에게서 전달받은 frame이라는것을 출력
				message4 += str.Mid(0,16);
				pDlg->m_edit4.SetWindowTextW(message4);
				pDlg->m_edit2.GetWindowText(message); // m_rx_edit를 읽어 message에 저장 
				message += str;
				con2.Format(_T("Receive Frame(%d): %s ==> [seq_num : %d ack_num : %d checksum : %d ] \r\n"), frame.seq_num, str, frame.seq_num, frame.ack_num, frame.checksum);
				pDlg->m_edit2.GetWindowText(con_2);
				con_2 += con2;
				con_2 += "\n";
			}
			if (frame.ack_num == frame.seq_num) // frame의 ack_num와 seq_num가 같다는것은 상대가 ack메세지를 보내기위해 보낸 frame을 의미함
			{ // ack 메세지를 받았다는것을 출력
				pDlg->m_edit2.GetWindowText(message); // m_rx_edit를 읽어 message에 저장 
				message += str;
				con2.Format(_T("Receive Acknowledge Frame(%d): %s ==> [seq_num : %d ack_num : %d checksum : %d ] \r\n"), frame.seq_num, str, frame.seq_num, frame.ack_num, find_checksum(frame.p_buffer,frame.checksum));
				pDlg->m_edit2.GetWindowText(con_2);
				con_2 += con2;
				con_2 += "\n";
				ack = frame.ack_num;
			}
			frame.ack_num = frame.seq_num; // ack확인 메세지를 보내기위해 frame의 ack_num와 seq_num를 같게해줌
	
			if (find_checksum(frame.p_buffer, frame.checksum) == 0 && send_flag != 1) // 체크섬을 확인함=> 중복메세지인지 확인, send_flag가 1이면 => ack메세지를 보내주어야함을뜻함
			{
				pDlg->m_pDataSocket->SendToEx(&frame, sizeof(Frame), port, addr, 0); // 해당 port,addr ack메세지를 실은 frame전송
				con2.Format(_T("Send Acknowledge Frame(%d): %s ==> [seq_num : %d ack_num : %d checksum : %d ] \r\n"), frame.seq_num, str, frame.seq_num, 
					frame.ack_num, find_checksum(frame.p_buffer, frame.checksum));
				con_2 += con2;
				con_2 += "\n";
			} // acknowledge 전송 메세지를 출력

			if (frame.ack_num == seq) // 만약 frame.ack_num와 seq가 같다면 send_flag를 0으로 만들어줌
				send_flag = 0;

			re += str.Mid(0, 16);

			//con_3.Format(_T(""))
			pDlg->m_edit2.SetWindowText(con_2); // con_2를 진행상황 컨트롤에 출력
			pDlg->m_edit2.LineScroll(pDlg->m_edit2.GetLineCount());

			//pDlg->m_edit3.SetWindowText(re); 
			pDlg->m_edit3.LineScroll(pDlg->m_edit3.GetLineCount());
			flist->RemoveAt(current_pos);

		}
		Sleep(10);
	}
	return 0;
}

UINT TXThread(LPVOID arg)
{
	ThreadArg *pArg = (ThreadArg *)arg; // 전달받은 인수 arg를 ThreadArg*형 pArg에 저장
	CList<Frame>* flist = pArg->fList; // CList<Frame>*형 flist에 pArg->fList를 저장
	CARQClient_thdDlg *pDlg = (CARQClient_thdDlg *)pArg->pDlg; // CARQClient_thdDlg*형 pDlg에 (CARQClient_thdDlg *)pArg->pDlg를 저장

	UINT port = 8000; // 포트 선언
	CString addr = _T("127.0.0.1"); // addr선언

	while (pArg->Thread_run) // Thread_run이 1일동안 반복
	{
		POSITION pos = flist->GetHeadPosition(); // flist의 처음position을 pos에저장
		POSITION current_pos;
		CString message2;

		while (pos != NULL) // pos 가 NULL이 아닐동안 반복
		{
			current_pos = pos; // pos를 current_pos에 넣어줌
			pDlg->m_pDataSocket->GetPeerName(addr, port);

			tx_cs.Lock(); // 임계구역 들어가기
			Frame frame = flist->GetNext(pos); // flist의 pos위치 원소를 frame에저장후 pos증가
			Frame frame1;
			TCHAR* message1 = frame.p_buffer; // frame의 p_buffer를 message1에 저장
			CString str = (LPCTSTR)message1; // message1을 cstring으로형변환하여 str에 저장
			tx_cs.Unlock();	// 임계구역 나오기

			CString message;
			//pDlg->m_edit3.GetWindowText(message); // m_tx_edit를 읽어 message에 저장 
			message3 += str.Mid(0,16); // str에 \n문자를 지우기위해 0부터 16번쨰글자를 추출하여 message3에 저장
			con3 = message3;
			pDlg->m_edit3.SetWindowTextW(message3); // message3를 전송한 메세지에 출력

			con3.Format(_T("[ 나 ] : %s "), str.Mid(0,16));
			//pDlg->m_edit3.GetWindowText(co1);
			co1 += con3;
			//pDlg->m_edit3.SetWindowTextW(co1);


			/*co4.Format(_T("Send Frame(%d): %s ==> [seq_num : %d ack_num : %d checksum : %d ] \r\n"), frame.seq_num, str, frame.seq_num, frame.ack_num, frame.checksum); // 전송한 프레임 형식
			pDlg->m_edit2.GetWindowText(con_2);
			con_2 += co4;
			con_2 += "\n";
			pDlg->m_edit2.SetWindowText(con_2); // 전송한 프레임을 출력*/
			double time;

			while (ack!=frame.seq_num) // 이때 프레임이 전송되었으나 ack메세지를 받지못했다면 재전송하기위한 프로세스
			{
				clock_t start = clock(); // 현재의 clock을 측정
				pDlg->m_pDataSocket->SendToEx(&frame, sizeof(Frame), port, addr, 0); // 전송
				co4.Format(_T("Send Frame(%d): %s ==> [seq_num : %d ack_num : %d checksum : %d ] \r\n"), frame.seq_num, str, frame.seq_num, frame.ack_num, frame.checksum); // 전송한 프레임 형식
				pDlg->m_edit2.GetWindowText(con_2);
				con_2 += co4;
				con_2 += "\n";
				pDlg->m_edit2.SetWindowText(con_2);
				while (1) // 무한반복
				{
					if (ack == frame.seq_num) // ack와 frame.seq_num가 같아질떄 while문 나가기
					{
						time = (double)(clock() - start) / CLOCKS_PER_SEC; // 이때 전송에서 ack메세지를 받는데까지 걸린시간 측정하여 time에 저장
						break; 
					}
					if ((clock() - start) / CLOCKS_PER_SEC > 5.0) // 프레임전송후 ack메세지가 5초동안 오지않으면 반복문 나가고 재전송
						break;
				}
			} // stop-and-wait

			//pDlg->m_edit2.GetWindowText(con_2);
			co5.Format(_T("[Send to Receive time : %lf s]"), time);
			con_2 += co5;
			con_2 += "\r\n";
			pDlg->m_edit2.SetWindowText(con_2); // 전송한 프레임을 출력

			pDlg->m_edit3.LineScroll(pDlg->m_edit3.GetLineCount());

			flist->RemoveAt(current_pos); // current_pos의 원소 삭제
		}
		Sleep(10);
	}
	return 0;
}


// CARQClient_thdDlg 메시지 처리기

BOOL CARQClient_thdDlg::OnInitDialog()
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
	CList<Frame>* flist1 = new CList<Frame>;
	arg1.fList = flist1;
	arg1.pList = newlist;
	arg1.Thread_run = 1;
	arg1.pDlg = this;

	CStringList* newlist2 = new CStringList;
	CList<Frame>* flist2 = new CList<Frame>;
	arg2.fList = flist2;
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

void CARQClient_thdDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CARQClient_thdDlg::OnPaint()
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
HCURSOR CARQClient_thdDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CARQClient_thdDlg::ProcessReceive(CDataSocket* pSocket, int nErrorCode)
{
	TCHAR pBuf[1024 + 1];
	CString strData;
	Frame f;
	Frame* pf = &f;
	int nbytes = 0;
	memset(pBuf, 0, 1025 * 2);

	CString addr; // addr선언
	UINT port = 8001; // port선언
	m_ipaddr.GetWindowTextW(addr);

	nbytes = pSocket->ReceiveFromEx(&f, sizeof(Frame), addr, port, 0); // 해당 포트와 주소를 가진 소켓에서 데이터받아오기

	rx_cs.Lock();
	arg2.fList->AddTail(*pf);
	rx_cs.Unlock();
}


void CARQClient_thdDlg::OnBnClickedSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pDataSocket == NULL)	//m_pDataSocket이 NULL 일때
	{
		MessageBox(_T("서버에 접속 안 함!"), _T("알림"), MB_ICONERROR);
	}
	else
	{
	
		POSITION current_pos,pos1;
		pos1 = arg1.pList->GetHeadPosition();
		send_flag = 1; // send버튼을 눌렀을때 send_flag를 1로 지정해 이 노드에서 전송함을 나타냄
		while (pos1 != NULL)
		{
			current_pos = pos1;
			arg1.pList->GetNext(pos1);
			arg1.pList->RemoveAt(current_pos);
		}
		CString message;
		Frame f;
		TCHAR* t;
		int chk = 0;
		int checksum = 0;
		int ack_num=-1;
		int seq_num = 0; // frame에 전달할 기본적인 멤버인수들 초기화
		m_edit1.GetWindowTextW(message); // 전달할 메세지를 받아와 message에 저장
		//message += _T("\r\n");

		if (message.GetLength() <= 16) // message의 길이가 16이하일때 
		{
			tx_cs.Lock();
			ack = -1; // ack를 기본적으로 -1로만듬
			message += "\r\n"; //
			arg1.pList->AddTail(message);
			t = (TCHAR*)(LPCTSTR)message; // message를 tchar*형으로 형변환
			_tcscpy_s(f.p_buffer, t); // t를 f의 p_buffer에저장
			checksum = find_checksum(f.p_buffer, chk); // p_buffer의 체크섬을 구해 checksum에 저장
			f.checksum = checksum;
			f.ack_num = ack_num;
			f.seq_num = seq_num; // f에 인자들을 전달
			arg1.fList->AddTail(f); // f를 arg1의 flist의 맨뒷부분에 삽입
			seq = f.seq_num; // f의 seq_num를 seq에저장 // 마지막으로 전송한 seq_num가 무엇인지 알아놓기위함
			tx_cs.Unlock();
		}
		else if (message.GetLength() > 16) // message의 길이가 16보다 클때
		{
			ack = -1;
			CString *str = new CString[(message.GetLength() / 16) + 1]; // message를 16바이트씩 잘라 저장할 문자열 배열 str 생성

			for (int i = 0; i < (message.GetLength() / 16) + 1; i++)
			{
				str[i] = message.Mid(i * 16, 16); // message를 16바이트씩 잘라 str[i]에 차례로 저장
				str[i] += "\r\n";
			}
			tx_cs.Lock();
			for (int i = 0; i < (message.GetLength() / 16) + 1; i++)
			{
				arg1.pList->AddTail(str[i]); // str을 plist에 차례로저장
			}
			POSITION pos = arg1.pList->GetHeadPosition();
			for (int i = 0; i < (message.GetLength() / 16) + 1 ; i++)
			{
				current_pos = pos;
				Frame f1;
				_tcscpy_s(f1.p_buffer, arg1.pList->GetNext(pos)); // plist의 앞에서 하나씩 꺼낸 문자열을 f1의 p_buffer에 저장
				checksum = find_checksum(f1.p_buffer,chk); // f1의 p_buffer의 체크섬을 구하여 checksum에저장
				f1.checksum = checksum;
				f1.ack_num = ack_num;
				f1.seq_num = seq_num; // f1의 인자들 설정
				arg1.fList->AddTail(f1); // f1을 arg1의 flist 뒷부분에 삽입
				seq_num++; // seq_num는 하나씩 증가
			}
			seq = seq_num-1; // 마지막으로 전송한 seq는 seq_num-1
			tx_cs.Unlock();
		}

		tx_cs.Lock();
		/*t = (TCHAR*)(LPCTSTR)message;
		_tcscpy_s(f.p_buffer, t);
		f.ack_num = ack_num;
		f.seq_num = seq_num;
		arg1.fList->AddTail(f);*/
		arg1.pList->AddTail(message);
		tx_cs.Unlock();

		m_edit1.SetWindowTextW(_T(""));
		m_edit1.SetFocus();

		int len = m_edit3.GetWindowTextLengthW();
		m_edit3.SetSel(len, len);
		m_edit3.ReplaceSel(message);
	}
}


void CARQClient_thdDlg::OnBnClickedClose()
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

		int len = m_edit2.GetWindowTextLengthW();
		CString message = _T("\n### 접속 종료 ###\r\n\r\n");

		m_edit2.SetSel(len, len);
		m_edit2.ReplaceSel(message);
	}
}

int find_checksum(TCHAR* t,int chk)
{
	CString s;
	CString tem;
	int carry;
	int res=0,res1,res2;
	char ch1,ch2,ch3;
	int x;
	int len;
	s = (LPCTSTR)t; // t를 cstring형으로 형변환하여 s에저장
	len=s.GetLength();
	char* temp = new char[len]; // char* 형 temp생성
	memcpy(temp, (unsigned char*)(LPCTSTR)s,len); // temp에 s를 옮겨줌

	// 16bit checksum이므로 한문자 =  1byte = 8bit
	// 따라서 두문자씩 잘라 더하면 16bit checksum을 구현할수있다

	for (int i = 0; i < len; i += 4) // tchar의 한문자가 2byte였기때문에 char배열에서 2씩 더해줘야 다음문자를 찾을수있다.
	{
		ch1 = temp[i]; // temp[i]는 첫번째문자 => ch1에 저장
		ch2 = temp[i + 2]; // temp[i+2]는 두번째문자 => ch2에 저장
		x = (ch1 << 8) + ch2; // ch1을 8bit만큼왼쪽으로 옮기고 ch2를 더하면 두글자만 단독으로떼어낸형태가된다 이를 x에 저장 
		res += x; // res = res+x

		if (res >= 0x00010000) // res가 16비트를 넘게되면 
		{
			carry = (res & 0xFFFF0000) >> 16; // res와 0xffff0000을 and연산후 16비트 만큼 오른쪽으로 옮기면 carry를 알수있다.
			res -= res & 0xFFFF0000; // res의 하위16비트만 남기기위해 res와 0xffff0000을 and연산한값을 원래의 res에 빼준다.
			res += carry; // carry를 res에 더해줌
		}
	}

	//tem.Format(_T("%d"), res);
	res1 = res & 0x0000FFFF;
	res1 += chk; // 마지막으로 chk를 res1에 더해준다
	//AfxMessageBox(tem);
	res2 = ~res1; // res1의 모든 비트를 부호반전시켜 res2에 저장한다.
	res2 = res2 & 0x0000FFFF; // 마지막으로 16비트만 남기고 반환
	return res2;
}