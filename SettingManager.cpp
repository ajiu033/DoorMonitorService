#include "StdAfx.h"
#include "SettingManager.h"
#include "ConnectDatabaseDlg.h"

SettingManager* SettingManager::settingManager_=nullptr;

SettingManager* SettingManager::getInstance()
{
	if (NULL == settingManager_)
    {
        settingManager_ = new SettingManager();
    }
    return settingManager_;
}

void SettingManager::clearInstance()
{
	delete settingManager_;
	settingManager_=nullptr;
}

SettingManager::SettingManager(void)
{
	char buf[MAX_PATH]; 
	GetModuleFileNameA(GetModuleHandle(NULL), buf, sizeof buf); 
	CString main_path(buf);
	int npos=main_path.ReverseFind('\\');
	main_path=main_path.Mid(0,npos);
	iniPath_ = main_path +"\\Setting.ini";
}


SettingManager::~SettingManager(void)
{
}

bool SettingManager::WriteString(std::string section,std::string key,std::string value)
{
	return WritePrivateProfileString(section.c_str(),key.c_str(),value.c_str(),iniPath_.c_str());
}

void SettingManager::ReadString(std::string section,std::string key,std::string& value)
{
	value="";
	char cvalue[MAX_PATH]={0};
	GetPrivateProfileString(section.c_str(),key.c_str(),"null",cvalue,MAX_PATH,iniPath_.c_str());
	value = cvalue;
}

int SettingManager::ReadInt(std::string section,std::string key)
{
	return GetPrivateProfileInt(section.c_str(),key.c_str(),0,iniPath_.c_str());;
}

void SettingManager::GetDatabaseSetting(std::vector<std::string>& vcsetting)
{
	std::string value;
	ReadString("DATABASE","IP",value);
	if(vcsetting.size()<4)
		vcsetting.push_back(value);
	else 
		vcsetting[0]=value;

	ReadString("DATABASE","DB",value);
	if(vcsetting.size()<4)
		vcsetting.push_back(value);
	else 
		vcsetting[1]=value;

	ReadString("DATABASE","USER",value);
	if(vcsetting.size()<4)
		vcsetting.push_back(value);
	else 
		vcsetting[2]=value;

	ReadString("DATABASE","PWD",value);
	if(vcsetting.size()<4)
		vcsetting.push_back(value);
	else 
		vcsetting[3]=value;
}

bool SettingManager::SetDatabaseSetting(std::vector<std::string>& vcsetting)
{
	bool ret=true;
	if(vcsetting.size()!=4)
		return false;

	WriteString("DATABASE","IP",vcsetting[0]);
	WriteString("DATABASE","DB",vcsetting[1]);
	WriteString("DATABASE","USER",vcsetting[2]);
	WriteString("DATABASE","PWD",vcsetting[3]);

	return ret;
}