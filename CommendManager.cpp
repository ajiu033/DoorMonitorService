#include "StdAfx.h"
#include "CommendManager.h"
#include "DatabaseManager.h"
#include "ConversionManager.h"
#include "MachineInfo.h"
#include "MachineRecordInfo.h"
#include <boost/algorithm/string.hpp>

CommendManager* CommendManager::cmdManager_=nullptr;

CommendManager* CommendManager::getInstance()
{
	if (NULL == cmdManager_)
    {
        cmdManager_ = new CommendManager();
    }
    return cmdManager_;
}

void CommendManager::clearInstance()
{
	delete cmdManager_;
	cmdManager_=nullptr;
}

CommendManager::CommendManager(void)
{
	//std::string sIP("120.24.214.21");
	//std::string sDB("testonecard");
	//std::string sUS("sa");
	//std::string sPW("tIWr0yrsTQAeXXo6h7C4eGFaq164flTO");
	//connect(sIP,sDB,sUS,sPW);
}


CommendManager::~CommendManager(void)
{
	//DatabaseManager::clearInstance();
}

void CommendManager::connect(std::string serviceIP,std::string database,
					std::string username,std::string password)
{
	bool ret = DatabaseManager::getInstance()->connectDB(serviceIP,database,username,password);
	functorCallback_("connect","connect database",ret);
}

bool CommendManager::CreateRecordTable(std::string table)
{
	std::string sql ="CREATE TABLE [" + table + "] ( \
					[Id] [int] IDENTITY (1, 1) NOT NULL , \
					[TrafficCode] [varchar] (80) NULL , \
					[PersonId] [int] NULL , \
					[CardId] [int] NULL , \
					[TrafficTime] [datetime] NULL , \
					[MachineNo] [varchar] (20) NULL , \
					[MachineId] [int] NOT NULL , \
					[DoorNo] [varchar] (20) NULL, \
					[DoorId] [int] NOT NULL , \
					[ReadHead] [int] NULL , \
					[TrafficResult] [int] NULL , \
					[Image] [varchar] (50) NULL, \
					[IsValid] [bit] NOT NULL, \
					[CreateTime] [datetime] NOT NULL \
					) ON [PRIMARY]";
	return DatabaseManager::getInstance()->CreateTable(sql,table);
}

bool CommendManager::getMachineInfo(MAPInfo& mapInfo)
{
	std::vector<std::string> vcField(CMachineInfo::vcField);
	std::string strfield;
	std::vector<std::string>::iterator it = vcField.begin();
	while(it!=vcField.end()){
		strfield+=*it;
		++it;
		if(it!=vcField.end()){
			strfield+=",";
		}
	}

	std::string sql ="SELECT " + strfield + " FROM Door_Machine";

	VCRecordset vcRecordset;
	bool ret = DatabaseManager::getInstance()->queryDB(sql,vcField,vcRecordset);
	if(ret){
		VCRecordset::iterator iter = vcRecordset.begin();
		while(iter!=vcRecordset.end())
		{
			std::vector<std::string> vcinfo = *iter;
			std::string mno = vcinfo[2];
			boost::trim(mno);//erese space
			std::string mser = mno;//ConversionManager::ReveseHex(mno);	//revese machineser,and restore then release the procedure.
			vcinfo[2] = mser;										//revese machineser,and restore then release the procedure.
			mapInfo.insert(std::make_pair(mser,vcinfo));
			++iter;
		}
	}

	return ret;
}

bool CommendManager::getRecordInfo(std::string mno,std::string cardno,std::map<std::string,std::string>& recordInfo,int mType)
{
	std::vector<std::string> vcField(CMachineRecordInfo::vcField);
	std::vector<std::string> vcFieldIdentify(CMachineRecordInfo::vcFieldIdentify);
	std::vector<std::string> vcMyField;
	std::string sql = "";
	if(mType==0){
		vcMyField.swap(vcField);
		sql = "SELECT c.Id AS CardId, p.PersonId, p.MachineNo, p.MachineId, p.DoorNo, p.DoorId, \
				CONVERT(varchar(100), GETDATE(), 20) AS CreateTime, c.CardSer \
				FROM Door_PersonDoorJurisdiction AS p INNER JOIN Mana_Cards AS c  \
				ON p.PersonId = c.PersonId  \
				WHERE (c.CardSer = '" + cardno + "') AND (p.MachineNo = '" + mno + "')";
	}else{
		vcMyField.swap(vcFieldIdentify);
		sql = "SELECT   VisitorsIdCard, VisitorsName, VisitorsTel, RespondentsName, RespondentsTel, \
				CONVERT(varchar(100), GETDATE(), 20) AS CreateTime \
				FROM      Spec_Door_VisitorsList \
				WHERE   (VisitorsIdCard = '" + cardno + "')";
	}

	VCRecordset vcRecordset;
	bool ret = DatabaseManager::getInstance()->queryDB(sql,vcMyField,vcRecordset);
	if(ret){
		VCRecordset::iterator iter = vcRecordset.begin();
		if(iter!=vcRecordset.end())
		{
			std::vector<std::string> vcinfo = *iter;
			std::vector<std::string>::iterator it0 = vcinfo.begin();
			std::vector<std::string>::iterator it1 = vcMyField.begin();
			for(;it1!=vcMyField.end();++it0,++it1)
			{
				recordInfo[*it1] = *it0;
			}
		}
	}

	return ret;
}

bool CommendManager::getPermission(std::string mno,std::string cardno,std::string doorno,int mType)
{
	std::string sql = "";
	if(mType==0){
		sql = "SELECT p.PersonId, p.MachineNo, p.DoorNo, c.CardSer FROM \
			  Door_PersonDoorJurisdiction AS p INNER JOIN Mana_Cards AS c  \
			  ON p.PersonId = c.PersonId WHERE (c.CardSer = '" + cardno + "')  \
			  AND (p.MachineNo = '" + mno + "') AND (p.JurisdictionNo = 1) AND (p.DoorNo='" + doorno + "')";
	}else{
		sql = "SELECT VisitorsIdCard, ValidityStartTime, ValidityEndTime, JurisdictionScope \
			  FROM Spec_Door_VisitorsList \
			  WHERE (VisitorsIdCard = '" + cardno + "')";
	}

	return DatabaseManager::getInstance()->queryDB(sql);
}

bool CommendManager::updateMachineInfo(std::string mser,MAPUpdateSet& mapUpdateSet)
{
	std::string sql ="SELECT * FROM Door_Machine where MachineSer='" + mser + "'";
	return DatabaseManager::getInstance()->updateDB(sql,mapUpdateSet);
}

bool CommendManager::updateRecordInfo(std::string table,std::string trafficCode,MAPUpdateSet& mapUpdateSet)
{
	std::string sql ="SELECT * FROM " + table + " where TrafficCode='" + trafficCode + "'";
	return DatabaseManager::getInstance()->updateDB(sql,mapUpdateSet);
}

bool CommendManager::CommendNameList(std::string mno,std::vector<std::string>& vccmd)
{
	std::string commend(mno);
	std::string sql ="SELECT c.State, c.PersonId, c.CardSer, p.MachineNo, p.DoorNo,p.JurisdictionNo \
					  FROM Mana_Cards AS c LEFT OUTER JOIN Door_PersonDoorJurisdiction AS p  \
					  ON c.PersonId = p.PersonId  \
					  WHERE (c.State = 1)  \
					  AND (p.JurisdictionNo = 1)  \
					  AND (c.CardSer IS NOT NULL)  \
					  AND (p.DoorNo IS NOT NULL)  \
					  AND (p.MachineNo =  \
					  (SELECT MachineNo  \
                      FROM Door_Machine \
                      WHERE (MachineSer = '" + mno + "')))";

	std::vector<std::string> vcField;
	vcField.push_back("CardSer");
	vcField.push_back("DoorNo");
	vcField.push_back("JurisdictionNo");
	VCRecordset vcRecordset;
	bool ret = DatabaseManager::getInstance()->queryDB(sql,vcField,vcRecordset);
	if(ret){
		if(vcRecordset.size()<=0)
			return false;

		std::string namestr;
		VCRecordset::iterator iter = vcRecordset.begin();
		while(iter!=vcRecordset.end())
		{
			std::vector<std::string> vcnames = *iter;
			if(vcnames.size()==vcField.size()){
				std::string cardser = vcnames[0];
				std::string doorno	= vcnames[1];
				std::string jno		= vcnames[2];
				namestr=SingleName(cardser,doorno,jno);
				vccmd.push_back(namestr);
			}
			
			++iter;
		}
	}

	return ret;
}

std::string CommendManager::SingleName(std::string cardser,std::string doorno,std::string jno)
{
	std::string cmd;
	std::string cardno;
	cmd = ConversionManager::ReveseHex(cardser) + ConversionManager::IntToHex(doorno,2) + ConversionManager::IntToHex(jno,2) + "0000";
	return cmd;
}

void CommendManager::CommendNameListClear(std::string mno,std::string& cmd)
{
	cmd = mno + "02002032";
}

void CommendManager::CommendSetTime(std::string mno,std::string time,std::string& cmd)
{
	cmd = mno + "0a002020"+time + "05";
}

bool CommendManager::CommendSetParamater(std::string mno,std::string& cmd)
{
	std::string commend(mno);
	std::string swlock("01");
	std::string locktype("00");
	std::string pw("000000000000");
	std::string other("000000000000000000000000");
	commend+="20002070";
	std::string sql ="SELECT MachineSer, UnlockType, UnlockPwd  \
					  FROM Door_Machine  \
                      WHERE (MachineSer = '" + mno + "')";

	std::vector<std::string> vcField;
	vcField.push_back("MachineSer");
	vcField.push_back("UnlockType");
	vcField.push_back("UnlockPwd");
	VCRecordset vcRecordset;
	bool ret = DatabaseManager::getInstance()->queryDB(sql,vcField,vcRecordset);
	if(ret){
		if(vcRecordset.size()<=0)
			return false;

		VCRecordset::iterator iter = vcRecordset.begin();
		if(iter!=vcRecordset.end())
		{
			std::vector<std::string> vcnames = *iter;
			if(vcnames.size()==vcField.size()){
				std::string mser	= vcnames[0];
				std::string type	= vcnames[1];
				std::string pwd		= vcnames[2];
				locktype	= ConversionManager::GetStringRight(locktype+type,2);
				pw			= ConversionManager::GetStringRight(pw+ConversionManager::StringSingleToHex(pwd),pw.length());
				commend+=(swlock+locktype+pw+other);
				cmd = commend;
			}
		}
			
	}

	return ret;
}
