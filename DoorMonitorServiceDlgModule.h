#pragma once

class CDoorMonitorServiceDlg;
class CMachine;
class CDoorServer;


class CDoorMonitorServiceDlgModule
{
	typedef std::map<std::string,boost::shared_ptr<CMachine>> MapMachine;
	typedef std::vector<std::map<std::string,std::string>>    VecRecord;
public:
	static CDoorMonitorServiceDlgModule* getInstance();
	static void clearInstance();
	static CDoorMonitorServiceDlgModule* module_;

	CDoorMonitorServiceDlgModule(void);
	~CDoorMonitorServiceDlgModule(void);

	void setDelegate(CDoorMonitorServiceDlg* delegate_);//{Delegate_=delegate_;}
	void getMachineList();
	void OnTimer(UINT_PTR nIDEvent);
	void ResetTime();
	void Run(int port);
	void Stop();
	void Disconnect(std::string mser);
	void SendCommend(std::string mser,int mType);
	bool MachineIsConn(std::string mser);
	void SetMachineCheck(std::string mser,bool check);
private:
	CDoorMonitorServiceDlg* Delegate_;
	boost::shared_ptr<CDoorServer> doorSvr;
	
	MapMachine mapMachineList;

	int dTimes,hTimes,mTimes,nTimes;// ±∑÷√Î
	std::map<std::string,int> mapMachineTime;
	bool isRun;
	
private:
	CString get_longtime();
	void SetValue(int& a,int& b,const int v);
	int isDoConnet(std::string mno,int time);
	void UpdateMachineList();
	void SetListRowColors(int row,int col,bool state);
	int GetListIndex(std::string mser);
	void ShowAndWriteLog(std::string mser,std::string strmsg,int mType=0);
	bool GetMachine(std::string mser,boost::shared_ptr<CMachine>& info,int& index); 
	void SendMessageUpdate(int index,std::string mser,std::string msg);
private:
	static std::map<int,std::string> mapMserList;
	static UINT CALLBACK SendSelTime(LPVOID  lparam);
	static UINT CALLBACK SendSelParamater(LPVOID  lparam);
	static UINT CALLBACK SendSelNameList(LPVOID  lparam);
	//send commend function
	void WaitForSendState(boost::shared_ptr<CMachine>& info);
	void SendAllTime();
	void SendAllParamater();
	void SendAllNameList();

	//message function
	void MessageCallback(std::string mser,std::string msg);
	void DealSendInfo(std::string mser);
	void DealHeartInfo(std::string mser);
	void DealRecordInfo(std::string mser);
	void DealPermissionInfo(std::string mser);
	void DealCommonInfo(std::string mser);
};

