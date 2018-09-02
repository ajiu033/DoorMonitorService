#pragma once
#include "boost\tuple\tuple.hpp"

class DatabaseManager
{
public:
	static DatabaseManager* getInstance();
	static void clearInstance();
	//common function
	bool connectDB(std::string serviceIP,std::string database,
					std::string username,std::string password);
	bool queryDB(std::string sql,std::vector<std::string>& vcField, VCRecordset& vcRs);
	bool queryDB(std::string sql);
	bool updateDB(std::string sql,MAPUpdateSet& mapUpdateSet);
	void deleteDB(std::string sql);
	bool CreateTable(std::string sql,std::string table);
private:
	DatabaseManager(void);
	~DatabaseManager(void);
	bool TableIsExist(std::string table);

	_ConnectionPtr pConnection_;

	static DatabaseManager* dbManager_;
	static bool isConn;
	
};

