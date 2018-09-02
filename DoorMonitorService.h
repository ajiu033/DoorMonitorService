
// DoorMonitorService.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDoorMonitorServiceApp:
// See DoorMonitorService.cpp for the implementation of this class
//

class CDoorMonitorServiceApp : public CWinApp
{
public:
	CDoorMonitorServiceApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CDoorMonitorServiceApp theApp;