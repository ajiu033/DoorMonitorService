#include "StdAfx.h"
#include "DoorServer.h"
#include "ConversionManager.h"
#include "Machine.h"

CDoorServer::MapMachine* CDoorServer::mapMachineList=nullptr;
bool CDoorServer::isMonitorStop=false;
HANDLE CDoorServer::hMutex;  //定义互斥对象句柄
std::map<SOCKET,std::string> CDoorServer::mapSocket ;
HWND CDoorServer::mainHwnd_=0;
int CDoorServer::curCount_=0;//客户端数量

UINT CALLBACK CDoorServer::RecvThread(LPVOID  lparam)  
{  
	SOCKET  ClientSocket=(SOCKET)lparam;  
	boost::shared_ptr<CMachine> machine=nullptr;

	std::string mser;//机器序列号
	int  bytesRecv=0;  
	char  sendbuf[100]={0};  
	char  recvbuf[1024]={0};  
	std::string strCmd;
	std::vector<unsigned char> vcBuffer;
	std::set<int> setCmdLen;//命令长度集，用于判断接收长度是否在命令长度集里面.
	setCmdLen.insert(38);
	setCmdLen.insert(28);
	setCmdLen.insert(18);
	setCmdLen.insert(10);
	setCmdLen.insert(12);
	setCmdLen.insert(13);
	setCmdLen.insert(30);
	while (1)
	{
		
		memset(recvbuf,0,sizeof(recvbuf));
		memset(sendbuf,0,sizeof(sendbuf));
		bytesRecv=recv(ClientSocket,recvbuf,sizeof(recvbuf),0); 
		if (bytesRecv <= 0)
		{
			if ( bytesRecv == 0 ) 
				TRACE(_T("--网络连接断开--\n"));
			else 
				TRACE(_T("--错误：%d\n"),WSAGetLastError());
			::SendMessageA(mainHwnd_,UM_END,0,(LPARAM)(LPCTSTR)CString(mser.c_str()));
			return 0;
		}else
		{
			const int count = setCmdLen.count(bytesRecv);
			if(!count){
				   //此方法用于实时接收下位机命令和实时反馈与当前命令的确认命令判断。
				   //如果不等于这些限定长度，抛弃当前缓冲区的命令数据，继续接收下一个命令，直到命令完整为止。
				   continue;
			}

			vcBuffer.clear();//不在累计命令，只碰到当前接收的命令.
			vcBuffer.insert(vcBuffer.end(),recvbuf,recvbuf+bytesRecv);
			const unsigned int vcSize=vcBuffer.size();
			if(vcSize<8)continue;
			unsigned int cmdLen=(vcBuffer[5]<<8|vcBuffer[4]);//获取命令长度
			cmdLen+=8;					//实际命令长度
			if(vcSize<cmdLen)continue;//小于命令长度，继续读数据
			std::vector<unsigned char> vcCommend(vcBuffer.begin(),vcBuffer.end());;//获取有效命令数据

			//获取对应的CMachine对象指针
			if(mser.empty()){
				ConversionManager::Byte2Hex(vcCommend,0,4,&mser);
				int count = mapMachineList->count(mser);
				if(count){
					machine = (*mapMachineList)[mser];
					machine->SetMachineSer(mser);
					machine->UpdateSocket(ClientSocket);
					machine->SetIpAndPort(mapSocket[ClientSocket]);
				}
			}

			if(machine==nullptr)break;//退出线程
			machine->SetConnectState(true);

			//处理数据
			machine->ParseCommend(vcCommend);
		}
	}

	return  0;  
}  

UINT CALLBACK CDoorServer::SendThread(LPVOID  lparam)  
{
	std::string mser = *((std::string*)lparam);
	boost::shared_ptr<CMachine> machine;
	machine = (*mapMachineList)[mser];
	std::string strCmd,strHex;
	machine->SendCommend(strCmd,strHex);
	return 0;
}

UINT CALLBACK CDoorServer::ListenThread(LPVOID  lparam)  
{
	SOCKET  mSocket=(SOCKET)lparam;
	using namespace std;
	//vector<SOCKET> vcSocket ;
	mapSocket.clear();
	curCount_=0;
	while (true)
	{
		TRACE(_T("\n正在等待客户端的接入\n"));
		sockaddr_in remote_addr;
		int addr_len = sizeof(sockaddr);
		SOCKET sock = accept(mSocket, (sockaddr *)&remote_addr, &addr_len);
		if (INVALID_SOCKET == sock)
		{
			TRACE(_T("网络连接被断开，线程将会返回。。\n"));
			map<SOCKET,string>::iterator i;
			for (i = mapSocket.begin(); i != mapSocket.end(); ++i)
			{
				shutdown(i->first, SD_BOTH);
				closesocket(i->first);
				curCount_--;
			}
			curCount_=0;
			//关闭所有客户端的连接
			return 0;
		} 
		else
		{
			char str_ip[128]={0};
			inet_ntop(AF_INET, &remote_addr.sin_addr, str_ip, sizeof(str_ip));
			std::string  port = std::to_string((long long)ntohs(remote_addr.sin_port));
			std::string  ip(str_ip);
			std::string  ip_port = ip + ":" + port;
			TRACE(_T("客户连接成功%s\n"),ip_port.c_str());
			WaitForSingleObject(hMutex, INFINITE);//互斥
			mapSocket[sock]=ip_port;
			curCount_++;
			ReleaseMutex(hMutex);
			//在这里可进账号验证，如果验证成功就会启动新的线程进行消息分发。
			//保存sock，以备后续进行关闭。
			AfxBeginThread((AFX_THREADPROC)CDoorServer::RecvThread,(LPVOID)sock);
		}
	}
	return 0;

}

CDoorServer::CDoorServer(void)
	:port_(52000),
	isRun_(false),
	isStopping(false)
{
	hMutex = CreateMutex(NULL, false, "mutex");     //创建互斥对象
}


CDoorServer::~CDoorServer(void)
{
	StopListen();
}


void CDoorServer::StartListen(int port)
{
	if(isRun_)return;
	isRun_=true;
	isMonitorStop=false;

	port_ = port;
	
	//启动套接字
	WSADATA wsaData;
	//ZeroMemory(&wsaData,sizeof(WSADATA));
	if (WSAStartup(MAKEWORD(1,1),&wsaData))
	{
		TRACE(_T("初始化sock失败\n"));
		return;
	}
 
	listenSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket_ == INVALID_SOCKET)
	{
		TRACE(_T("创建监听sock失败\n"));
		return;
	}
	//端口绑定
	SOCKADDR_IN local;
	local.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	local.sin_family = AF_INET;
	local.sin_port = htons(port_);
	if (bind(listenSocket_, (sockaddr *)&local, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		closesocket(listenSocket_);
		listenSocket_ = INVALID_SOCKET;
		return;
	}
	listen(listenSocket_,5);

	::SendMessageA(mainHwnd_,UM_LISTEN,0,0);
	CWinThread* thread = AfxBeginThread((AFX_THREADPROC)CDoorServer::ListenThread,(LPVOID)listenSocket_);//启动线程来监听，避免阻塞
}

void CDoorServer::StopListen()
{
	if(isStopping)return;//防止重复点击
	isStopping=true;
	if (listenSocket_ != INVALID_SOCKET)
	{
		isMonitorStop=true;
		shutdown(listenSocket_, SD_BOTH);
		closesocket(listenSocket_);
		while (curCount_ > 0)//等待所有线程结束
		{
			Sleep(100);
		}
	}

	listenSocket_ = INVALID_SOCKET;
	WSACleanup();	 //释放套接字资源;
	isRun_=false;
	isStopping=false;
	isMonitorStop=false;

	::SendMessageA(mainHwnd_,UM_STOP,0,0);
}

void CDoorServer::ReleaseSocket(std::string mno)
{
	//移除映射
	int count = mapMachineList->count(mno);
	if(count){
		boost::shared_ptr<CMachine> machine;
		machine = (*mapMachineList)[mno];
		SOCKET ClientSocket = machine->GetSocket();
		if(ClientSocket==INVALID_SOCKET)
			return;

		const int n = mapSocket.count(ClientSocket);
		if(n){
			closesocket(ClientSocket);
			//WaitForSingleObject(hMutex, INFINITE);//互斥
			curCount_--;
			mapSocket.erase(ClientSocket);
			//ReleaseMutex(hMutex);
		}
		machine->Disconnect();
	}
}

void CDoorServer::SendCommend(std::string mser)
{
	//DWORD  dwThreadId;  
	//HANDLE  hThread;  
	//hThread=CreateThread(NULL,NULL,SendThread,(LPVOID)lpdoorCmd,0,&dwThreadId);  
	AfxBeginThread((AFX_THREADPROC)CDoorServer::SendThread,(LPVOID)&mser);
}