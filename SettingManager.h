#pragma once
class SettingManager
{
public:
	static SettingManager* getInstance();
	static void clearInstance();

	//general
	bool WriteString(std::string section,std::string key,std::string value);
	void ReadString(std::string section,std::string key,std::string& value);
	int ReadInt(std::string section,std::string key);
	//specific
	void GetDatabaseSetting(std::vector<std::string>& vcsetting);
	bool SetDatabaseSetting(std::vector<std::string>& vcsetting);
private:
	SettingManager(void);
	~SettingManager(void);

	std::string iniPath_;

	static SettingManager* settingManager_;
};

