// SQLServer.h: interface for the CQuery class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERY_H__CAEAF203_40C0_4C32_BA76_9A4B0245984B__INCLUDED_)
#define AFX_QUERY_H__CAEAF203_40C0_4C32_BA76_9A4B0245984B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
 #include "windows.h"
#endif
//ODBC API
#include <sql.h>
#include <sqlext.h>
#include <odbcinst.h>

//--
#pragma comment(lib,"odbc32.lib")

//#pragma comment(lib,"OdbcCP32.Lib")

#ifdef WIN32
typedef __int64 _INT64;
#endif

#ifdef GCC
typedef long long int _INT64;
#endif

//--
/* SQLEndTran() options */
#define SQL_COMMIT          0 //�ύ
#define SQL_ROLLBACK        1 //�ع�
//
//////////////////////////////////////////////////////////////////////////
//������붨��
//������붨�����ERROR_����_������_�����ڱ��
//�ݶ���λ������
#define BASE_ERROR      100000

//���ݿ�ģ��������
#define DATABASE_BASE_ERROR    BASE_ERROR + 0


//CConnection���еĴ������
#define CON_BASE_ERROR   DATABASE_BASE_ERROR + 0

#define ERROR_CON_INIT_1  CON_BASE_ERROR + 1 
#define ERROR_CON_INIT_2  CON_BASE_ERROR + 2 
#define ERROR_CON_INIT_3  CON_BASE_ERROR + 3 
#define ERROR_CON_CONNECT_1  CON_BASE_ERROR + 4 
#define ERROR_CON_CONNECT_2  CON_BASE_ERROR + 5 
#define ERROR_CON_DISCONNECT_1 CON_BASE_ERROR + 6
#define ERROR_CON_DISCONNECT_2 CON_BASE_ERROR + 7
#define ERROR_CON_BEGINTRAN_1 CON_BASE_ERROR + 8
#define ERROR_CON_ENDTRAN_1  CON_BASE_ERROR + 9
#define ERROR_CON_ENDTRAN_2  CON_BASE_ERROR + 10
#define ERROR_CON_SETTIMEOUT_1 CON_BASE_ERROR + 11
#define ERROR_CON_SETTIMEOUT_2 CON_BASE_ERROR + 12
#define ERROR_CON_SETTIMEOUT_3 CON_BASE_ERROR + 13
#define ERROR_CON_SETTIMEOUT_4 CON_BASE_ERROR + 14
#define ERROR_CON_CONNECT_3  CON_BASE_ERROR + 15
#define ERROR_CON_DISCONNECT_3 CON_BASE_ERROR + 16
#define ERROR_CON_BEGINTRAN_2 CON_BASE_ERROR + 17
#define ERROR_CON_ENDTRAN_3  CON_BASE_ERROR + 18
#define ERROR_CON_SETTIMEOUT_5 CON_BASE_ERROR + 19
#define ERROR_CON_CONNECT_4  CON_BASE_ERROR + 20 
#define ERROR_CON_BEGINTRAN_3 CON_BASE_ERROR + 21
#define ERROR_CON_ENDTRAN_4  CON_BASE_ERROR + 22
#define ERROR_CON_ISCONNECT_1 CON_BASE_ERROR + 23
#define ERROR_CON_ISCONNECT_2 CON_BASE_ERROR + 24

//CQuery���еĴ������
#define QUERY_BASE_ERROR  DATABASE_BASE_ERROR + 100

#define ERROR_QUERY_INIT_1   QUERY_BASE_ERROR + 1
#define ERROR_QUERY_INIT_2   QUERY_BASE_ERROR + 2
#define ERROR_QUERY_INIT_3   QUERY_BASE_ERROR + 3
#define ERROR_QUERY_GETCOLCOUNT_1 QUERY_BASE_ERROR + 4
#define ERROR_QUERY_GETCOLCOUNT_2 QUERY_BASE_ERROR + 5
#define ERROR_QUERY_GETCROWCOUNT_1 QUERY_BASE_ERROR + 6
#define ERROR_QUERY_GETCROWCOUNT_2 QUERY_BASE_ERROR + 7
#define ERROR_QUERY_EXECSQL_1  QUERY_BASE_ERROR + 8
#define ERROR_QUERY_EXECSQL_2  QUERY_BASE_ERROR + 9
#define ERROR_QUERY_FETCH_1   QUERY_BASE_ERROR + 10
#define ERROR_QUERY_FETCH_2   QUERY_BASE_ERROR + 11
#define ERROR_QUERY_FETCHNEXT_1  QUERY_BASE_ERROR + 12
#define ERROR_QUERY_FETCHNEXT_2  QUERY_BASE_ERROR + 13
#define ERROR_QUERY_FETCHPRE_1  QUERY_BASE_ERROR + 14
#define ERROR_QUERY_FETCHPRE_2  QUERY_BASE_ERROR + 15
#define ERROR_QUERY_FETCHFIRST_1 QUERY_BASE_ERROR + 16
#define ERROR_QUERY_FETCHFIRST_2 QUERY_BASE_ERROR + 17
#define ERROR_QUERY_FETCHLAST_1  QUERY_BASE_ERROR + 18
#define ERROR_QUERY_FETCHLAST_2  QUERY_BASE_ERROR + 19
#define ERROR_QUERY_FETCHROW_1  QUERY_BASE_ERROR + 20
#define ERROR_QUERY_FETCHROW_2  QUERY_BASE_ERROR + 21
#define ERROR_QUERY_CANCEL_1  QUERY_BASE_ERROR + 22
#define ERROR_QUERY_CANCEL_2  QUERY_BASE_ERROR + 23
#define ERROR_QUERY_GETDATA_1  QUERY_BASE_ERROR + 24
#define ERROR_QUERY_GETDATA_2  QUERY_BASE_ERROR + 25
#define ERROR_QUERY_GETDATA_3  QUERY_BASE_ERROR + 26
#define ERROR_QUERY_GETDATA_4  QUERY_BASE_ERROR + 27
#define ERROR_QUERY_GETCOLBYNAME_1 QUERY_BASE_ERROR + 28
#define ERROR_QUERY_GETCOLNAME_1 QUERY_BASE_ERROR + 29
#define ERROR_QUERY_GETCOLNAME_2 QUERY_BASE_ERROR + 30
#define ERROR_QUERY_INIT_4   QUERY_BASE_ERROR + 31
#define ERROR_QUERY_GETCOLCOUNT_3 QUERY_BASE_ERROR + 32
#define ERROR_QUERY_GETCROWCOUNT_3 QUERY_BASE_ERROR + 33
#define ERROR_QUERY_EXECSQL_3  QUERY_BASE_ERROR + 34
#define ERROR_QUERY_FETCH_3   QUERY_BASE_ERROR + 35
#define ERROR_QUERY_FETCHNEXT_3  QUERY_BASE_ERROR + 36
#define ERROR_QUERY_FETCHPRE_3  QUERY_BASE_ERROR + 37
#define ERROR_QUERY_FETCHFIRST_3 QUERY_BASE_ERROR + 38
#define ERROR_QUERY_FETCHLAST_3  QUERY_BASE_ERROR + 39
#define ERROR_QUERY_FETCHROW_3  QUERY_BASE_ERROR + 40
#define ERROR_QUERY_CANCEL_3  QUERY_BASE_ERROR + 41
#define ERROR_QUERY_GETDATA_5  QUERY_BASE_ERROR + 42
#define ERROR_QUERY_GETCOLNAME_3 QUERY_BASE_ERROR + 43
#define ERROR_QUERY_INSERTARRAY_1 QUERY_BASE_ERROR + 44
#define ERROR_QUERY_INSERTARRAY_2 QUERY_BASE_ERROR + 45
#define ERROR_QUERY_INSERTARRAY_3 QUERY_BASE_ERROR + 46
#define ERROR_QUERY_INSERTARRAY_4 QUERY_BASE_ERROR + 47
#define ERROR_QUERY_INSERTARRAY_5 QUERY_BASE_ERROR + 48
#define ERROR_QUERY_INSERTARRAY_6 QUERY_BASE_ERROR + 49
#define ERROR_QUERY_INIT_5   QUERY_BASE_ERROR + 50
//CDBAccess���еĴ������
#define DBACCESS_BASE_ERROR  DATABASE_BASE_ERROR + 200

#define ERROR_DBACCESS_INIT_1   DBACCESS_BASE_ERROR + 1
#define ERROR_DBACCESS_INIT_2   DBACCESS_BASE_ERROR + 2
#define ERROR_DBACCESS_CONNECT_1  DBACCESS_BASE_ERROR + 3
#define ERROR_DBACCESS_CONNECT_2  DBACCESS_BASE_ERROR + 4
#define ERROR_DBACCESS_DISCONNECT_1  DBACCESS_BASE_ERROR + 5
#define ERROR_DBACCESS_DISCONNECT_2  DBACCESS_BASE_ERROR + 6
#define ERROR_DBACCESS_RECONNECT_1  DBACCESS_BASE_ERROR + 7
#define ERROR_DBACCESS_EXECUTESQL_1  DBACCESS_BASE_ERROR + 8
#define ERROR_DBACCESS_EXECUTESQL_2  DBACCESS_BASE_ERROR + 9
#define ERROR_DBACCESS_EXECUTESQL_3  DBACCESS_BASE_ERROR + 10
#define ERROR_DBACCESS_EXECUTESQL_4  DBACCESS_BASE_ERROR + 11
#define ERROR_DBACCESS_EXECUTESQL_5  DBACCESS_BASE_ERROR + 12
#define ERROR_DBACCESS_INIT_3   DBACCESS_BASE_ERROR + 13
#define ERROR_DBACCESS_INIT_4   DBACCESS_BASE_ERROR + 14
#define ERROR_DBACCESS_GETTASKDATA_1 DBACCESS_BASE_ERROR + 15
#define ERROR_DBACCESS_GETTASKDATA_2 DBACCESS_BASE_ERROR + 16
#define ERROR_DBACCESS_GETTASKDATA_3 DBACCESS_BASE_ERROR + 17
#define ERROR_DBACCESS_GETTASKDATA_4 DBACCESS_BASE_ERROR + 18
#define ERROR_DBACCESS_GETTASKDATA_5 DBACCESS_BASE_ERROR + 19
#define ERROR_DBACCESS_GETTASKDATA_6 DBACCESS_BASE_ERROR + 20
#define ERROR_DBACCESS_GETTASKDATA_7 DBACCESS_BASE_ERROR + 21
#define ERROR_DBACCESS_GETTASKDATA_8 DBACCESS_BASE_ERROR + 22
#define ERROR_DBACCESS_GETTASKDATA_9 DBACCESS_BASE_ERROR + 23
#define ERROR_DBACCESS_GETTASKDATA_10 DBACCESS_BASE_ERROR + 24
#define ERROR_DBACCESS_GETTASKDATA_11 DBACCESS_BASE_ERROR + 25
#define ERROR_DBACCESS_GETTASKDATA_12 DBACCESS_BASE_ERROR + 26
#define ERROR_DBACCESS_GETTASKDATA_13 DBACCESS_BASE_ERROR + 27

//////////////////////////////////////////////////////////////////////////

class CConnection
{
public:
 CConnection();
 virtual ~CConnection();
public:
 //�ύ������.����: SQL_COMMITΪ�ύ��SQL_ROLLBACKΪ�ع�.����ֵ:�Ƿ��ύ�ɹ�
 bool EndTran(short nAction);

 //��ʼ������.����ֵ:�Ƿ�ɹ�
 bool BeginTran();

 //�Ͽ�����. �Ͽ�����ǰ��Ҫȷ���Ƿ��ύ�����񡣷���ֵ:�Ƿ�ɹ�
 bool Disconnect();

 //���ӵ�DNS������:DNS�����û���������, ����ֵ:�Ƿ����ӳɹ�
 bool Connect(const char* pszDNS, const char* pszUser, const char* pszPwd);

 //ȡ������Ϣ,����ֵ:������Ϣ
 const char* GetLastError(){return (char*)m_pszLastError;}

 //�Ƿ����ӵ����ݿ�,����ֵ:�Ƿ����ӵ����ݿ�
 bool IsConnect();

 //���ó�ʱ�������ֱ�Ϊ���ӳ�ʱ����¼��ʱ.���Ϊ-1����ʾ�ò�����������,����ֵ:�Ƿ����óɹ�
 bool SetTimeOut(int nConnect = -1, int nLogin  = -1);

 //�Ƿ����ڽ���������,����ֵ:�Ƿ����������.
 bool IsInTran(){return m_bTraning;}

 //�����������ݿ�,����ֵ:�Ƿ������ɹ�
 bool ReConnect(); 

 //��ʼ��,����:���ӳ�ʱ����¼��ʱ,����ֵ:�Ƿ��ʼ���ɹ�.
 bool Init(int nConnectTimeOut = -1, int nLoginTimeOut = -1);

 
 //���Է����ΪCQuery��ʹ�á�
 operator SQLHANDLE(){return m_hCon;}

 
private:
 //���ô�����Ϣ,����:������Ϣ,�������
 void SetErrorInfo(const char* pszError, long lErrorCode); 

 //ȡ�ô�����Ϣ������:������ͣ�����ľ�����������,����ֵ:�Ƿ�ɹ�
 bool GetErrorInfo(SQLSMALLINT nHandleType, SQLHANDLE nHandle, long lErrorCode);

 //�Ƿ�ɹ�ִ���ˣ�����:��Ҫ�жϵķ���ֵ��������ͣ���Ҫ�жϵľ�����������,����ֵ:�Ƿ�ɹ�
 bool IsSuccess(SQLRETURN nRet, SQLSMALLINT nHandleType, SQLHANDLE nHandle, long lErrorCode);
public:

 SQLHANDLE m_hCon; //���ݿ����Ӿ�� 

protected:

 int m_nLoginTimeOut;
 int m_nConnectTimeOut;

 SQLHANDLE m_hEnv; //�������
 SQLRETURN m_nRet; //����ֵ

protected:
 bool m_bIsConnect; //�Ƿ��������ݿ�
 SQLCHAR m_pszLastError[SQL_MAX_MESSAGE_LENGTH+100]; //������Ϣ

 bool m_bTraning; //�������Ƿ������
 bool m_bInit; //��ʼ���Ƿ�����

 char m_pszDNS[255];   //ODBC DNS��
 char m_pszUser[255];  //ODBC �û���
 char m_pszPwd[255];   //ODBC �û�����
};

class CQuery 
{
public:
 CQuery(CConnection** ppDBCon, int nQueryTimeOut = 3);
 virtual ~CQuery();


 //ȡ�ü�¼��������.����ֵ:����
 unsigned short GetColumnCount();

 //ȡ��Ӱ������,����ֵ:Ӱ������
 long GetChangedRowCount(void);

 //ִ��ָ����sql���,����:Ҫִ�е�sql���,����ֵ:�Ƿ�ɹ�
 bool ExecuteSQL(const char* pszSQL);

 //��һ����¼,����ֵ:�Ƿ�ɹ�
 bool Fetch();

 //ǰһ����¼,����ֵ:�Ƿ�ɹ�
 bool FetchPrevious();

 //��һ����¼,����ֵ:�Ƿ�ɹ�
 bool FecthNext();

 //��AbsoluteΪtrue�ǣ�����nRowָ���ľ����У������ɵ�ǰλ�ù���������FetchOffsetָ��������У�nRow����0��ʾ��ǰ������nRowС��0��ʾ������
 bool FetchRow(unsigned int nRow, bool bAbsolute = true);

 //������һ��,����ֵ:�Ƿ�ɹ�
 bool FetchFirst();

 //�������һ��,����ֵ:�Ƿ�ɹ�
 bool FetchLast();

 //ȡ��,����ֵ:�Ƿ�ɹ�
 bool Cancel();

 //ȡ�õ�ǰ�еĵ�nColumn�е�ֵ�����������У����ջ����������ջ�������С������ֵ��С����������C�������͡�����ֵ:�Ƿ�ɹ�
 bool GetData(unsigned short nColumn, void* pBuffer,
     unsigned long nBufLen,
     long * nDataLen = NULL,
     int nType=SQL_C_DEFAULT);

 //ȡ�õ�ǰ�е�pszName�ֶε�ֵ�����������У����ջ����������ջ�������С������ֵ��С����������C�������͡�����ֵ:�Ƿ�ɹ�
 bool GetData(const char* pszName, void* pBuffer,
       unsigned long nBufLen,
       long * nDataLen = NULL,
       int nType=SQL_C_DEFAULT);

 //�ر����ӣ�����ִ��sql���ʱ�������ȶϿ����ӣ�Ȼ���ڷ��������У��������Ƿ����α�
 void Close();

 //ȡ������Ϣ.����ֵ:������Ϣ
 const char* GetLastError(){return (char*)m_pszLastError;}
protected:

 //��ʼ����������.����ֵ:�Ƿ�ɹ�
 bool Init();

 //ȡ��nColumn�е��ֶ���.����:����,�ֶ���,�ֶ�������.����ֵ:�Ƿ�ɹ�
 bool GetColumnName(unsigned short nColumn, char* pszName, short nNameLen);

 //ȡ���ֶ���ΪpszColumn���ڵ���,����:�ֶ���,����ֵ:�ֶ���������
 unsigned short GetColumnByName(const char* pszColumn);

 //���ô�����Ϣ������:������Ϣ���������
 void SetErrorInfo(const char* pszError, long lErrorCode);
 
 //ȡ�ô�����Ϣ������:������ͣ�����ľ�����������.����ֵ:�Ƿ�ɹ�
 bool GetErrorInfo(SQLSMALLINT nHandleType, SQLHANDLE nHandle, long lErrorCode);

 //�Ƿ�ɹ�ִ���ˣ�����:��Ҫ�жϵķ���ֵ��������ͣ� ��Ҫ�жϵľ�����������,����ֵ:�Ƿ�ɹ�
 bool IsSuccess(SQLRETURN nRet, SQLSMALLINT nHandleType, SQLHANDLE nHandle, long lErrorCode);

 //�Ƿ����.����ֵ:�Ƿ�ɹ�
 bool IsValid();

protected:
 SQLHSTMT m_hStmt; //STMT���
 SQLRETURN m_nRet; //����ֵ
 
private:
 CConnection** m_ppDBCon;
protected:
 SQLCHAR m_pszLastError[SQL_MAX_MESSAGE_LENGTH+100]; //������Ϣ
 int m_nQueryTimeOut;  //��ѯ��ʱʱ��
};


class CDBAccess 
{
public:
 //ִ��SQL���,����:Ҫִ�е�SQL���.����ֵ:�Ƿ�ִ�гɹ�
 bool ExecuteSQL(const char* pszSQL);

 //�رյ����ݿ������,��������ʹ��,�����ȵ���Init����.
 void Close();

 //�����������ݿ�,����ֵ:�Ƿ������ɹ�
 bool ReConnect();

 //�Ͽ������ݿ������.����ֵ:�Ƿ�Ͽ�����
 bool Disconnect();

 //���ӵ����ݿ�.����:DNS��,�û���,����.����ֵ:�Ƿ����ӵ����ݿ�.
 bool Connect(const char* pszDNS, const char* pszUser, const char* pszPwd);

 //��ʼ��,����:sql��䱣���ļ�,���ӳ�ʱ,��¼��ʱ.����ֵ:�Ƿ��ʼ���ɹ�.
 bool Init(const char* pszSaveSQLFile, CConnection** ppDBCon, int nConnectTimeOut = -1, int nLoginTimeOut = -1, int nQueryTimeOut = 3);

 //�Ƿ��Ѿ��������ݿ���,����ֵ,�Ƿ����ӵ����ݿ�
 bool IsConnect();

 //��ʼ������,����ֵ:�Ƿ�ɹ�
 bool BeginTran(){return (*m_ppDBCon)->BeginTran();}

 //�ύ������,����:�ύ����,����ֵ:�Ƿ�ɹ�
 bool EndTran(short nAction){return (*m_ppDBCon)->EndTran(nAction);}

 //add by stavck at 20051031
 //ȡ�õ�ǰ�е�cszName�ֶε�ֵ�����������У����ջ����������ջ�������С������ֵ��С����������C�������͡�����ֵ:�Ƿ�ɹ�
 bool GetData(const char* pszName, void* pBuffer, unsigned long nBufLen
  , long * nDataLen = NULL, int nType=SQL_C_DEFAULT)
 {return m_pQuery->GetData(pszName, pBuffer, nBufLen, nDataLen, nType);}

 //��һ����¼,����ֵ:�Ƿ�ɹ�
 bool Fetch(){return m_pQuery->Fetch();}
 //end add
 //������һ��,����ֵ:�Ƿ�ɹ�
 bool FetchFirst(){return m_pQuery->FetchFirst();}

 //ǰһ����¼,����ֵ:�Ƿ�ɹ�
 bool FetchPrevious(){return m_pQuery->FetchPrevious();}

 //��һ����¼,����ֵ:�Ƿ�ɹ�
 bool FecthNext(){return m_pQuery->FecthNext();}

 //��AbsoluteΪtrue�ǣ�����nRowָ���ľ����У������ɵ�ǰλ�ù���������FetchOffsetָ��������У�nRow����0��ʾ��ǰ������nRowС��0��ʾ������
 bool FetchRow(unsigned int nRow, bool bAbsolute = true)
  {return m_pQuery->FetchRow(nRow, bAbsolute);}

 //�������һ��,����ֵ:�Ƿ�ɹ�
 bool FetchLast(){return m_pQuery->FetchLast();} 

 //ȡ������Ϣ.����ֵ:������Ϣ
 const char* GetLastError(){return (char*)m_pszLastError;}
 
 CDBAccess();
 virtual ~CDBAccess();
 
 //add at 20051124
 //�ر����������Ϊ�˳�ʱ�رջ��������ÿ�����ݿ�������Ҫʹ�øú������ر�
 void CloseStmt()
 {	
	 if(m_pQuery) 
		m_pQuery->Close();
 }
protected: 
 int m_nLoginTimeOut;  //��¼��ʱʱ��
 int m_nConnectTimeOut;  //���ӳ�ʱʱ��
 int m_nQueryTimeOut;  //��ѯ��ʱʱ��

 char m_pszLastError[255]; //���������Ĵ�����Ϣ
 CConnection **m_ppDBCon;  //���ݿ�����

 CQuery * m_pQuery;   //��ѯ��������ʱʹ��,��¼��

protected:

 //���ô�����Ϣ,����:������Ϣ,���������λ��,�Ƿ���ӵ���־
 void SetErrorInfo(const char *pszError, long lErrorCode); 

protected:

 bool m_bCreateCon; //�Ƿ����Լ����������ݿ�����
 bool m_bEnd; //��ѯ��������ʱʹ��,�Ƿ������һ����¼��.
};

#endif // !defined(AFX_QUERY_H__CAEAF203_40C0_4C32_BA76_9A4B0245984B__INCLUDED_)