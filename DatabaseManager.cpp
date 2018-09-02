#include "StdAfx.h"
#include "DatabaseManager.h"

DatabaseManager* DatabaseManager::dbManager_=NULL;
bool DatabaseManager::isConn = false;

DatabaseManager::DatabaseManager(void)
{
	CoInitialize(NULL); 
	pConnection_=NULL;
}


DatabaseManager::~DatabaseManager(void)
{

}

DatabaseManager* DatabaseManager::getInstance()
{
	if (NULL == dbManager_)
    {
        dbManager_ = new DatabaseManager();
    }
    return dbManager_;
}

void DatabaseManager::clearInstance()
{
	if(dbManager_==NULL)return;
	if(dbManager_->pConnection_->GetState()!=0)
		dbManager_->pConnection_->Close();
	dbManager_->pConnection_=NULL;
	delete dbManager_;
	dbManager_=NULL;
	isConn=false;
	CoUninitialize();
}

bool DatabaseManager::connectDB(std::string serviceIP,std::string database,
					std::string username,std::string password)
{

	if(isConn)return true;

	isConn = true;
	HRESULT hr=pConnection_.CreateInstance(_uuidof(Connection));
	if(FAILED(hr))
		return false;

	pConnection_->ConnectionTimeout = 8;
	try
	{	//在COM接口中，参数若为字符串， 一定要用BSTR  
		std::string cnnstr = "Driver={sql server};server=" + serviceIP + ";uid=" + username + ";pwd=" + password + ";database=" + database + ";";
		//"Driver={sql server};server=127.0.0.1,1433;uid=mysa;pwd=11;database=CTXSYS;";
		_bstr_t con_bstr= cnnstr.c_str();
		pConnection_->Open(con_bstr,"","",adModeUnknown);// adModeUnknown 缺省，当前的许可权未设置
	}
	catch(_com_error &e) 
	{
		//cout<<e.Description()<<endl;
		isConn = false;
	}

	return isConn;

}

bool DatabaseManager::TableIsExist(std::string table)
{
	if(!isConn)return false;

	std::string sql = "SELECT id FROM sysobjects WHERE name='" + table + "' AND type='u'";

	bool ret = false;
	_RecordsetPtr pRecordset;
	pRecordset.CreateInstance(_uuidof(Recordset));
	_bstr_t bstrSQL(sql.c_str());
	try {
		pRecordset->Open(bstrSQL,pConnection_.GetInterfacePtr(),adOpenForwardOnly,adLockReadOnly,adCmdText);
		if(!pRecordset->adoEOF) 
			ret = true;
	}catch (_com_error &e)
	{
		AfxMessageBox(e.Description());
		ret = false;
	}

	pRecordset->Close();
	return ret;
}

bool DatabaseManager::CreateTable(std::string sql,std::string table)
{
	if(!isConn)return false;
	if(!TableIsExist(table))
	{
		pConnection_->Execute(sql.c_str(),NULL,adCmdText); 
	}

	return true;
}

bool DatabaseManager::queryDB(std::string sql,std::vector<std::string>& vcField, VCRecordset& vcRs)
{
	if(!isConn)return false;

	bool ret = true;
	_RecordsetPtr pRecordset;
	pRecordset.CreateInstance(_uuidof(Recordset));
	_bstr_t bstrSQL(sql.c_str());
	try {
		pRecordset->Open(bstrSQL,pConnection_.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
		while(!pRecordset->adoEOF) 
		{
			std::vector<std::string> vcRow;
			int fieldcount = vcField.size();
			std::vector<std::string>::iterator iter = vcField.begin();
			for(;iter!=vcField.end();++iter)
			{
				std::string fieldname = *iter;
				_variant_t var = pRecordset->GetCollect(fieldname.c_str());
				if(var.vt!=VT_NULL)
				{
					std::string value((_bstr_t)var); 
					vcRow.push_back(value);
				}else{
					std::string value(""); 
					vcRow.push_back(value);
				}
			}

			vcRs.push_back(vcRow);
			pRecordset->MoveNext();
		}
	}
	catch (_com_error &e)
	{
		AfxMessageBox(e.Description());
		ret = false;
	}

	pRecordset->Close();
	return ret;
}

bool DatabaseManager::queryDB(std::string sql)
{
	if(!isConn)return false;

	bool ret = true;
	_RecordsetPtr pRecordset;
	pRecordset.CreateInstance(_uuidof(Recordset));
	_bstr_t bstrSQL(sql.c_str());
	try {
		pRecordset->Open(bstrSQL,pConnection_.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
		if(pRecordset->adoEOF) 
			ret = false;
	}
	catch (_com_error &e)
	{
		AfxMessageBox(e.Description());
		ret = false;
	}

	pRecordset->Close();
	return ret;
}

bool DatabaseManager::updateDB(std::string sql,MAPUpdateSet& mapUpdateSet)
{
	if(!isConn)
		return false;

	bool ret = true;
	_RecordsetPtr pRecordset;
	pRecordset.CreateInstance(_uuidof(Recordset));
	_bstr_t bstrSQL(sql.c_str());
	try {
		pRecordset->Open(bstrSQL,pConnection_.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
		//pRecordset->MoveFirst();
		if(pRecordset->adoEOF) 
		{
			pRecordset->AddNew();//insert
		}

		MAPUpdateSet::iterator iter  = mapUpdateSet.begin();
		while(iter!=mapUpdateSet.end())
		{
			std::string field = iter->first;
			std::string value = iter->second;
			pRecordset->PutCollect(field.c_str(),_variant_t(value.c_str()));
			++iter;
		}

		pRecordset->Update();
	}
	catch (_com_error &e)
	{
		AfxMessageBox(e.Description());
		ret = false;
	}

	pRecordset->Close();
	return ret;
}

void DatabaseManager::deleteDB(std::string sql)
{
	if(!isConn)return;
	pConnection_->Execute(sql.c_str(),NULL,1); 
}
