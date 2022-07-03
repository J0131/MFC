// DataSocket.cpp : ���� �����Դϴ�.
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


// DataSocket ��� �Լ�


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
