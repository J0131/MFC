// DataSocket.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ARQServer_thd.h"
#include "DataSocket.h"
#include "ARQServer_thdDlg.h"


// DataSocket

CDataSocket::CDataSocket(CARQServer_thdDlg *pDlg)
{
	m_pDlg = pDlg;
}

CDataSocket::~CDataSocket()
{
}


// DataSocket 멤버 함수


void CDataSocket::OnReceive(int nErrorCode)
{
	CSocket::OnReceive(nErrorCode);
	m_pDlg->ProcessReceive(this, nErrorCode);
}


void CDataSocket::OnClose(int nErrorCode)
{
	CSocket::OnClose(nErrorCode);
	m_pDlg->ProcessClose(this, nErrorCode);
}
