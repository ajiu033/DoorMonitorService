#include "StdAfx.h"
#include "MachineRecordInfo.h"
#include "ConversionManager.h"

std::vector<std::string> CMachineRecordInfo::vcField;
std::vector<std::string> CMachineRecordInfo::vcFieldIdentify;
std::vector<std::string> CMachineRecordInfo::vcRecord;

void CMachineRecordInfo::InitalInfo()
{
	vcField.clear();
	vcField.push_back("PersonId");
	vcField.push_back("CardId");
	vcField.push_back("MachineNo");
	vcField.push_back("MachineId");
	vcField.push_back("DoorNo");
	vcField.push_back("DoorId");
	vcField.push_back("CreateTime");

	vcFieldIdentify.clear();
	vcFieldIdentify.push_back("VisitorsIdCard");
	vcFieldIdentify.push_back("VisitorsName");
	vcFieldIdentify.push_back("VisitorsTel");
	vcFieldIdentify.push_back("RespondentsName");
	vcFieldIdentify.push_back("RespondentsTel");
	vcFieldIdentify.push_back("CreateTime");

	vcRecord.clear();
	vcRecord.push_back("TrafficCode");
	vcRecord.push_back("MachineSer");
	vcRecord.push_back("PacketNo");
	vcRecord.push_back("CardNo16");
	vcRecord.push_back("DoorNo");
	vcRecord.push_back("TrafficTime");
	vcRecord.push_back("RecordType");

	//jurisdiction
	vcRecord.push_back("CardType");
	vcRecord.push_back("ReadHead");
}

CMachineRecordInfo::CMachineRecordInfo(void)
{
	mType_=-1;
}


CMachineRecordInfo::~CMachineRecordInfo(void)
{
}

void CMachineRecordInfo::ParseCommend(std::vector<unsigned char>& vcCmd,int mType)
{
	Reset();

	mType_ = mType;
	std::string strHex,strTime;
	ConversionManager::Byte2Hex(vcCmd,0,4,&strHex);
	SetRecordInfoForKey("MachineSer",strHex);

	ConversionManager::Byte2Hex(vcCmd,8,10,&strHex);
	SetRecordInfoForKey("PacketNo",strHex);

	ConversionManager::Byte2HexReverse(vcCmd,10,18,&strHex);
	SetRecordInfoForKey("CardNo16",strHex);

	ConversionManager::Byte2Hex(vcCmd,18,19,&strHex);
	SetRecordInfoForKey("DoorNo",strHex);

	ConversionManager::Byte2Hex(vcCmd,19,25,&strHex);
	ParseTime(strHex,strTime);
	SetRecordInfoForKey("TrafficTime",strTime);

	ConversionManager::Byte2Dec(vcCmd,25,26,&strHex);
	SetRecordInfoForKey("RecordType",strHex);
}

void CMachineRecordInfo::SetRecordInfoForKey(std::string key,std::string value)
{
	mapRecord_[key]=value;
}

void CMachineRecordInfo::ParseTime(std::string& strHex,std::string& strTime)
{
	strTime="20";
	strTime=strTime+
			strHex.substr(0,2) + "-" + 
			strHex.substr(2,2) + "-" + 
			strHex.substr(4,2) + " " + 
			strHex.substr(6,2) + ":" + 
			strHex.substr(8,2) + ":" + 
			strHex.substr(10,2);
}

void CMachineRecordInfo::SetFieldInfo(std::string mser,std::vector<std::string> vcinfo)
{
	assert(vcField.size()==vcinfo.size());
	std::vector<std::string>::iterator iter = vcField.begin();
	int i=0;
	while(iter!=vcField.end()){
		mapField_[*iter]=vcinfo[i];
		++i;
		++iter;
	}
}

std::string CMachineRecordInfo::GetFieldInfoForKey(std::string key)
{
	std::string ret;
	int n = mapField_.count(key);
	if(n)
		ret = mapField_[key];
	return ret;
}

std::string CMachineRecordInfo::GetRecordInfoForKey(std::string key)
{
	std::string ret;
	int n = mapRecord_.count(key);
	if(n)
		ret = mapRecord_[key];
	return ret;
}

void CMachineRecordInfo::Reset()
{
	mType_=-1;
	mapField_.clear();
	mapRecord_.clear();
}