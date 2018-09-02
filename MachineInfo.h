#pragma once
class CMachineInfo
{
public:
	static void InitalInfo();
	CMachineInfo(void);
	CMachineInfo(const CMachineInfo& info);
	~CMachineInfo(void);

	void ParseCommend(std::vector<unsigned char>& vcCmd);
	void UpdateHeartCount();
	void SetMachineInfo(std::string mser,std::vector<std::string> vcinfo);
	void SetMachineInfoForKey(std::string key,std::string value);
	std::string GetMachineInfoForKey(std::string key);
	std::string GetHeartInfoForKey(std::string key);
	unsigned long long GetHeartCount();
	void Reset();
private:
	//base info
	typedef std::map<std::string,std::string>	MapInfo;
	MapInfo mapInfo_;
	//beat heart info
	MapInfo mapHeartInfo_;
	unsigned long long heartCount_;
	void ParseTime(std::string& strHex,std::string& strTime);
	void SetHeartInfoForKey(std::string key,std::string value);
public:
	static std::vector<std::string> vcField;
	static std::vector<std::string> vcHeart;
	
};

