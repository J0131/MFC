#pragma once

// CDataSocket 명령 대상입니다.
class CARQClient_thdDlg;

class CDataSocket : public CSocket
{
public:
	CDataSocket(CARQClient_thdDlg* pDlg);
	virtual ~CDataSocket();
	CARQClient_thdDlg* m_pDlg;
	void OnReceive(int nErrorCode);
};


