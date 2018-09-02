#pragma once
class CMachine;

class CDoorServer
{
	typedef std::map<std::string,boost::shared_ptr<CMachine>> MapMachine;
public:
	CDoorServer(void);
	~CDoorServer(void);
	void StartListen(int port);
	void StopListen();
	void SendCommend(std::string mser);
	void ReleaseSocket(std::string mno);
private:
	SOCKET  listenSocket_;  
	int port_;
	bool isRun_;
	bool isStopping;

public:
	static void Initialize(MapMachine *list,HWND hwnd)
	{
		isMonitorStop=false;
		mapMachineList=list;
		mainHwnd_=hwnd;
		curCount_=0;
	}

private:
	static MapMachine *mapMachineList;
	static bool isMonitorStop;
	static HANDLE hMutex;  //定义互斥对象句柄
	static std::map<SOCKET,std::string> mapSocket ;
	static HWND mainHwnd_;
	static int curCount_;//客户端数量

	static UINT CALLBACK ListenThread(LPVOID  lparam); 
	static UINT CALLBACK RecvThread(LPVOID  lparam);  
	static UINT CALLBACK SendThread(LPVOID  lparam);
	
};


