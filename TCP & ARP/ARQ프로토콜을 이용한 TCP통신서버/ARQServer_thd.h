
// ARQServer_thd.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CARQServer_thdApp:
// �� Ŭ������ ������ ���ؼ��� ARQServer_thd.cpp�� �����Ͻʽÿ�.
//

class CARQServer_thdApp : public CWinApp
{
public:
	CARQServer_thdApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CARQServer_thdApp theApp;