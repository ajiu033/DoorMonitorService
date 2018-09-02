// ConnectDatabaseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DoorMonitorService.h"
#include "ConnectDatabaseDlg.h"
#include "afxdialogex.h"

CommendManager* CConnectDatabaseDlg::lpcmdMnger_=nullptr;
SettingManager* CConnectDatabaseDlg::lpsetMnger_=nullptr;
std::vector<std::string> CConnectDatabaseDlg::vcSetting_;
bool CConnectDatabaseDlg::isConnecting_=false;
// CConnectDatabaseDlg 对话框

IMPLEMENT_DYNAMIC(CConnectDatabaseDlg, CDialog)

CConnectDatabaseDlg::CConnectDatabaseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectDatabaseDlg::IDD, pParent)
{

}

CConnectDatabaseDlg::~CConnectDatabaseDlg()
{
}

void CConnectDatabaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_IP, c_edit_ip);
	DDX_Control(pDX, IDC_EDIT_DB, c_edit_db);
	DDX_Control(pDX, IDC_EDIT_USER, c_edit_user);
	DDX_Control(pDX, IDC_EDIT_PWD, c_edit_pwd);
}


BEGIN_MESSAGE_MAP(CConnectDatabaseDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CConnectDatabaseDlg::OnBnClickedBtnClear)
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CConnectDatabaseDlg::OnBnClickedBtnConnect)
END_MESSAGE_MAP()


// CConnectDatabaseDlg 消息处理程序


void CConnectDatabaseDlg::OnBnClickedBtnClear()
{
	// TODO: 在此添加控件通知处理程序代码
	c_edit_ip.SetWindowTextA("");
	c_edit_db.SetWindowTextA("");
	c_edit_user.SetWindowTextA("");
	c_edit_pwd.SetWindowTextA("");
	vcSetting_.clear();
}


void CConnectDatabaseDlg::OnBnClickedBtnConnect()
{
	// TODO: 在此添加控件通知处理程序代码
	if(isConnecting_){
		//MessageBox("connecting database...");
		return;
	}

	CString strValue0,strValue1,strValue2,strValue3;
	c_edit_ip.GetWindowTextA(strValue0);
	c_edit_db.GetWindowTextA(strValue1);
	c_edit_user.GetWindowTextA(strValue2);
	c_edit_pwd.GetWindowTextA(strValue3);

	vcSetting_[0]=strValue0;
	vcSetting_[1]=strValue1;
	vcSetting_[2]=strValue2;
	vcSetting_[3]=strValue3;

	isConnecting_=true;
	AfxBeginThread((AFX_THREADPROC)CConnectDatabaseDlg::ConnectDBThread,0);
	
}


BOOL CConnectDatabaseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	lpcmdMnger_->SetFunctorCallback(boost::bind(&CConnectDatabaseDlg::ConnetDBResult,this,_1,_2,_3));
	lpsetMnger_->GetDatabaseSetting(vcSetting_);

	c_edit_ip.SetWindowTextA(CString(vcSetting_[0].c_str()));
	c_edit_db.SetWindowTextA(CString(vcSetting_[1].c_str()));
	c_edit_user.SetWindowTextA(CString(vcSetting_[2].c_str()));
	c_edit_pwd.SetWindowTextA(CString(vcSetting_[3].c_str()));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CConnectDatabaseDlg::ConnetDBResult(std::string title,std::string msg,bool ret)
{
	isConnecting_=false;
	if(ret){
		//MessageBox("connect database successful");
		lpsetMnger_->SetDatabaseSetting(vcSetting_);
		EndDialog(IDOK);
	}else{
		MessageBox("连接数据库失败！");
	}

	
}

UINT CALLBACK CConnectDatabaseDlg::ConnectDBThread(LPVOID  lparam)
{
	lpcmdMnger_->connect(vcSetting_[0],
						 vcSetting_[1],
						 vcSetting_[2],
						 vcSetting_[3]);
	return 0;
}

void CConnectDatabaseDlg::setManager(CommendManager* lp_c,SettingManager* lp_s)
{
	lpcmdMnger_ = lp_c;
	lpsetMnger_ = lp_s;
}