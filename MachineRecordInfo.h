#pragma once
class CMachineRecordInfo
{
public:
	static void InitalInfo();
	CMachineRecordInfo(void);
	~CMachineRecordInfo(void);

	void ParseCommend(std::vector<unsigned char>& vcCmd,int mType);
	void SetFieldInfo(std::string mser,std::vector<std::string> vcinfo);
	std::string GetFieldInfoForKey(std::string key);
	void SetRecordInfoForKey(std::string key,std::string value);
	std::string GetRecordInfoForKey(std::string key);
	void Reset();

private:
	typedef std::map<std::string,std::string>	MapInfo;
	MapInfo mapField_;
	MapInfo mapRecord_;

	int mType_;//0xaa=0,0x55=1,unknown=-1
	void ParseTime(std::string& strHex,std::string& strTime);
public:
	static std::vector<std::string> vcField;
	static std::vector<std::string> vcFieldIdentify;
private:
	static std::vector<std::string> vcRecord;
};

