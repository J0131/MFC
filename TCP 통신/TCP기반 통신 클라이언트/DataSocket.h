#pragma once
#include "afxsock.h"

class CTCPClient_thdDlg;

class CDataSocket :public CSocket
{
public:
	CDataSocket(CTCPClient_thdDlg* pDlg);
	~CDataSocket();
	CTCPClient_thdDlg *m_pDlg;
	void OnReceive(int nErrorCode);
};

