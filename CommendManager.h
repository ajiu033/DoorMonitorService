#pragma once
class CommendManager
{
public:
	typedef boost::function<void(std::string,std::string,bool)> FunctorCallback;

	static CommendManager* getInstance();
	static void clearInstance();
	void   connect(std::string serviceIP,std::string database,
					std::string username,std::string password);
	void SetFunctorCallback(const FunctorCallback& cb)
	{functorCallback_=cb;}
	//get info
	bool getMachineInfo(MAPInfo& mapInfo);
	bool getRecordInfo(std::string mno,std::string cardno,std::map<std::string,std::string>& recordInfo,int mType=0);
	bool getPermission(std::string mno,std::string cardno,std::string doorno,int mType=0);
	bool CreateRecordTable(std::string table);
	//update
	bool updateMachineInfo(std::string mser,MAPUpdateSet& mapUpdateSet);
	bool updateRecordInfo(std::string table,std::string trafficCode,MAPUpdateSet& mapUpdateSet);
	//set commend string
	bool CommendNameList(std::string mno,std::vector<std::string>& vccmd); 
	void CommendNameListClear(std::string mno,std::string& cmd);
	void CommendSetTime(std::string mno,std::string time,std::string& cmd);
	bool CommendSetParamater(std::string mno,std::string& cmd);
private:
	CommendManager(void);
	~CommendManager(void);
	std::string SingleName(std::string cardser,std::string doorno,std::string jno);

	FunctorCallback functorCallback_;
	static CommendManager* cmdManager_;
	static bool IsNameListRunning;
	static bool IsSetTimeRunning;

};

