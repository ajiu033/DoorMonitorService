#pragma once
#include "afxwin.h"
#include "resource.h"
#include "SettingManager.h"
#include "CommendManager.h"

// CConnectDatabaseDlg 对话框

class CConnectDatabaseDlg : public CDialog
{
	DECLARE_DYNAMIC(CConnectDatabaseDlg)

public:
	CConnectDatabaseDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CConnectDatabaseDlg();

// 对话框数据
	enum { IDD = IDD_CONNECT_SERVICE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit c_edit_ip;
	CEdit c_edit_db;
	CEdit c_edit_user;
	CEdit c_edit_pwd;
	afx_msg void OnBnClickedBtnClear();
	afx_msg void OnBnClickedBtnConnect();

	void setManager(CommendManager* lp_c,SettingManager* lp_s);
private:
	void ConnetDBResult(std::string title,std::string msg,bool ret);

	static CommendManager *lpcmdMnger_;
	static SettingManager *lpsetMnger_;
	static std::vector<std::string> vcSetting_;
	static bool isConnecting_;
	static UINT CALLBACK ConnectDBThread(LPVOID  lparam);
public:
	virtual BOOL OnInitDialog();
};
