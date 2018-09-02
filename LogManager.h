#pragma once
class LogManager
{
public:
	static LogManager* getInstance();
	static void clearInstance();
	void writeLog(std::string strLog,int mType=0);
private:
	LogManager(void);
	~LogManager(void);
	std::string getTime(int mType);
	void openLog();
	void openErr();
	void open(std::ofstream& ofile,std::string logname);

	std::ofstream logFile;
	std::ofstream logErr;
	std::string RootPath_;

	static LogManager* logManager_;
	
};

