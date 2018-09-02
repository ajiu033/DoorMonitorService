
// DoorMonitorService.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DoorMonitorService.h"
#include "DoorMonitorServiceDlg.h"
#include "ConnectDatabaseDlg.h"

#include "DatabaseManager.h"
#include "CommendManager.h"
#include "SettingManager.h"
#include "LogManager.h"
#include "DoorMonitorServiceDlgModule.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDoorMonitorServiceApp

BEGIN_MESSAGE_MAP(CDoorMonitorServiceApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDoorMonitorServiceApp construction

CDoorMonitorServiceApp::CDoorMonitorServiceApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDoorMonitorServiceApp object

CDoorMonitorServiceApp theApp;


// CDoorMonitorServiceApp initialization

BOOL CDoorMonitorServiceApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	{
		//login
		//AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CConnectDatabaseDlg *logDlg_ = new CConnectDatabaseDlg();
		logDlg_->setManager(CommendManager::getInstance(),SettingManager::getInstance());
		INT_PTR nResponse =logDlg_->DoModal();
		delete logDlg_;
		if(nResponse==IDOK){
			//MessageBox(NULL,"login","",0);
		}
		else
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with Cancel
			if (pShellManager != NULL)
			{
				delete pShellManager;
			}
			return FALSE;
		}
		
	}

	CDoorMonitorServiceDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}



int CDoorMonitorServiceApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	DatabaseManager::clearInstance();
	CommendManager::clearInstance();
	SettingManager::clearInstance();
	LogManager::clearInstance();
	CDoorMonitorServiceDlgModule::clearInstance();
	return CWinApp::ExitInstance();
}
