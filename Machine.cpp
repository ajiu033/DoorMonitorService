#include "StdAfx.h"
#include "Machine.h"
#include "ConversionManager.h"

void CMachine::Init()
{
	CMachineInfo::InitalInfo();
	CMachineRecordInfo::InitalInfo();
}

CMachine::CMachine(void)
{
	/*socket_ = INVALID_SOCKET;
	isConn_ = false;
	rev_cmdLen_=0;
	rev_cmdTag_=0;
	isCheck_=false;
	IsTaskOk_=false;*/

	Reset();
}


CMachine::~CMachine(void)
{
}

void CMachine::Reset()
{
	socket_ = INVALID_SOCKET;
	isConn_ = false;
	rev_cmdLen_=0;
	rev_cmdTag_=0;
	rev_strCmd_="";
	send_cmd_="";
	rev_message_="";
	send_message_="";
	machineInfo_.Reset();
	machineRecordInfo_.Reset();
	isCheck_=false;
	isReapet_=false;
	send_state_=false;
	IsTaskOk_=false;
	setNewCmd.clear();
}

void CMachine::Disconnect()
{
	Reset();
	std::string msg;
	msg = "断开连接";
	msgCallback_(machineSer_,msg);
}

void CMachine::ParseCommend(std::vector<unsigned char>& vcCmd)
{
	ConversionManager::Byte2Hex(vcCmd,0,vcCmd.size(),&rev_strCmd_);
	rev_cmdLen_		= vcCmd.size();
	rev_cmdTag_		= vcCmd[7];

	GetCommendType();
	isConn_ = true;
	//防重复命令
	isReapet_ = static_cast<bool>(setNewCmd.count(rev_strCmd_));
	setNewCmd.insert(rev_strCmd_);
	if(cmdCount<setNewCmd.size())
		setNewCmd.erase(setNewCmd.begin());
	//end 防重复

	switch(cmdType_){
	case REV_HEART:
		machineInfo_.ParseCommend(vcCmd);
		rev_message_="接收心跳指令：" + rev_strCmd_;
		heartCallback_(machineSer_);
		machineInfo_.UpdateHeartCount();
		SendCmdHeart();
		break;
	case REV_RECORD:
		machineRecordInfo_.ParseCommend(vcCmd,0);
		machineRecordInfo_.SetRecordInfoForKey("TrafficCode",rev_strCmd_);
		rev_message_="接收记录指令：" + rev_strCmd_;
		recordCallback_(machineSer_);
		//SendCmdRecord();
		break;
	case REV_PERMISSION:
		machineRecordInfo_.ParseCommend(vcCmd,1);
		rev_message_="接收权限指令：" + rev_strCmd_;
		permissionCallback_(machineSer_);
		//SendCmdPermission();
		break;
	case REV_SETTIME:
		rev_message_="机器校时返回指令：" + rev_strCmd_;
		commonCallback_(machineSer_);
		send_state_=false;
		break;
	case REV_NAMELIST:
		rev_message_="下传名单返回指令：" + rev_strCmd_;
		commonCallback_(machineSer_);
		send_state_=false;
		break;
	case REV_NAMELIST_CLEAR:
		rev_message_="清空名单返回指令：" + rev_strCmd_;
		commonCallback_(machineSer_);
		send_state_=false;
		break;
	case REV_PARAMATER:
		rev_message_="设置参数返回指令：" + rev_strCmd_;
		commonCallback_(machineSer_);
		send_state_=false;
		break;
	case REV_OPENDOOR:
		rev_message_="远程开门返回指令：" + rev_strCmd_;
		commonCallback_(machineSer_);
		send_state_=false;
		break;
	default:
		break;
	}
}

void CMachine::GetCommendType()
{
	if(38==rev_cmdLen_ && 0x12==rev_cmdTag_){
		cmdType_ = REV_HEART;
	}else if(28==rev_cmdLen_ && 0xaa==rev_cmdTag_)
	{
		cmdType_ = REV_RECORD;
	}else if(28==rev_cmdLen_ && 0x55==rev_cmdTag_)
	{
		cmdType_ = REV_PERMISSION;
	}else if(18==rev_cmdLen_ && 0x20==rev_cmdTag_)
	{
		cmdType_ = REV_SETTIME;
	}else if(12==rev_cmdLen_ && 0x30==rev_cmdTag_)
	{
		cmdType_ = REV_NAMELIST;
	}else if(12==rev_cmdLen_ && 0x32==rev_cmdTag_)
	{
		cmdType_ = REV_NAMELIST_CLEAR;
	}else if(30==rev_cmdLen_ && 0x70==rev_cmdTag_)
	{
		cmdType_ = REV_PARAMATER;
	}else if(10==rev_cmdLen_ && 0x60==rev_cmdTag_)
	{
		cmdType_ = REV_OPENDOOR;
	}else
	{
		cmdType_ = REV_UNKNOWN;
	}
}

//**********************************************************************
//receive machine data and send the response interface begin
//**********************************************************************
void CMachine::SetCmdHexToBytes(std::string strcmd,char* sendbuf,int sendlen)
{
	ConversionManager::HexStrToByte(strcmd.c_str(),(unsigned char*)sendbuf,sendlen-2);

	int sum = GetVerifySum(sendbuf,sendlen-2);
	char buf[2]={0};
	memcpy(buf,  &sum, 2);

	sendbuf[sendlen-2] = buf[0];
	sendbuf[sendlen-1] = buf[1];
}

void CMachine::SetCmdBytesToHex(unsigned char* sendbuf,int sendlen,std::string& rel)
{
	ConversionManager::Byte2Hex(sendbuf,sendlen,&rel);
}

int CMachine::GetVerifySum(char* bArray,int len)
{
	int ret = 0;
	int n = 0;
	for(int i=0;i<len;i++)
	{
		n = (int)bArray[i];
		ret=ret+(n&0xff);
	}

	return ret;
}

void CMachine::SendCmdAll(const std::string strcmd,std::string& ret)
{
	if(socket_==INVALID_SOCKET)
	{
		send_state_=false;
		return;
	}

	assert(socket_!=INVALID_SOCKET);
	int cmdlen = strcmd.length()/2+2;
	char *sendbuf = new char[cmdlen*2];
	//memset(sendbuf,0,cmdlen*2);

	SetCmdHexToBytes(strcmd,sendbuf,cmdlen);
	send(socket_, sendbuf, cmdlen, 0);
	//commend transfer to hex string
	SetCmdBytesToHex((unsigned char*)sendbuf,cmdlen,ret);

	send_message_ = "发送命令：" + ret;
	sendCallback_(machineSer_);
	delete[] sendbuf;
}

void CMachine::SendCommend(std::string strcmd,std::string& strhex)
{
	send_cmd_=strcmd;
	if(send_cmd_.empty())
	{
		send_state_=false;
		return;
	}

	SendCmdAll(send_cmd_,strhex);
}

void CMachine::SendCmdHeart()
{
	std::string strCmd = rev_strCmd_.substr(0,8) + "02002012";
	std::string ret;
	SendCmdAll(strCmd,ret);
}

void CMachine::SendCmdPermission()
{
	std::string strCmd = rev_strCmd_.substr(0,8) + "06002055" + rev_strCmd_.substr(16,4) + "0000";
	std::string ret;
	SendCmdAll(strCmd,ret);
}

void CMachine::SendCmdRecord()
{
	std::string strCmd = rev_strCmd_.substr(0,8) + "060020AA" + rev_strCmd_.substr(16,4) + "0000";
	std::string ret;
	SendCmdAll(strCmd,ret);
}

void CMachine::SendCmdOpenDoor(std::string doorno)
{
	std::string strCmd = rev_strCmd_.substr(0,8) + "040020600" + doorno + "05";
	std::string ret;
	SendCmdAll(strCmd,ret);
}

//**********************************************************************
//CMachineInfo interface begin
//**********************************************************************

void CMachine::SetMachineInfo(std::string mser,std::vector<std::string> vcinfo)
{
	machineInfo_.SetMachineInfo(mser,vcinfo);
}

std::string CMachine::GetMachineInfoForKey(std::string key,int mType)
{
	if(mType==0){
		return machineInfo_.GetMachineInfoForKey(key);
	}else{
		return machineInfo_.GetHeartInfoForKey(key);
	}
}

unsigned long long CMachine::GetHeartCount()
{
	return machineInfo_.GetHeartCount();
}

void CMachine::SetIpAndPort(std::string str)
{
	std::vector<std::string> destination;
	ConversionManager::Split(str,":",destination);
	if(destination.size()!=2)
		return;

	machineInfo_.SetMachineInfoForKey("IpAddress",destination[0]);
	machineInfo_.SetMachineInfoForKey("Port",destination[1]);
}

//**********************************************************************
//CMachineRecordInfo interface begin
//**********************************************************************
void CMachine::SetFieldInfo(std::string mser,std::vector<std::string> vcinfo)
{
	machineRecordInfo_.SetFieldInfo(mser,vcinfo);
}

std::string CMachine::GetRecordInfoForKey(std::string key,int mType)
{
	if(mType==0){
		return machineRecordInfo_.GetFieldInfoForKey(key);
	}else{
		return machineRecordInfo_.GetRecordInfoForKey(key);
	}
}
