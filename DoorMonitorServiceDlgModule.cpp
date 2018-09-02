#include "StdAfx.h"
#include "DoorMonitorServiceDlgModule.h"
#include "CommendManager.h"
#include "LogManager.h"
#include "DoorMonitorServiceDlg.h"
#include "Machine.h"
#include "DoorServer.h"
#include "ConversionManager.h"
#include "MD5.h"

CDoorMonitorServiceDlgModule* CDoorMonitorServiceDlgModule::module_=nullptr;
std::map<int,std::string> CDoorMonitorServiceDlgModule::mapMserList;

CDoorMonitorServiceDlgModule* CDoorMonitorServiceDlgModule::getInstance()
{
	if (nullptr == module_)
    {
        module_ = new CDoorMonitorServiceDlgModule();
    }
    return module_;
}

void CDoorMonitorServiceDlgModule::clearInstance()
{
	delete module_;
	module_=nullptr;
}


CDoorMonitorServiceDlgModule::CDoorMonitorServiceDlgModule(void)
{
}


CDoorMonitorServiceDlgModule::~CDoorMonitorServiceDlgModule(void)
{
}

void CDoorMonitorServiceDlgModule::setDelegate(CDoorMonitorServiceDlg* delegate_)
{
	Delegate_=delegate_;
	isRun = false;
	CMachine::Init();
	CDoorServer::Initialize(&mapMachineList,Delegate_->m_hWnd);
	boost::shared_ptr<CDoorServer> d(new CDoorServer());
	doorSvr=d;

	getMachineList();
}

void CDoorMonitorServiceDlgModule::getMachineList()
{
	mapMachineList.clear();
	MAPInfo mapInfo;
	bool ret = CommendManager::getInstance()->getMachineInfo(mapInfo);
	if(ret)
	{
		MAPInfo::iterator iter = mapInfo.begin();
		while(iter!=mapInfo.end())
		{
			boost::shared_ptr<CMachine> machine(new CMachine());
			machine->SetMachineInfo(iter->first,iter->second);
			machine->SetMachineSer(iter->first);
			//bind callback function
			machine->SetFuncMsgCallback(boost::bind(&CDoorMonitorServiceDlgModule::MessageCallback,this,_1,_2));
			machine->SetFuncSendCallback(boost::bind(&CDoorMonitorServiceDlgModule::DealSendInfo,this,_1));
			machine->SetFuncHeartCallback(boost::bind(&CDoorMonitorServiceDlgModule::DealHeartInfo,this,_1));
			machine->SetFuncRecordCallback(boost::bind(&CDoorMonitorServiceDlgModule::DealRecordInfo,this,_1));
			machine->SetFuncPermissionCallback(boost::bind(&CDoorMonitorServiceDlgModule::DealPermissionInfo,this,_1));
			machine->SetFuncCommonCallback(boost::bind(&CDoorMonitorServiceDlgModule::DealCommonInfo,this,_1));
			//set value
			mapMachineList[iter->first]=machine;
			++iter;
		}
	}

	UpdateMachineList();
}

void CDoorMonitorServiceDlgModule::UpdateMachineList()
{
	Delegate_->c_list_door.DeleteAllItems();
	std::string ip;
	std::string msg;
	MapMachine::iterator iter = mapMachineList.begin();
	int index=0;
	while(iter!=mapMachineList.end()){
		boost::shared_ptr<CMachine> info=iter->second;
		Delegate_->c_list_door.InsertItem(index,"");
		Delegate_->c_list_door.SetItemText(index,1,info->GetMachineInfoForKey("MachineSer",0).c_str());
		Delegate_->c_list_door.SetItemText(index,2,info->GetMachineInfoForKey("MachineNo",0).c_str());
		Delegate_->c_list_door.SetItemText(index,3,info->GetMachineInfoForKey("MachinePlace",0).c_str());

		ip = info->GetMachineInfoForKey("IpAddress",0) + ":" + info->GetMachineInfoForKey("Port",0);
		msg =" 名单数：";
		msg+=info->GetMachineInfoForKey("NameListCount",0);
		msg+=" 记录数：";
		msg+=info->GetMachineInfoForKey("RecordCount",0);
		msg+=" 锁状态：";
		msg+=info->GetMachineInfoForKey("LockState",1);
		msg+=" 时间：";
		msg+=info->GetMachineInfoForKey("HeartTime",1);

		Delegate_->c_list_door.SetItemText(index,4,ip.c_str());
		SetListRowColors(index,5,info->GetConnectState());
		Delegate_->c_list_door.SetItemText(index,6,msg.c_str());
		index++;
		++iter;

		ShowAndWriteLog(info->GetMachineInfoForKey("MachineSer",0),"已获取!");
	}
}

void CDoorMonitorServiceDlgModule::SetListRowColors(int row,int col,bool state)
{
	std::string txt("已连接");
	int color = 0;
	if(!state)
	{
		txt="未连接";
		color=1;
	}

	Delegate_->c_list_door.SetItemText(row,col,txt.c_str());
	Delegate_->c_list_door.SetItemData(row, color);
}

void CDoorMonitorServiceDlgModule::ResetTime()
{
	dTimes=0;
	hTimes=0;
	mTimes=0;
	nTimes=0;
}

void CDoorMonitorServiceDlgModule::Run(int port)
{
	if(isRun)return;
	isRun = true;
	ResetTime();
	doorSvr->StartListen(port);
}

void CDoorMonitorServiceDlgModule::Stop()
{
	isRun = false;
	doorSvr->StopListen();
}

void CDoorMonitorServiceDlgModule::Disconnect(std::string mser)
{
	int index = GetListIndex(mser);

	MapMachine::iterator iter = mapMachineList.find(mser);
	if(iter!=mapMachineList.end())
	{
		doorSvr->ReleaseSocket(mser);//释放socket
	}
}

void CDoorMonitorServiceDlgModule::OnTimer(UINT_PTR nIDEvent)
{
	++nTimes;
	Delegate_->c_static_timelong.SetWindowText(get_longtime());
	int n=0;
	std::string mno;
	std::map<std::string,int>::iterator iter = mapMachineTime.begin();
	for(;iter!=mapMachineTime.end();++iter){
		n = iter->second;
		mno = iter->first;
		n = isDoConnet(mno,n);
		mapMachineTime[mno]=n;
	}

	//Delegate_->c_edit_log.Invalidate();
}

int CDoorMonitorServiceDlgModule::isDoConnet(std::string mno,int time)
{
	if(time>10){
		/*int index = GetListIndex(mno);
		if(index<0)
			return time+1;*/

		MapMachine::iterator iter = mapMachineList.find(mno);
		if(iter!=mapMachineList.end())
		{
			doorSvr->ReleaseSocket(mno);//释放socket
		}
	}
	
	return time+1;
}

bool CDoorMonitorServiceDlgModule::MachineIsConn(std::string mser)
{
	int index = -1;
	boost::shared_ptr<CMachine> info;
	if(!GetMachine(mser,info,index))
		return false;

	return info->GetConnectState();
}

void CDoorMonitorServiceDlgModule::SetMachineCheck(std::string mser,bool check)
{
	int index = -1;
	boost::shared_ptr<CMachine> info;
	if(!GetMachine(mser,info,index))
		return;

	info->SetCheck(check);
}

CString CDoorMonitorServiceDlgModule::get_longtime()
{
	SetValue(nTimes,mTimes,60);
	SetValue(mTimes,hTimes,60);
	SetValue(hTimes,dTimes,24);
	CString strTime;
	strTime.Format("本次监控时长：%02d天:%02d时:%02d分:%02d秒",dTimes,hTimes,mTimes,nTimes);
	return strTime;
}

void CDoorMonitorServiceDlgModule::SetValue(int& a,int& b,const int v)
{
	if (a >= v)
	{
		++b;
		a = 0;
	}
}

void CDoorMonitorServiceDlgModule::WaitForSendState(boost::shared_ptr<CMachine>& info)
{
	while(info->GetSendState())
	{
		Sleep(50);
	}
}

void CDoorMonitorServiceDlgModule::SendCommend(std::string mser,int mType)
{
	int index = GetListIndex(mser);
	mapMserList[index] = mser;
	switch(mType){
	case 0:
		//SendSelTime(mser);
		AfxBeginThread((AFX_THREADPROC)CDoorMonitorServiceDlgModule::SendSelTime,&mapMserList[index]);
		break;
	case 1:
		//SendSelParamater(mser);
		AfxBeginThread((AFX_THREADPROC)CDoorMonitorServiceDlgModule::SendSelParamater,&mapMserList[index]);
		break;
	case 2:
		//SendSelNameList(mser);
		AfxBeginThread((AFX_THREADPROC)CDoorMonitorServiceDlgModule::SendSelNameList,&mapMserList[index]);
		break;
	case 3:
		SendAllTime();
		break;
	case 4:
		SendAllParamater();
		break;
	case 5:
		SendAllNameList();
		break;
	default:
		break;
	}
}

UINT CALLBACK CDoorMonitorServiceDlgModule::SendSelTime(LPVOID  lparam)
{
	std::string mser(static_cast<std::string*>(lparam)->c_str());
	int index = -1;
	boost::shared_ptr<CMachine> info;
	if(!module_->GetMachine(mser,info,index))
		return -1;

	if(info->GetTaskState())
		return -1;//cmd task is imcomplete ,return;

	module_->SendMessageUpdate(index,mser,"正在校时...");
	info->SetTaskState(true);
	module_->WaitForSendState(info);
	info->SetSendState(true);

	std::string mser_ = mser;//ConversionManager::ReveseHex(mser);
	std::string time,cmd,hex;
	time = ConversionManager::GetTimeString(3);
	CommendManager::getInstance()->CommendSetTime(mser_,time,cmd);
	info->SendCommend(cmd,hex);

	module_->WaitForSendState(info);
	info->SetTaskState(false);

	module_->SendMessageUpdate(index,mser,"校时完成");
	return 0;
}

UINT CALLBACK CDoorMonitorServiceDlgModule::SendSelParamater(LPVOID  lparam)
{
	std::string mser(static_cast<std::string*>(lparam)->c_str());
	int index = -1;
	boost::shared_ptr<CMachine> info;
	if(!module_->GetMachine(mser,info,index))
		return -1;

	if(info->GetTaskState())
		return -1;//cmd task is imcomplete ,return;

	module_->SendMessageUpdate(index,mser,"正在更新参数...");
	info->SetTaskState(true);
	module_->WaitForSendState(info);
	info->SetSendState(true);

	std::string mser_ = mser;//ConversionManager::ReveseHex(mser);
	std::string cmd,hex;
	bool ret  = CommendManager::getInstance()->CommendSetParamater(mser_,cmd);
	if(ret)
	{
		info->SendCommend(cmd,hex);
	}
	else
	{
		module_->SendMessageUpdate(index,mser,"更新参数失败");
		info->SetSendState(false);
		return -1;
	}

	module_->WaitForSendState(info);
	info->SetTaskState(false);
	module_->SendMessageUpdate(index,mser,"更新参数完成");
	return 0;
}

UINT CALLBACK CDoorMonitorServiceDlgModule::SendSelNameList(LPVOID  lparam)
{
	std::string mser(static_cast<std::string*>(lparam)->c_str());
	int index = -1;
	boost::shared_ptr<CMachine> info;
	if(!module_->GetMachine(mser,info,index))
		return -1;
	
	if(info->GetTaskState())
		return -1;//cmd task is imcomplete ,return;

	module_->SendMessageUpdate(index,mser,"清空名单中...");
	info->SetTaskState(true);
	//等待下传命令返回的确认命令信号
	module_->WaitForSendState(info);
	info->SetSendState(true);

	std::string mser_ = mser;//ConversionManager::ReveseHex(mser);
	std::string cmd,hex;
	//清除名单
	CommendManager::getInstance()->CommendNameListClear(mser_,cmd);
	info->SendCommend(cmd,hex);

	module_->WaitForSendState(info);
	//info->SetSendState(true);
	module_->SendMessageUpdate(index,mser,"清空名单完成，开始下传名单...");
	//下传名单
	std::vector<std::string> vclist;
	cmd=hex="";
	bool ret  = CommendManager::getInstance()->CommendNameList(mser_,vclist);
	if(!ret)
	{
		info->SetSendState(false);
		info->SetTaskState(false);
		module_->SendMessageUpdate(index,mser,"下传名单失败0");
		return -1;
	}

	if(vclist.empty())
	{
		info->SetSendState(false);
		info->SetTaskState(false);
		module_->SendMessageUpdate(index,mser,"下传名单失败1");
		return -1;
	}

	vclist.push_back("FFFFFFFFFFFFFFFF");//add 1 because  the last namelist must be (ff ff ff ff ff ff ff ff)
	const int max_count_once=64;
	int name_count = vclist.size();
	std::vector<std::string>::iterator iter = vclist.begin();
	//const int send_count = floor((double)name_count/(double)max_count_once);
	int current_count = 0;
	while(current_count<name_count){
		module_->WaitForSendState(info);//wait for the comfirm signal in info object call send_state_;
		info->SetSendState(true);
		int count = name_count-current_count;
		if(max_count_once<count){
			count = max_count_once;
		}

		int datalen = count*8+3;
		cmd = mser + ConversionManager::IntToHex(datalen) + "2030" + ConversionManager::IntToHex(count,2);
		iter += current_count;
		std::vector<std::string> curList(iter,iter+count);
		std::vector<std::string>::iterator it = curList.begin();
		while(it!=curList.end())
		{
			cmd += *it;
			++it;
		}

		info->SendCommend(cmd,hex);
		current_count+=count;
	}

	module_->WaitForSendState(info);
	info->SetTaskState(false);//set task tag to complete state;
	module_->SendMessageUpdate(index,mser,"下传名单完成");
	return 0;
}

void CDoorMonitorServiceDlgModule::SendAllTime()
{
	MapMachine::iterator iter = mapMachineList.begin();
	int index = 0;
	while(iter!=mapMachineList.end()){
		boost::shared_ptr<CMachine> info=iter->second;
		if(info->GetCheck() && info->GetConnectState()){
			std::string mser =info->GetMachineSer();
			mapMserList[index] = mser;
			AfxBeginThread((AFX_THREADPROC)CDoorMonitorServiceDlgModule::SendSelTime,&mapMserList[index]);
		}

		++index;
		++iter;
	}
}

void CDoorMonitorServiceDlgModule::SendAllParamater()
{
	MapMachine::iterator iter = mapMachineList.begin();
	int index = 0;
	while(iter!=mapMachineList.end()){
		boost::shared_ptr<CMachine> info=iter->second;
		if(info->GetCheck() && info->GetConnectState()){
			std::string mser =info->GetMachineSer();
			mapMserList[index] = mser;
			AfxBeginThread((AFX_THREADPROC)CDoorMonitorServiceDlgModule::SendSelParamater,&mapMserList[index]);
		}

		++index;
		++iter;
	}
}

void CDoorMonitorServiceDlgModule::SendAllNameList()
{
	MapMachine::iterator iter = mapMachineList.begin();
	int index = 0;
	while(iter!=mapMachineList.end()){
		boost::shared_ptr<CMachine> info=iter->second;
		if(info->GetCheck() && info->GetConnectState()){
			std::string mser =info->GetMachineSer();
			mapMserList[index] = mser;
			AfxBeginThread((AFX_THREADPROC)CDoorMonitorServiceDlgModule::SendSelNameList,&mapMserList[index]);
		}

		++index;
		++iter;
	}
}

void CDoorMonitorServiceDlgModule::SendMessageUpdate(int index,std::string mser,std::string msg)
{
	Delegate_->c_list_door.SetItemText(index,7,msg.c_str());
	ShowAndWriteLog(mser,msg,3);
}

int CDoorMonitorServiceDlgModule::GetListIndex(std::string mser)
{
	int count = Delegate_->c_list_door.GetItemCount();
	int index = -1;
	for(int i=0;i<count;++i)
	{
		std::string mSer = Delegate_->c_list_door.GetItemText(i,1);
		if(mser==mSer){
			index=i;
			break;
		}
	}

	return index;
}

void CDoorMonitorServiceDlgModule::ShowAndWriteLog(std::string mser,std::string strmsg,int mType)
{
	int rows = Delegate_->c_edit_log.GetLineCount();
	if(rows>100)
		Delegate_->c_edit_log.SetWindowTextA("");

	std::string strtime = ConversionManager::GetTimeString(0);
	std::string msg = strtime + "机器【" + mser + "】" + strmsg;
 
	Delegate_->c_edit_log.Invalidate(FALSE);

	int iLen = Delegate_->c_edit_log.GetWindowTextLength();
	Delegate_->c_edit_log.SetSel(iLen, iLen);
	Delegate_->c_edit_log.ReplaceSel(msg.c_str());
	Delegate_->c_edit_log.ReplaceSel("\r\n");
	//Delegate_->c_edit_log.Invalidate();

	LogManager::getInstance()->writeLog(msg,mType);
	//心跳计时，超过时长判断连接已断开
	mapMachineTime[mser]=0;
}

bool CDoorMonitorServiceDlgModule::GetMachine(std::string mser,boost::shared_ptr<CMachine>& info,int& index)
{
	MapMachine::iterator iter = mapMachineList.find(mser);
	if(iter==mapMachineList.end())
		return false;

	index = GetListIndex(mser);
	if(index<0)
		return false;

	info = iter->second;

	return true;
}

void CDoorMonitorServiceDlgModule::MessageCallback(std::string mser,std::string msg)
{
	int index = -1;
	boost::shared_ptr<CMachine> info;
	if(!GetMachine(mser,info,index))
		return;

	SetListRowColors(index,5,info->GetConnectState());
	ShowAndWriteLog(mser,msg);
}

void CDoorMonitorServiceDlgModule::DealSendInfo(std::string mser)
{
	int index = -1;
	boost::shared_ptr<CMachine> info;
	if(!GetMachine(mser,info,index))
		return;
	//info->SetSendState(false);
	ShowAndWriteLog(mser,info->GetSendMessage());
}

void CDoorMonitorServiceDlgModule::DealHeartInfo(std::string mser)
{
	int index = -1;
	boost::shared_ptr<CMachine> info;
	if(!GetMachine(mser,info,index))
		return;
	std::string strmsg="";
	std::string ip="";
	std::string msg="";

	ip = info->GetMachineInfoForKey("IpAddress",0) + ":" + info->GetMachineInfoForKey("Port",0);
	msg+=" 名单数：";
	msg+=info->GetMachineInfoForKey("NameListCount",0);
	msg+=" 记录数：";
	msg+=info->GetMachineInfoForKey("RecordCount",0);
	msg+=" 锁状态：";
	msg+=info->GetMachineInfoForKey("LockState",1);
	msg+=" 时间：";
	msg+=info->GetMachineInfoForKey("HeartTime",1);

	Delegate_->c_list_door.SetItemText(index,4,ip.c_str());
	SetListRowColors(index,5,info->GetConnectState());
	Delegate_->c_list_door.SetItemText(index,6,msg.c_str());
	
	if(info->GetConnectState()){
		if(info->GetHeartCount()>0)
			strmsg ="心跳信息更新";
		else
		{
			//update database
			MAPUpdateSet mapUpdateSet;
			mapUpdateSet.insert(std::make_pair("IpAddress",info->GetMachineInfoForKey("IpAddress",0)));
			mapUpdateSet.insert(std::make_pair("Port",info->GetMachineInfoForKey("Port",0)));
			std::string mser_ = mser;//ConversionManager::ReveseHex(mser);//release 去除
			bool ret = CommendManager::getInstance()->updateMachineInfo(mser_,mapUpdateSet);
			strmsg ="连接成功";
		}
	}
	else
	{
		strmsg ="断开连接";
	}

	ShowAndWriteLog(mser,strmsg);
}

void CDoorMonitorServiceDlgModule::DealRecordInfo(std::string mser)
{
	int index = -1;
	boost::shared_ptr<CMachine> info;
	if(!GetMachine(mser,info,index))
		return;

	SetListRowColors(index,5,info->GetConnectState());
	//update database code scope
	int mType = 0;//0 is normal record info,and 1 is identification card info
	bool ret = true;
	std::map<std::string,std::string> recordInfo;
	std::string table	= "Door_TrafficRecord" + ConversionManager::GetTimeString(4);
	std::string mno		= info->GetMachineInfoForKey("MachineNo",0);
	std::string cardno	= info->GetRecordInfoForKey("CardNo16",1);
	std::string doorno	= info->GetRecordInfoForKey("DoorNo",1);
	recordInfo["TrafficCode"] =info->GetRecordInfoForKey("TrafficCode",1);
	CMD5 md5;
	recordInfo["TrafficCode"] = md5.md5(recordInfo["TrafficCode"]);
	if(doorno=="03"){
		//identification card
		mType=1;
		recordInfo["MachineNo"]=info->GetMachineInfoForKey("MachineNo",0);
		recordInfo["MachineId"]=info->GetMachineInfoForKey("Id",0);
		recordInfo["Time"]=info->GetRecordInfoForKey("TrafficTime",1);
		cardno	= ConversionManager::HexToIdentify(cardno);
		table	= "Spec_Door_VisitorsTrafficRecord";
	}else{
		//ceate table
		cardno = ConversionManager::GetStringRight(cardno,8);
		recordInfo["IsValid"]="1";
		recordInfo["TrafficTime"]=info->GetRecordInfoForKey("TrafficTime",1);
		ret = CommendManager::getInstance()->CreateRecordTable(table);
		if(!ret)return;
	}

	ret = CommendManager::getInstance()->getRecordInfo(mno,cardno,recordInfo,mType);
	if(!ret)return;

	ret = CommendManager::getInstance()->updateRecordInfo(table,recordInfo["TrafficCode"],recordInfo);
	if(!ret)return;

	//show record info
	std::string strmsg;
	std::string strinfo;

	strinfo = "  包编号："	 + info->GetRecordInfoForKey("PacketNo",1) + 
			  "  门编号："	 + info->GetRecordInfoForKey("DoorNo",1) + 
			  "  卡号："	 + info->GetRecordInfoForKey("CardNo16",1) + 
			  "  记录类型：" + info->GetRecordInfoForKey("RecordType",1) + 
			  "  刷卡时间：" + info->GetRecordInfoForKey("RecordTime",1);

	strmsg = " 刷卡信息->" + strinfo;

	ShowAndWriteLog(mser,strmsg);

	//return comfirm commend to low machine
	info->SendCmdRecord();
}

void CDoorMonitorServiceDlgModule::DealPermissionInfo(std::string mser)
{
	int index = -1;
	boost::shared_ptr<CMachine> info;
	if(!GetMachine(mser,info,index))
		return;

	int mType = 0;//0 is normal record info,and 1 is identification card info
	bool ret = true;
	std::string mno		= info->GetMachineInfoForKey("MachineNo",0);
	std::string cardno	= info->GetRecordInfoForKey("CardNo16",1);
	std::string doorno	= info->GetRecordInfoForKey("DoorNo",1);
	doorno = ConversionManager::GetStringRight(doorno,1);
	if(doorno=="3"){
		//identification card
		mType=1;
		cardno	= ConversionManager::HexToIdentify(cardno);
	}else{
		cardno = ConversionManager::GetStringRight(cardno,8);
	}

	ret = CommendManager::getInstance()->getPermission(mno,cardno,doorno,mType);
	if(ret){
		SendMessageUpdate(index,mser,"机号：" + mno + "，门号：" + doorno + "，卡号：" + cardno + " ：有开门权限，开门");
		info->SendCmdPermission();
		//open door commend 
		info->SendCmdOpenDoor(doorno);
	}else{
		SendMessageUpdate(index,mser,"机号：" + mno + "，门号：" + doorno + "，卡号：" + cardno + " ：无开门权限！");
	}
}

void CDoorMonitorServiceDlgModule::DealCommonInfo(std::string mser)
{
	int index = -1;
	boost::shared_ptr<CMachine> info;
	if(!GetMachine(mser,info,index))
		return;

	SetListRowColors(index,5,info->GetConnectState());
	ShowAndWriteLog(mser,info->GetRevMessage());
}