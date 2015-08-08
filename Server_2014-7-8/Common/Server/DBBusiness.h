#pragma once

#include "General.h"
#include "SQLServerInterface.h"
#include "EntityS.h"

#ifdef VE_DEBUG
#	define dprintf printf
#else
#	define dprintf //
#endif

////用户名
//#define DB_USERNAME "venus"
////密码
//#define DB_PASSWORD "abc123!@#"
////ODBC数据源
//#define DB_SOURCE "TexasSqlServer"

#define DB_VARCHAR_LENGTH (512)

class PropertySValue;
class PropertySContainer;

class DBBusiness: public VeMemObject
{
public:
	enum Result{
		RESULT_OK,
		RESULT_CREATE,
		RESULT_FAIL
	};

	DBBusiness(const char* name,const char* password,const char* source);

	virtual ~DBBusiness();

	void DataToString(void* data,char* str,int type);

	VeStringA TrimString(char* data);

	int GetProperty(PropertySValue* prop);

	int AddProperty(PropertySValue* prop,PropertySContainer* pkContainer);

	int CreateTable(PropertySContainer* pkContainer);

	int Load(PropertySContainer* pkContainer);

	int Update(PropertySContainer* pkContainer);

	int Insert(PropertySContainer* pkContainer);

	int Delete(PropertySContainer* pkContainer);

	VeVector< VePair<VeString, VeInt32> > checkPurchase(const char* uuid);

protected:

private:
	CConnection* mConnection;
	CDBAccess mDbAccess;

};
