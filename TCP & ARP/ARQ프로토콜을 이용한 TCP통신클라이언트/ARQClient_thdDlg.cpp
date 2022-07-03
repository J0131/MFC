
// ARQClient_thdDlg.cpp : ���� ����
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
// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.
int find_checksum(TCHAR* t,int chk);
CString re;
char send_flag=0;
int seq;
CString co1,co2,con2,con_2,con3,con_3,co4,co5;

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


// CARQClient_thdDlg ��ȭ ����



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
	ThreadArg *pArg = (ThreadArg *)arg; // ���޹��� �μ� arg�� ThreadArg*�� pArg�� ����
	CList<Frame>* flist = pArg->fList; // CList<Frame>*�� flist�� pArg->pList�� ����
	CARQClient_thdDlg *pDlg = (CARQClient_thdDlg *)pArg->pDlg; // CARQClient_thdDlg*�� pDlg�� (CARQClient_thdDlg *)pArg->pDlg�� ����
	UINT port = 8000; // ��Ʈ ����
	CString addr = _T("127.0.0.1"); // addr����
	while (pArg->Thread_run) { // Thread_run�� 1�ϵ��� �ݺ�
		POSITION pos = flist->GetHeadPosition(); // flist�� ó��position�� pos������
		POSITION current_pos;
		while (pos != NULL) // pos �� NULL�� �ƴҵ��� �ݺ�
		{
			current_pos = pos; // pos�� current_pos�� �־���
			rx_cs.Lock();  // �Ӱ豸�� ����
			Frame frame = flist->GetNext(pos); // flist�� pos��ġ ���Ҹ� frame�������� pos����
			TCHAR* message1 = frame.p_buffer; // frame�� p_buffer�� message1�� ����
			CString str = (LPCTSTR)message1; // message1�� CString������ �ٲپ� str������
			rx_cs.Unlock(); // �Ӱ豸�� ������

			CString message;

			if (frame.ack_num != frame.seq_num) // frame�� ack_num�� seq_num�� �����ʴٴ°��� ��밡 ������ �������̶�°��� �ǹ���
			{ // �̶��� ���濡�Լ� ���޹��� frame�̶�°��� ���
				message4 += str.Mid(0,16);
				pDlg->m_edit4.SetWindowTextW(message4);
				pDlg->m_edit2.GetWindowText(message); // m_rx_edit�� �о� message�� ���� 
				message += str;
				con2.Format(_T("Receive Frame(%d): %s ==> [seq_num : %d ack_num : %d checksum : %d ] \r\n"), frame.seq_num, str, frame.seq_num, frame.ack_num, frame.checksum);
				pDlg->m_edit2.GetWindowText(con_2);
				con_2 += con2;
				con_2 += "\n";
			}
			if (frame.ack_num == frame.seq_num) // frame�� ack_num�� seq_num�� ���ٴ°��� ��밡 ack�޼����� ���������� ���� frame�� �ǹ���
			{ // ack �޼����� �޾Ҵٴ°��� ���
				pDlg->m_edit2.GetWindowText(message); // m_rx_edit�� �о� message�� ���� 
				message += str;
				con2.Format(_T("Receive Acknowledge Frame(%d): %s ==> [seq_num : %d ack_num : %d checksum : %d ] \r\n"), frame.seq_num, str, frame.seq_num, frame.ack_num, find_checksum(frame.p_buffer,frame.checksum));
				pDlg->m_edit2.GetWindowText(con_2);
				con_2 += con2;
				con_2 += "\n";
				ack = frame.ack_num;
			}
			frame.ack_num = frame.seq_num; // ackȮ�� �޼����� ���������� frame�� ack_num�� seq_num�� ��������
	
			if (find_checksum(frame.p_buffer, frame.checksum) == 0 && send_flag != 1) // üũ���� Ȯ����=> �ߺ��޼������� Ȯ��, send_flag�� 1�̸� => ack�޼����� �����־����������
			{
				pDlg->m_pDataSocket->SendToEx(&frame, sizeof(Frame), port, addr, 0); // �ش� port,addr ack�޼����� ���� frame����
				con2.Format(_T("Send Acknowledge Frame(%d): %s ==> [seq_num : %d ack_num : %d checksum : %d ] \r\n"), frame.seq_num, str, frame.seq_num, 
					frame.ack_num, find_checksum(frame.p_buffer, frame.checksum));
				con_2 += con2;
				con_2 += "\n";
			} // acknowledge ���� �޼����� ���

			if (frame.ack_num == seq) // ���� frame.ack_num�� seq�� ���ٸ� send_flag�� 0���� �������
				send_flag = 0;

			re += str.Mid(0, 16);

			//con_3.Format(_T(""))
			pDlg->m_edit2.SetWindowText(con_2); // con_2�� �����Ȳ ��Ʈ�ѿ� ���
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
	ThreadArg *pArg = (ThreadArg *)arg; // ���޹��� �μ� arg�� ThreadArg*�� pArg�� ����
	CList<Frame>* flist = pArg->fList; // CList<Frame>*�� flist�� pArg->fList�� ����
	CARQClient_thdDlg *pDlg = (CARQClient_thdDlg *)pArg->pDlg; // CARQClient_thdDlg*�� pDlg�� (CARQClient_thdDlg *)pArg->pDlg�� ����

	UINT port = 8000; // ��Ʈ ����
	CString addr = _T("127.0.0.1"); // addr����

	while (pArg->Thread_run) // Thread_run�� 1�ϵ��� �ݺ�
	{
		POSITION pos = flist->GetHeadPosition(); // flist�� ó��position�� pos������
		POSITION current_pos;
		CString message2;

		while (pos != NULL) // pos �� NULL�� �ƴҵ��� �ݺ�
		{
			current_pos = pos; // pos�� current_pos�� �־���
			pDlg->m_pDataSocket->GetPeerName(addr, port);

			tx_cs.Lock(); // �Ӱ豸�� ����
			Frame frame = flist->GetNext(pos); // flist�� pos��ġ ���Ҹ� frame�������� pos����
			Frame frame1;
			TCHAR* message1 = frame.p_buffer; // frame�� p_buffer�� message1�� ����
			CString str = (LPCTSTR)message1; // message1�� cstring��������ȯ�Ͽ� str�� ����
			tx_cs.Unlock();	// �Ӱ豸�� ������

			CString message;
			//pDlg->m_edit3.GetWindowText(message); // m_tx_edit�� �о� message�� ���� 
			message3 += str.Mid(0,16); // str�� \n���ڸ� ��������� 0���� 16�������ڸ� �����Ͽ� message3�� ����
			con3 = message3;
			pDlg->m_edit3.SetWindowTextW(message3); // message3�� ������ �޼����� ���

			con3.Format(_T("[ �� ] : %s "), str.Mid(0,16));
			//pDlg->m_edit3.GetWindowText(co1);
			co1 += con3;
			//pDlg->m_edit3.SetWindowTextW(co1);


			/*co4.Format(_T("Send Frame(%d): %s ==> [seq_num : %d ack_num : %d checksum : %d ] \r\n"), frame.seq_num, str, frame.seq_num, frame.ack_num, frame.checksum); // ������ ������ ����
			pDlg->m_edit2.GetWindowText(con_2);
			con_2 += co4;
			con_2 += "\n";
			pDlg->m_edit2.SetWindowText(con_2); // ������ �������� ���*/
			double time;

			while (ack!=frame.seq_num) // �̶� �������� ���۵Ǿ����� ack�޼����� �������ߴٸ� �������ϱ����� ���μ���
			{
				clock_t start = clock(); // ������ clock�� ����
				pDlg->m_pDataSocket->SendToEx(&frame, sizeof(Frame), port, addr, 0); // ����
				co4.Format(_T("Send Frame(%d): %s ==> [seq_num : %d ack_num : %d checksum : %d ] \r\n"), frame.seq_num, str, frame.seq_num, frame.ack_num, frame.checksum); // ������ ������ ����
				pDlg->m_edit2.GetWindowText(con_2);
				con_2 += co4;
				con_2 += "\n";
				pDlg->m_edit2.SetWindowText(con_2);
				while (1) // ���ѹݺ�
				{
					if (ack == frame.seq_num) // ack�� frame.seq_num�� �������� while�� ������
					{
						time = (double)(clock() - start) / CLOCKS_PER_SEC; // �̶� ���ۿ��� ack�޼����� �޴µ����� �ɸ��ð� �����Ͽ� time�� ����
						break; 
					}
					if ((clock() - start) / CLOCKS_PER_SEC > 5.0) // ������������ ack�޼����� 5�ʵ��� ���������� �ݺ��� ������ ������
						break;
				}
			} // stop-and-wait

			//pDlg->m_edit2.GetWindowText(con_2);
			co5.Format(_T("[Send to Receive time : %lf s]"), time);
			con_2 += co5;
			con_2 += "\r\n";
			pDlg->m_edit2.SetWindowText(con_2); // ������ �������� ���

			pDlg->m_edit3.LineScroll(pDlg->m_edit3.GetLineCount());

			flist->RemoveAt(current_pos); // current_pos�� ���� ����
		}
		Sleep(10);
	}
	return 0;
}


// CARQClient_thdDlg �޽��� ó����

BOOL CARQClient_thdDlg::OnInitDialog()
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
	m_pDataSocket = new CDataSocket(this);
	m_pDataSocket->Create(8001, SOCK_DGRAM); // datasocket udp�������� ����
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
	arg2.pDlg = this; // ������ �μ� �ʱ�ȭ

	WSADATA wsa;
	int error_code;
	if ((error_code = WSAStartup(MAKEWORD(2, 2), &wsa)) != 0) {

		TCHAR buffer[256];
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 256, NULL);
		AfxMessageBox(buffer, MB_ICONERROR);
	}

	pThread1 = AfxBeginThread(TXThread, (LPVOID)&arg1); // tx������ ����
	pThread2 = AfxBeginThread(RXThread, (LPVOID)&arg2); // rx������ ���� 

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.

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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CARQClient_thdDlg::OnPaint()
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

	CString addr; // addr����
	UINT port = 8001; // port����
	m_ipaddr.GetWindowTextW(addr);

	nbytes = pSocket->ReceiveFromEx(&f, sizeof(Frame), addr, port, 0); // �ش� ��Ʈ�� �ּҸ� ���� ���Ͽ��� �����͹޾ƿ���

	rx_cs.Lock();
	arg2.fList->AddTail(*pf);
	rx_cs.Unlock();
}


void CARQClient_thdDlg::OnBnClickedSend()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_pDataSocket == NULL)	//m_pDataSocket�� NULL �϶�
	{
		MessageBox(_T("������ ���� �� ��!"), _T("�˸�"), MB_ICONERROR);
	}
	else
	{
	
		POSITION current_pos,pos1;
		pos1 = arg1.pList->GetHeadPosition();
		send_flag = 1; // send��ư�� �������� send_flag�� 1�� ������ �� ��忡�� �������� ��Ÿ��
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
		int seq_num = 0; // frame�� ������ �⺻���� ����μ��� �ʱ�ȭ
		m_edit1.GetWindowTextW(message); // ������ �޼����� �޾ƿ� message�� ����
		//message += _T("\r\n");

		if (message.GetLength() <= 16) // message�� ���̰� 16�����϶� 
		{
			tx_cs.Lock();
			ack = -1; // ack�� �⺻������ -1�θ���
			message += "\r\n"; //
			arg1.pList->AddTail(message);
			t = (TCHAR*)(LPCTSTR)message; // message�� tchar*������ ����ȯ
			_tcscpy_s(f.p_buffer, t); // t�� f�� p_buffer������
			checksum = find_checksum(f.p_buffer, chk); // p_buffer�� üũ���� ���� checksum�� ����
			f.checksum = checksum;
			f.ack_num = ack_num;
			f.seq_num = seq_num; // f�� ���ڵ��� ����
			arg1.fList->AddTail(f); // f�� arg1�� flist�� �ǵ޺κп� ����
			seq = f.seq_num; // f�� seq_num�� seq������ // ���������� ������ seq_num�� �������� �˾Ƴ�������
			tx_cs.Unlock();
		}
		else if (message.GetLength() > 16) // message�� ���̰� 16���� Ŭ��
		{
			ack = -1;
			CString *str = new CString[(message.GetLength() / 16) + 1]; // message�� 16����Ʈ�� �߶� ������ ���ڿ� �迭 str ����

			for (int i = 0; i < (message.GetLength() / 16) + 1; i++)
			{
				str[i] = message.Mid(i * 16, 16); // message�� 16����Ʈ�� �߶� str[i]�� ���ʷ� ����
				str[i] += "\r\n";
			}
			tx_cs.Lock();
			for (int i = 0; i < (message.GetLength() / 16) + 1; i++)
			{
				arg1.pList->AddTail(str[i]); // str�� plist�� ���ʷ�����
			}
			POSITION pos = arg1.pList->GetHeadPosition();
			for (int i = 0; i < (message.GetLength() / 16) + 1 ; i++)
			{
				current_pos = pos;
				Frame f1;
				_tcscpy_s(f1.p_buffer, arg1.pList->GetNext(pos)); // plist�� �տ��� �ϳ��� ���� ���ڿ��� f1�� p_buffer�� ����
				checksum = find_checksum(f1.p_buffer,chk); // f1�� p_buffer�� üũ���� ���Ͽ� checksum������
				f1.checksum = checksum;
				f1.ack_num = ack_num;
				f1.seq_num = seq_num; // f1�� ���ڵ� ����
				arg1.fList->AddTail(f1); // f1�� arg1�� flist �޺κп� ����
				seq_num++; // seq_num�� �ϳ��� ����
			}
			seq = seq_num-1; // ���������� ������ seq�� seq_num-1
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_pDataSocket == NULL)
	{
		MessageBox(_T("������ ���� ����!"), _T("�˸�"), MB_ICONINFORMATION);
	}
	else
	{
		arg1.Thread_run = 0;
		arg2.Thread_run = 0;

		m_pDataSocket->Close();
		delete m_pDataSocket;
		m_pDataSocket = NULL;

		int len = m_edit2.GetWindowTextLengthW();
		CString message = _T("\n### ���� ���� ###\r\n\r\n");

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
	s = (LPCTSTR)t; // t�� cstring������ ����ȯ�Ͽ� s������
	len=s.GetLength();
	char* temp = new char[len]; // char* �� temp����
	memcpy(temp, (unsigned char*)(LPCTSTR)s,len); // temp�� s�� �Ű���

	// 16bit checksum�̹Ƿ� �ѹ��� =  1byte = 8bit
	// ���� �ι��ھ� �߶� ���ϸ� 16bit checksum�� �����Ҽ��ִ�

	for (int i = 0; i < len; i += 4) // tchar�� �ѹ��ڰ� 2byte���⶧���� char�迭���� 2�� ������� �������ڸ� ã�����ִ�.
	{
		ch1 = temp[i]; // temp[i]�� ù��°���� => ch1�� ����
		ch2 = temp[i + 2]; // temp[i+2]�� �ι�°���� => ch2�� ����
		x = (ch1 << 8) + ch2; // ch1�� 8bit��ŭ�������� �ű�� ch2�� ���ϸ� �α��ڸ� �ܵ����ζ�����°��ȴ� �̸� x�� ���� 
		res += x; // res = res+x

		if (res >= 0x00010000) // res�� 16��Ʈ�� �ѰԵǸ� 
		{
			carry = (res & 0xFFFF0000) >> 16; // res�� 0xffff0000�� and������ 16��Ʈ ��ŭ ���������� �ű�� carry�� �˼��ִ�.
			res -= res & 0xFFFF0000; // res�� ����16��Ʈ�� ��������� res�� 0xffff0000�� and�����Ѱ��� ������ res�� ���ش�.
			res += carry; // carry�� res�� ������
		}
	}

	//tem.Format(_T("%d"), res);
	res1 = res & 0x0000FFFF;
	res1 += chk; // ���������� chk�� res1�� �����ش�
	//AfxMessageBox(tem);
	res2 = ~res1; // res1�� ��� ��Ʈ�� ��ȣ�������� res2�� �����Ѵ�.
	res2 = res2 & 0x0000FFFF; // ���������� 16��Ʈ�� ����� ��ȯ
	return res2;
}