// DataSocket.cpp : 구현 파일입니다.
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


// CDataSocket 멤버 함수


void CDataSocket::OnReceive(int nErrorCode)
{
	CSocket::OnReceive(nErrorCode);
	m_pDlg->ProcessReceive(this, nErrorCode);
}
