#pragma once
#include "afxwin.h"
#include "resource.h"
#include "SettingManager.h"
#include "CommendManager.h"

// CConnectDatabaseDlg �Ի���

class CConnectDatabaseDlg : public CDialog
{
	DECLARE_DYNAMIC(CConnectDatabaseDlg)

public:
	CConnectDatabaseDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CConnectDatabaseDlg();

// �Ի�������
	enum { IDD = IDD_CONNECT_SERVICE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
