// DataSocket.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ARQClient_thd.h"
#include "DataSocket.h"
#include "ARQClient_thdDlg.h"

// CDataSocket

CDataSocket::CDataSocket(CARQClient_thdDlg* pDlg)
{
	m_pDlg = pDlg;
}

CDataSocket::~CDataSocket()
{
}


// CDataSocket ��� �Լ�


void CDataSocket::OnReceive(int nErrorCode)
{
	CSocket::OnReceive(nErrorCode);
	m_pDlg->ProcessReceive(this, nErrorCode);
}
