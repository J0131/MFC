#pragma once

// CDataSocket ��� ����Դϴ�.
class CUDPServer_thdDlg;

class CDataSocket : public CSocket
{
public:
	CDataSocket(CUDPServer_thdDlg* pDlg);
	virtual ~CDataSocket();
	CUDPServer_thdDlg* m_pDlg;
	void OnReceive(int nErrorCode);
	void OnClose(int nErrorCode);
};


