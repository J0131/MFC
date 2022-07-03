#pragma once

// DataSocket 명령 대상입니다.
class CARQServer_thdDlg;

class CDataSocket : public CSocket
{
public:
	CDataSocket(CARQServer_thdDlg* pDlg);
	virtual ~CDataSocket();
	CARQServer_thdDlg* m_pDlg;
	void OnReceive(int nErrorCode);
	void OnClose(int nErrorCode);
};


