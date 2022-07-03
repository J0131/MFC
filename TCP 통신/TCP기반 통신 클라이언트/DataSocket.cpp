#include "stdafx.h"
#include "DataSocket.h"
#include "TCPClient_thdDlg.h"
#include "TCPClient_thd.h"

CDataSocket::CDataSocket(CTCPClient_thdDlg* pDlg)
{
	m_pDlg = pDlg;
}


CDataSocket::~CDataSocket()
{
}


void CDataSocket::OnReceive(int nErrorCode)
{
	CSocket::OnReceive(nErrorCode);
	m_pDlg->ProcessReceive(this, nErrorCode);
}
