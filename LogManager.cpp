#include "StdAfx.h"
#include "LogManager.h"

LogManager* LogManager::logManager_=nullptr;

LogManager::LogManager(void)
{
	char buf[MAX_PATH]; 
	GetModuleFileNameA(GetModuleHandle(NULL), buf, sizeof buf); 
	CString main_path(buf);
	int npos=main_path.ReverseFind('\\');
	main_path=main_path.Mid(0,npos);
	RootPath_ = main_path +"\\DoorMonitorLog";

	if (!PathFileExists(RootPath_.c_str()))
	{
		CreateDirectory(RootPath_.c_str(),NULL);
	};
}

LogManager::~LogManager(void)
{
	if(logFile)logFile.close();
	if(logErr)logErr.close();
}

std::string LogManager::getTime(int mType)
{
	CTime nowtime;
	nowtime = CTime::GetCurrentTime();
	CString   strtime;
	if (mType==0)
	{
		strtime.Format("%.2d-%.2d-%.2d %.2d:%.2d:%.2d:", 
						nowtime.GetYear(),
						nowtime.GetMonth(),
						nowtime.GetDay(),
						nowtime.GetHour(), 
						nowtime.GetMinute(), 
						nowtime.GetSecond());
	}else if(mType==1)
	{
		strtime.Format("%.2d%.2d%.2d", 
						nowtime.GetYear(), 
						nowtime.GetMonth(), 
						nowtime.GetDay());
	}else if(mType==2)
	{
		strtime.Format("%.2d%.2d%.2d%.2d", 
						nowtime.GetYear(), 
						nowtime.GetMonth(), 
						nowtime.GetDay(),
						nowtime.GetHour());
	}

	return strtime.GetBuffer();
}

void LogManager::open(std::ofstream& ofile,std::string logname)
{
	std::string subLogPath = RootPath_ +"\\" + getTime(1);
	if (!PathFileExists(subLogPath.c_str()))
	{
		CreateDirectory(subLogPath.c_str(),NULL);
	}

	subLogPath += "\\";
	subLogPath += logname;

	if (!PathFileExists(subLogPath.c_str()))
	{
		if(ofile)ofile.close();
		std::string tempfile = subLogPath;
		ofile.open(tempfile.c_str(), std::ios::trunc | std::ios::out);
		ofile.close();

		ofile.open(tempfile.c_str(), std::ios::app);
	}

	if(!ofile.is_open())
	{
		if(ofile)ofile.close();
		std::string tempfile = subLogPath;
		ofile.open(tempfile.c_str(), std::ios::trunc | std::ios::out);
		ofile.close();

		ofile.open(tempfile.c_str(), std::ios::app);
	}
}

void LogManager::openLog()
{
	std::string logname("DoorMonitor");
	logname += getTime(2);
	logname += ".log";
	open(logFile,logname);
}

void LogManager::openErr()
{
	std::string logname("DayOperation.log");
	open(logErr,logname);
}

void LogManager::writeLog(std::string strLog,int mType)
{
	if(mType==0){
		openLog();
		logFile<< strLog << std::endl;
	}
	else{
		openErr();
		logErr<< strLog << std::endl;
	}
}

LogManager* LogManager::getInstance()
{
	if (nullptr == logManager_)
    {
        logManager_ = new LogManager();
    }
    return logManager_;
}

void LogManager::clearInstance()
{
	delete logManager_;
	logManager_=NULL;
}