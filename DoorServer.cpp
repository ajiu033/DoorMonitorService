#include "StdAfx.h"
#include "DoorServer.h"
#include "ConversionManager.h"
#include "Machine.h"

CDoorServer::MapMachine* CDoorServer::mapMachineList=nullptr;
bool CDoorServer::isMonitorStop=false;
HANDLE CDoorServer::hMutex;  //���廥�������
std::map<SOCKET,std::string> CDoorServer::mapSocket ;
HWND CDoorServer::mainHwnd_=0;
int CDoorServer::curCount_=0;//�ͻ�������

UINT CALLBACK CDoorServer::RecvThread(LPVOID  lparam)  
{  
	SOCKET  ClientSocket=(SOCKET)lparam;  
	boost::shared_ptr<CMachine> machine=nullptr;

	std::string mser;//�������к�
	int  bytesRecv=0;  
	char  sendbuf[100]={0};  
	char  recvbuf[1024]={0};  
	std::string strCmd;
	std::vector<unsigned char> vcBuffer;
	std::set<int> setCmdLen;//����ȼ��������жϽ��ճ����Ƿ�������ȼ�����.
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
				TRACE(_T("--�������ӶϿ�--\n"));
			else 
				TRACE(_T("--����%d\n"),WSAGetLastError());
			::SendMessageA(mainHwnd_,UM_END,0,(LPARAM)(LPCTSTR)CString(mser.c_str()));
			return 0;
		}else
		{
			const int count = setCmdLen.count(bytesRecv);
			if(!count){
				   //�˷�������ʵʱ������λ�������ʵʱ�����뵱ǰ�����ȷ�������жϡ�
				   //�����������Щ�޶����ȣ�������ǰ���������������ݣ�����������һ�����ֱ����������Ϊֹ��
				   continue;
			}

			vcBuffer.clear();//�����ۼ����ֻ������ǰ���յ�����.
			vcBuffer.insert(vcBuffer.end(),recvbuf,recvbuf+bytesRecv);
			const unsigned int vcSize=vcBuffer.size();
			if(vcSize<8)continue;
			unsigned int cmdLen=(vcBuffer[5]<<8|vcBuffer[4]);//��ȡ�����
			cmdLen+=8;					//ʵ�������
			if(vcSize<cmdLen)continue;//С������ȣ�����������
			std::vector<unsigned char> vcCommend(vcBuffer.begin(),vcBuffer.end());;//��ȡ��Ч��������

			//��ȡ��Ӧ��CMachine����ָ��
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

			if(machine==nullptr)break;//�˳��߳�
			machine->SetConnectState(true);

			//��������
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
		TRACE(_T("\n���ڵȴ��ͻ��˵Ľ���\n"));
		sockaddr_in remote_addr;
		int addr_len = sizeof(sockaddr);
		SOCKET sock = accept(mSocket, (sockaddr *)&remote_addr, &addr_len);
		if (INVALID_SOCKET == sock)
		{
			TRACE(_T("�������ӱ��Ͽ����߳̽��᷵�ء���\n"));
			map<SOCKET,string>::iterator i;
			for (i = mapSocket.begin(); i != mapSocket.end(); ++i)
			{
				shutdown(i->first, SD_BOTH);
				closesocket(i->first);
				curCount_--;
			}
			curCount_=0;
			//�ر����пͻ��˵�����
			return 0;
		} 
		else
		{
			char str_ip[128]={0};
			inet_ntop(AF_INET, &remote_addr.sin_addr, str_ip, sizeof(str_ip));
			std::string  port = std::to_string((long long)ntohs(remote_addr.sin_port));
			std::string  ip(str_ip);
			std::string  ip_port = ip + ":" + port;
			TRACE(_T("�ͻ����ӳɹ�%s\n"),ip_port.c_str());
			WaitForSingleObject(hMutex, INFINITE);//����
			mapSocket[sock]=ip_port;
			curCount_++;
			ReleaseMutex(hMutex);
			//������ɽ��˺���֤�������֤�ɹ��ͻ������µ��߳̽�����Ϣ�ַ���
			//����sock���Ա��������йرա�
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
	hMutex = CreateMutex(NULL, false, "mutex");     //�����������
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
	
	//�����׽���
	WSADATA wsaData;
	//ZeroMemory(&wsaData,sizeof(WSADATA));
	if (WSAStartup(MAKEWORD(1,1),&wsaData))
	{
		TRACE(_T("��ʼ��sockʧ��\n"));
		return;
	}
 
	listenSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket_ == INVALID_SOCKET)
	{
		TRACE(_T("��������sockʧ��\n"));
		return;
	}
	//�˿ڰ�
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
	CWinThread* thread = AfxBeginThread((AFX_THREADPROC)CDoorServer::ListenThread,(LPVOID)listenSocket_);//�����߳�����������������
}

void CDoorServer::StopListen()
{
	if(isStopping)return;//��ֹ�ظ����
	isStopping=true;
	if (listenSocket_ != INVALID_SOCKET)
	{
		isMonitorStop=true;
		shutdown(listenSocket_, SD_BOTH);
		closesocket(listenSocket_);
		while (curCount_ > 0)//�ȴ������߳̽���
		{
			Sleep(100);
		}
	}

	listenSocket_ = INVALID_SOCKET;
	WSACleanup();	 //�ͷ��׽�����Դ;
	isRun_=false;
	isStopping=false;
	isMonitorStop=false;

	::SendMessageA(mainHwnd_,UM_STOP,0,0);
}

void CDoorServer::ReleaseSocket(std::string mno)
{
	//�Ƴ�ӳ��
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
			//WaitForSingleObject(hMutex, INFINITE);//����
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