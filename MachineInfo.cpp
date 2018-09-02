#include "StdAfx.h"
#include "MachineInfo.h"
#include "ConversionManager.h"

std::vector<std::string> CMachineInfo::vcField;
std::vector<std::string> CMachineInfo::vcHeart;

void CMachineInfo::InitalInfo()
{
	vcField.clear();
	vcField.push_back("Id");
	vcField.push_back("MachineNo");
	vcField.push_back("MachineSer");
	vcField.push_back("MachineName");
	vcField.push_back("MachinePlace");
	vcField.push_back("IpAddress");
	vcField.push_back("Port");
	vcField.push_back("NameListCount");
	vcField.push_back("RecordCount");
	vcField.push_back("UnlockTime");

	vcHeart.clear();
	vcHeart.push_back("IPV6");
	vcHeart.push_back("IPV4");
	vcHeart.push_back("HeartTime");
	vcHeart.push_back("LockState");
}

CMachineInfo::CMachineInfo(void)
{
	heartCount_=0;
}

CMachineInfo::CMachineInfo(const CMachineInfo& info)
	:mapInfo_(info.mapInfo_),
	mapHeartInfo_(info.mapHeartInfo_),
	heartCount_(info.heartCount_)
{

}

CMachineInfo::~CMachineInfo(void)
{
}


void CMachineInfo::ParseCommend(std::vector<unsigned char>& vcCmd)
{
	std::string strHex,strTime;
	ConversionManager::Byte2Hex(vcCmd,0,4,&strHex);
	SetMachineInfoForKey("MachineSer",strHex);

	ConversionManager::Byte2Hex(vcCmd,8,14,&strHex);
	SetHeartInfoForKey("IPV6",strHex);

	ConversionManager::Byte2Hex(vcCmd,14,18,&strHex);
	SetHeartInfoForKey("IPV4",strHex);

	ConversionManager::Byte2Dec(vcCmd,18,20,&strHex);
	SetMachineInfoForKey("NameListCount",strHex);

	ConversionManager::Byte2Dec(vcCmd,20,24,&strHex);
	SetMachineInfoForKey("RecordCount",strHex);

	ConversionManager::Byte2Hex(vcCmd,24,31,&strHex);
	ParseTime(strHex,strTime);
	SetHeartInfoForKey("HeartTime",strTime);

	ConversionManager::Byte2Dec(vcCmd,31,32,&strHex);
	SetMachineInfoForKey("UnlockTime",strHex);

	ConversionManager::Byte2Dec(vcCmd,32,33,&strHex);
	SetHeartInfoForKey("LockState",strHex);
}

void CMachineInfo::ParseTime(std::string& strHex,std::string& strTime)
{
	strTime="20";
	strTime=strTime+
			strHex.substr(0,2) + "-" + 
			strHex.substr(2,2) + "-" + 
			strHex.substr(4,2) + " " + 
			strHex.substr(8,2) + ":" + 
			strHex.substr(10,2)+ ":" + 
			strHex.substr(12,2);
}

void CMachineInfo::SetMachineInfo(std::string mser,std::vector<std::string> vcinfo)
{
	assert(vcField.size()==vcinfo.size());
	std::vector<std::string>::iterator iter = vcField.begin();
	int i=0;
	while(iter!=vcField.end()){
		mapInfo_[*iter]=vcinfo[i];
		++i;
		++iter;
	}
}

void CMachineInfo::SetMachineInfoForKey(std::string key,std::string value)
{
	mapInfo_[key]=value;
}

std::string CMachineInfo::GetMachineInfoForKey(std::string key)
{
	std::string ret;
	int n = mapInfo_.count(key);
	if(n)
		ret = mapInfo_[key];
	return ret;
}

void CMachineInfo::SetHeartInfoForKey(std::string key,std::string value)
{
	mapHeartInfo_[key]=value;
}

std::string CMachineInfo::GetHeartInfoForKey(std::string key)
{
	return mapHeartInfo_[key];
}

void CMachineInfo::UpdateHeartCount()
{
	++heartCount_;
}

unsigned long long CMachineInfo::GetHeartCount()
{
	return heartCount_;
}

void CMachineInfo::Reset()
{
	heartCount_=0;
	mapInfo_.clear();
	mapHeartInfo_.clear();
}