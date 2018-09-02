#pragma once
#include "MachineInfo.h"
#include "MachineRecordInfo.h"

class CMachine
{
	typedef boost::function<void(std::string,std::string)> FuncMsgCallback;
	typedef boost::function<void(std::string)> FuncSendCallback;
	typedef boost::function<void(std::string)> FuncHeartCallback;
	typedef boost::function<void(std::string)> FuncRecordCallback;
	typedef boost::function<void(std::string)> FuncPermissionCallback;
	typedef boost::function<void(std::string)> FuncCommonCallback;

	enum COMMEND_TYPE{
		REV_HEART,
		REV_PERMISSION,
		REV_RECORD,
		REV_SETTIME,
		REV_NAMELIST,
		REV_NAMELIST_CLEAR,
		REV_PARAMATER,
		REV_OPENDOOR,
		REV_UNKNOWN
	}cmdType_;
public:
	static void Init();
	//bind callback function
	void SetFuncMsgCallback(const FuncMsgCallback& cb){msgCallback_=cb;}
	void SetFuncSendCallback(const FuncSendCallback& cb){sendCallback_=cb;}
	void SetFuncHeartCallback(const FuncHeartCallback& cb){heartCallback_=cb;}
	void SetFuncRecordCallback(const FuncRecordCallback& cb){recordCallback_=cb;}
	void SetFuncPermissionCallback(const FuncPermissionCallback& cb){permissionCallback_=cb;}
	void SetFuncCommonCallback(const FuncCommonCallback& cb){commonCallback_=cb;}

	//normal function
	CMachine(void);
	~CMachine(void);
	void ParseCommend(std::vector<unsigned char>& vcCmd);
	void UpdateSocket(SOCKET& s){socket_=s;}
	SOCKET& GetSocket(){return socket_;}
	std::string GetCommendRev(){return rev_strCmd_;}
	void SetConnectState(bool state){isConn_=state;}
	bool GetConnectState(){return isConn_;}
	void SetMachineSer(std::string mser){machineSer_=mser;}
	std::string GetMachineSer(){return machineSer_;}
	void SetCheck(bool check){isCheck_=check;}
	bool GetCheck(){return isCheck_;}
	std::string GetRevMessage(){return rev_message_;}
	std::string GetSendMessage(){return send_message_;}
	void SetSendState(bool state){send_state_=state;}
	bool GetSendState(){return send_state_;}
	void SetTaskState(bool state){IsTaskOk_=state;}
	bool GetTaskState(){return IsTaskOk_;}
	void Reset();
	void Disconnect();

	//SendCommend interface
	void SendCommend(std::string strcmd,std::string& strhex);

	//CMachineInfo interface
	void SetMachineInfo(std::string mser,std::vector<std::string> vcinfo);
	std::string GetMachineInfoForKey(std::string key,int mType);
	unsigned long long GetHeartCount();
	void SetIpAndPort(std::string str);

	//CMachineRecordInfo interface
	void SetFieldInfo(std::string mser,std::vector<std::string> vcinfo);
	std::string GetRecordInfoForKey(std::string key,int mType);

private:
	//callback members
	FuncMsgCallback			msgCallback_;
	FuncSendCallback		sendCallback_;
	FuncHeartCallback		heartCallback_;
	FuncRecordCallback		recordCallback_;
	FuncPermissionCallback  permissionCallback_;
	FuncCommonCallback		commonCallback_;

	//machine components
	CMachineInfo			machineInfo_;
	CMachineRecordInfo		machineRecordInfo_;

	//base info
	SOCKET					socket_;
	bool					isConn_;
	std::string				machineSer_;
	bool                    isCheck_;
	bool					IsTaskOk_;
	//recerve info
	std::string				rev_message_;
	std::string				rev_strCmd_;
	unsigned int			rev_cmdLen_;
	unsigned int			rev_cmdTag_;

	//***begin***use on ************************
	bool					isReapet_;//防止重复
	std::set<std::string>	setNewCmd;//最新接收的命令列表，用于判断命令是否重复了.
	const static int		cmdCount= 5;//对应setNewCmd数目
	//***end*************************************

	//***begin send a new commend string*******
	std::string				send_message_;
	std::string				send_cmd_;
	bool					send_state_;//发送命令后是否有返回命令，false=已返回，true没有返回
	//*****************************************

private://for private function
	int GetVerifySum(char* bArray,int len);
	void GetCommendType();

	//SendCommend interface
	void SetCmdHexToBytes(std::string strcmd,char* sendbuf,int sendlen);
	void SetCmdBytesToHex(unsigned char* sendbuf,int sendlen,std::string& rel);

	//specific send cmd interface
	void SendCmdAll(const std::string strcmd,std::string& ret);
public:
	void SendCmdHeart();
	void SendCmdPermission();
	void SendCmdRecord();
	void SendCmdOpenDoor(std::string doorno);

};

