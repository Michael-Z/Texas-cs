#include "General.h"
#include "DBBusiness.h"

DBBusiness::~DBBusiness()
{
	//断开到数据库的连接
	mDbAccess.CloseStmt();
	if(!mDbAccess.Disconnect())
	{
		dprintf("error disconnect:%s",mDbAccess.GetLastError());
		dprintf("\n");
	}

	mDbAccess.Close();
}

DBBusiness::DBBusiness(const char* name,const char* password,const  char* source)
{
	mConnection = NULL;
	if(!name || !password || !source) return;
	//初始化
	if(!mDbAccess.Init(NULL, &mConnection, 10, 10, 10))
	{
		printf("database Init failed:",mDbAccess.GetLastError());
		printf("\n");
	}

	//连接数据库
	if(!mDbAccess.Connect(source, name, password))
	{
		printf("database Connect failed:",mDbAccess.GetLastError());
		printf("\n");
	}
	else
		printf("database Connect success!\n");
}

VeStringA DBBusiness::TrimString(char* data)
{
	VeStringA res = data;
	res.TrimLeft();
	res.TrimRight();
	res.Replace("'","''");
	return res;
}

void  DBBusiness::DataToString(void* data,char* str,int type)
{
	if(!data)
		return;
	switch(type)
	{
	case TYPE_VeUInt8:
		_i64toa(*((VeUInt8*)data), str, 10);
		break;
	case TYPE_VeUInt16:
		_i64toa(*((VeUInt16*)data), str, 10);
		break;
	case TYPE_VeUInt32:
		_i64toa(*((VeUInt32*)data), str, 10);
		break;
	case TYPE_VeUInt64:
		_i64toa(*((VeUInt64*)data), str, 10);
		break;
	case TYPE_VeInt8:
		_i64toa(*((VeInt8*)data), str, 10);
		break;
	case TYPE_VeInt16:
		_i64toa(*((VeInt16*)data), str, 10);
		break;
	case TYPE_VeInt32:
		_i64toa(*((VeInt32*)data), str, 10);
		break;
	case TYPE_VeInt64:
		_i64toa(*((VeInt64*)data), str, 10);
		break;
	}
}

int DBBusiness::GetProperty(PropertySValue* prop)
{
	if(prop && prop->GetField() && mDbAccess.IsConnect())
	{
		int type = prop->GetType();
		long nRetLen;
		if(type==TYPE_VeStringA)
		{
			VeChar8 buffer[DB_VARCHAR_LENGTH]={0};
			if(!mDbAccess.GetData(prop->GetField(),buffer, sizeof(buffer), &nRetLen))
			{
				dprintf("获取字段信息失败：%s\n",prop->GetField());
				return RESULT_FAIL;
			}
			else
			{
				prop->SetData(buffer,nRetLen);
				dprintf("获得字段信息：%s=%s\n",prop->GetField(),buffer);
			}
		}
		else
		{
			if(type==TYPE_VeInt64 || type==TYPE_VeUInt64)
			{
				VeInt64 lTestBigInt = 0;
				if(!mDbAccess.GetData(prop->GetField(), &(lTestBigInt), sizeof(VeInt64), &nRetLen, SQL_C_SBIGINT))
				{
					dprintf("获取字段信息失败：%s\n",prop->GetField());
					return RESULT_FAIL;
				}  
				else
				{
					char str[128]={0};
					prop->SetData(&lTestBigInt,nRetLen);
					dprintf("获得字段信息：%s=%s\n",prop->GetField(),_i64toa(lTestBigInt,str,10));
				}
			}else{
				VeInt32 lTestBigInt = 0;
				if(!mDbAccess.GetData(prop->GetField(), &(lTestBigInt), sizeof(VeInt32), &nRetLen, SQL_C_SLONG))
				{
					dprintf("获取字段信息失败：%s\n",prop->GetField());
					return RESULT_FAIL;
				}  
				else
				{
					char str[128]={0};
					prop->SetData(&lTestBigInt,nRetLen);
					dprintf("获得字段信息：%s=%s\n",prop->GetField(),_itoa(lTestBigInt,str,10));
				}
			}
		}
		return RESULT_OK;
	}else
		return RESULT_FAIL;
}

int DBBusiness::AddProperty(PropertySValue* prop,PropertySContainer* pkContainer)
{
	if(prop && prop->GetField() && pkContainer && pkContainer->GetTableName() && mDbAccess.IsConnect())
	{
		int type = prop->GetType();
		VeStringA sqlStr;
		VeStringA tableName = pkContainer->GetTableName();
		VeStringA field = prop->GetField();

		sqlStr = "alter table "+tableName+" add "+field+" ";
		if(type==TYPE_VeStringA)
		{
			char str[128]={0};
			sprintf(str,"%d",DB_VARCHAR_LENGTH);
			sqlStr += "varchar(";
			sqlStr += str;
			sqlStr += ")";
			if(prop->GetDefaultData())
			{
				VeStringA defaultData = TrimString((char*)prop->GetDefaultData());
				sqlStr += " DEFAULT '"+defaultData+"' with values";
			}
		}
		else
		{
			if(type==TYPE_VeInt64 || type==TYPE_VeUInt64)
				sqlStr += "bigint";
			else
				sqlStr += "int";
			if(prop->GetDefaultData())
			{
				char str[128]={0};
				DataToString(prop->GetDefaultData(),str,type);
				sqlStr = sqlStr + " DEFAULT '"+str+"' with values";
			}
		}
		dprintf("AddProperty sqlStr:"+sqlStr+"\n");
		if(!mDbAccess.ExecuteSQL(sqlStr.GetString())){
			dprintf("dbAccess ExecuteSQL failed:",mDbAccess.GetLastError());
			dprintf("\n");
			return RESULT_FAIL;
		}
		return RESULT_OK;
	}else
		return RESULT_FAIL;
}

int DBBusiness::CreateTable(PropertySContainer* pkContainer)
{
	if(!pkContainer || !pkContainer->GetTableName() || !mDbAccess.IsConnect())
		return RESULT_FAIL;
	unsigned int i;
	VeStringA sqlStr;
	VeStringA tableName = pkContainer->GetTableName();
	VeStringA keyField = pkContainer->GetKeyField();
	VeStringA key = pkContainer->GetKey();

	sqlStr = "create table "+tableName+"(_id int identity(1,1) primary key not null";
	for(i=0;i<pkContainer->m_kDBChildren.Size();i++)
	{
		PropertySValue* prop = pkContainer->m_kDBChildren[i];
		VeStringA field = prop->GetField();
		int type = prop->GetType();
		sqlStr += ","+field+" ";
		if(type==TYPE_VeStringA)
		{
			char str[128]={0};
			sprintf(str,"%d",DB_VARCHAR_LENGTH);
			sqlStr += "varchar(";
			sqlStr += str;
			sqlStr += ")";
			if(prop->GetDefaultData())
			{
				VeStringA defaultData = TrimString((char*)prop->GetDefaultData());
				sqlStr += " DEFAULT '"+defaultData+"'";
			}
		}
		else
		{
			if(type==TYPE_VeInt64 || type==TYPE_VeUInt64)
				sqlStr += "bigint";
			else
				sqlStr += "int";
			if(prop->GetDefaultData())
			{
				char str[128]={0};
				DataToString(prop->GetDefaultData(),str,type);
				sqlStr = sqlStr + " DEFAULT '"+str+"'";
			}
		}	
	}
	sqlStr += ")";
	dprintf("CreateTable sqlStr:"+sqlStr+"\n");
	if(!mDbAccess.ExecuteSQL(sqlStr.GetString())){
		dprintf("dbAccess ExecuteSQL failed:",mDbAccess.GetLastError());
		dprintf("\n");
		return RESULT_FAIL;
	}else
		return RESULT_OK;
}

int DBBusiness::Load(PropertySContainer* pkContainer)
{
	if(!pkContainer || !pkContainer->GetTableName() || !mDbAccess.IsConnect()) 
		return RESULT_FAIL;

	unsigned int i;
	VeStringA sqlStr;
	VeStringA tableName = pkContainer->GetTableName();
	VeStringA keyField = pkContainer->GetKeyField();
	VeStringA key = pkContainer->GetKey();

	sqlStr = "select * from "+tableName;
	if(pkContainer->GetKeyField() && strlen(pkContainer->GetKeyField())>0 && pkContainer->GetKey() && strlen(pkContainer->GetKey())>0)
	{
		sqlStr += " where "+keyField+"='"+key+"'";
	}
	dprintf("Load sqlStr:"+sqlStr+"\n");

	if(mDbAccess.ExecuteSQL(sqlStr.GetString()))
	{ 
		//执行查询语句完毕，开始取数据
		if(mDbAccess.FetchFirst())
		{
			//记录获取失败的字段，在获取操作执行完后，一起新建这些字段
			int a = pkContainer->m_kDBChildren.Size();
			int* notFoundPropertyList = new int[pkContainer->m_kDBChildren.Size()];
			for(i=0;i<pkContainer->m_kDBChildren.Size();i++)
				notFoundPropertyList[i] = 0;

			for(i=0;i<pkContainer->m_kDBChildren.Size();i++)
			{
				PropertySValue* prop = pkContainer->m_kDBChildren[i];
				if(GetProperty(prop)==RESULT_FAIL)
					notFoundPropertyList[i] = 1;
			}
			for(i=0;i<pkContainer->m_kDBChildren.Size();i++)
			{
				PropertySValue* prop = pkContainer->m_kDBChildren[i];
				if(notFoundPropertyList[i]==1)
					AddProperty(prop,pkContainer);
			}
			delete[] notFoundPropertyList;
		}
		else
		{
			dprintf("dbAccess ExecuteSQL failed:%s",mDbAccess.GetLastError());
			dprintf("\n");
			if(Insert(pkContainer)==RESULT_OK)
				return RESULT_CREATE;
			else
				return RESULT_FAIL;
		}
	}
	else
	{
		dprintf("dbAccess ExecuteSQL failed:%s",mDbAccess.GetLastError());
		dprintf("\n");
		int flag = RESULT_FAIL;
		if(Insert(pkContainer)==RESULT_FAIL)
			flag = CreateTable(pkContainer);
		if(flag==RESULT_OK)
		{
			if(Insert(pkContainer)==RESULT_OK)
				return RESULT_CREATE;
			else
				return RESULT_FAIL;
		}else
			return RESULT_FAIL;
	}
	return RESULT_OK;
}

int DBBusiness::Update(PropertySContainer* pkContainer)
{
	if(!pkContainer || !pkContainer->GetTableName() || !mDbAccess.IsConnect() || pkContainer->m_kDBDirty.Size()<=0) 
		return RESULT_FAIL;

	unsigned int i=0;
	VeStringA sqlStr;
	VeStringA tableName = pkContainer->GetTableName();
	VeStringA keyField = pkContainer->GetKeyField();
	VeStringA key = pkContainer->GetKey();

	sqlStr = "update "+tableName+" set ";
	pkContainer->m_kDBDirty.BeginIterator();
	while(!pkContainer->m_kDBDirty.IsEnd())
	{
		PropertySValue* prop = pkContainer->m_kDBDirty.GetIterNode()->m_tContent;
		pkContainer->m_kDBDirty.Next();
		if(prop->GetField() && strlen(prop->GetField())>0 && prop->GetData())
		{
			if(i!=0)
				sqlStr = sqlStr+",";
			i++;
			if(prop->GetType()==TYPE_VeStringA)
			{
				sqlStr = sqlStr+prop->GetField()+"='"+TrimString((char*)prop->GetData())+"'";
			}
			else
			{
				char str[128]={0};
				DataToString(prop->GetData(),str,prop->GetType());
				sqlStr = sqlStr+prop->GetField()+"='"+str+"'";
			}
		}

	}
	if(pkContainer->GetKeyField() && strlen(pkContainer->GetKeyField())>0 && pkContainer->GetKey() && strlen(pkContainer->GetKey())>0)
	{
		sqlStr += " where "+keyField+"='"+key+"'";
	}
	dprintf("Update sqlStr:"+sqlStr+"\n");

	if(!mDbAccess.ExecuteSQL(sqlStr.GetString()))
	{
		dprintf("dbAccess ExecuteSQL failed:%s",mDbAccess.GetLastError());
		dprintf("\n");
		return RESULT_FAIL;
	}
	return RESULT_OK;
}

int DBBusiness::Insert(PropertySContainer* pkContainer)
{
	if(!pkContainer || !pkContainer->GetTableName() || !mDbAccess.IsConnect()) 
		return RESULT_FAIL;

	unsigned int i;
	VeStringA sqlStr;
	VeStringA tableName = pkContainer->GetTableName();
	VeStringA keyStr;
	VeStringA valueStr;
	VeStringA keyField = pkContainer->GetKeyField();
	VeStringA key = pkContainer->GetKey();

	if(pkContainer->GetKeyField() && strlen(pkContainer->GetKeyField())>0 && pkContainer->GetKey() && strlen(pkContainer->GetKey())>0)
	{
		keyStr = keyField;
		valueStr = "'"+key+"'";
	}
	for(i=0;i<pkContainer->m_kDBChildren.Size();i++)
	{
		PropertySValue* prop = pkContainer->m_kDBChildren[i];
		if(prop->GetField() && pkContainer->GetKeyField() && strcmp(prop->GetField(),pkContainer->GetKeyField())==0)
			continue;
		if(prop->GetField() && strlen(prop->GetField())>0 && prop->GetDefaultData())
		{
			if(!keyStr.IsEmpty())
			{
				keyStr = keyStr+",";
				valueStr = valueStr+",";
			}
			keyStr = keyStr+prop->GetField();
			if(prop->GetType()==TYPE_VeStringA)
			{
				valueStr = valueStr+"'"+TrimString((char*)prop->GetDefaultData())+"'";
			}
			else
			{
				char str[128]={0};
				DataToString(prop->GetDefaultData(),str,prop->GetType());
				valueStr = valueStr+"'"+str+"'";
			}
		}
	}
	sqlStr = "insert into "+tableName+"("+keyStr+") values("+valueStr+")";
	dprintf("Insert sqlStr:"+sqlStr+"\n");

	if(!mDbAccess.ExecuteSQL(sqlStr.GetString()))
	{
		dprintf("dbAccess ExecuteSQL failed:%s",mDbAccess.GetLastError());
		dprintf("\n");
		return RESULT_FAIL;
	}
	return RESULT_OK;
}

int DBBusiness::Delete(PropertySContainer* pkContainer)
{
	return RESULT_FAIL;
}

VeVector< VePair<VeString, VeInt32> > DBBusiness::checkPurchase(const char* uuid)
{
	VeStringA sqlStr;
	VeStringA uuidStr;
	VeVector< VePair<VeString, VeInt32> > ret;

	if(uuid==NULL)
		return ret;
	uuidStr = uuid;
	sqlStr = "select qudao,price from FeeInfo where state=1 and uuid='"+uuidStr+"'";
	dprintf("checkPurchase select sqlStr:"+sqlStr+"\n");

	if(mDbAccess.ExecuteSQL(sqlStr.GetString()))
	{ 
		//执行查询语句完毕，开始取数据
		bool flag = mDbAccess.FetchFirst();
		while(flag)
		{
			long nRetLen;
			VeInt32 lTestBigInt = -1;
			VeChar8 buffer[DB_VARCHAR_LENGTH]={0};
			if(!mDbAccess.GetData("qudao",buffer, sizeof(buffer), &nRetLen))
			{
				dprintf("获取字段信息失败：%s\n","qudao");
			}
			else
			{
				dprintf("获得字段信息：%s=%s\n","qudao",buffer);
			}
			if(!mDbAccess.GetData("price", &(lTestBigInt), sizeof(VeInt32), &nRetLen, SQL_C_SLONG))
			{
				dprintf("获取字段信息失败：%s\n","price");
			}  
			else
			{
				char str[128]={0};
				dprintf("获得字段信息：%s=%s\n","price",_itoa(lTestBigInt,str,10));
			}
			ret.PushBack(VePair<VeString, VeInt32>(buffer,lTestBigInt));
			flag = mDbAccess.FecthNext();
		}
		//更新记录
		if(ret.Size()>0)
		{
			sqlStr = "update FeeInfo set state=2 where state=1 and uuid='"+uuidStr+"'";
			dprintf("checkPurchase update sqlStr:"+sqlStr+"\n");
			if(!mDbAccess.ExecuteSQL(sqlStr.GetString()))
			{
				dprintf("dbAccess ExecuteSQL failed:%s",mDbAccess.GetLastError());
				dprintf("\n");
				ret.Clear();
			}
		}
	}
	else
	{
		dprintf("dbAccess ExecuteSQL failed:%s",mDbAccess.GetLastError());
		dprintf("\n");
	}
	return ret;
}