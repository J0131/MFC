
// ARQClient_thd.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CARQClient_thdApp:
// �� Ŭ������ ������ ���ؼ��� ARQClient_thd.cpp�� �����Ͻʽÿ�.
//

class CARQClient_thdApp : public CWinApp
{
public:
	CARQClient_thdApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CARQClient_thdApp theApp;