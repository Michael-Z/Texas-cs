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
#define SQL_COMMIT          0 //提交
#define SQL_ROLLBACK        1 //回滚
//
//////////////////////////////////////////////////////////////////////////
//错误代码定义
//错误代码定义规则：ERROR_类名_函数名_函数内编号
//暂定四位错误编号
#define BASE_ERROR      100000

//数据库模块错误代码
#define DATABASE_BASE_ERROR    BASE_ERROR + 0


//CConnection类中的错误代码
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

//CQuery类中的错误代码
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
//CDBAccess类中的错误代码
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
 //提交事务处理.参数: SQL_COMMIT为提交，SQL_ROLLBACK为回滚.返回值:是否提交成功
 bool EndTran(short nAction);

 //开始事务处理.返回值:是否成功
 bool BeginTran();

 //断开连接. 断开连接前，要确定是否提交了事务。返回值:是否成功
 bool Disconnect();

 //连接到DNS，参数:DNS名，用户名，口令, 返回值:是否连接成功
 bool Connect(const char* pszDNS, const char* pszUser, const char* pszPwd);

 //取错误信息,返回值:错误信息
 const char* GetLastError(){return (char*)m_pszLastError;}

 //是否连接到数据库,返回值:是否连接到数据库
 bool IsConnect();

 //设置超时，参数分别为连接超时，登录超时.如果为-1，表示该参数不起作用,返回值:是否设置成功
 bool SetTimeOut(int nConnect = -1, int nLogin  = -1);

 //是否正在进行事务处理,返回值:是否进行事务处理.
 bool IsInTran(){return m_bTraning;}

 //重新连接数据库,返回值:是否重连成功
 bool ReConnect(); 

 //初始化,参数:连接超时，登录超时,返回值:是否初始化成功.
 bool Init(int nConnectTimeOut = -1, int nLoginTimeOut = -1);

 
 //可以方便的为CQuery类使用。
 operator SQLHANDLE(){return m_hCon;}

 
private:
 //设置错误信息,参数:错误信息,错误代码
 void SetErrorInfo(const char* pszError, long lErrorCode); 

 //取得错误信息，参数:句柄类型，出错的句柄，错误代码,返回值:是否成功
 bool GetErrorInfo(SQLSMALLINT nHandleType, SQLHANDLE nHandle, long lErrorCode);

 //是否成功执行了，参数:需要判断的返回值，句柄类型，需要判断的句柄，错误代码,返回值:是否成功
 bool IsSuccess(SQLRETURN nRet, SQLSMALLINT nHandleType, SQLHANDLE nHandle, long lErrorCode);
public:

 SQLHANDLE m_hCon; //数据库连接句柄 

protected:

 int m_nLoginTimeOut;
 int m_nConnectTimeOut;

 SQLHANDLE m_hEnv; //环境句柄
 SQLRETURN m_nRet; //返回值

protected:
 bool m_bIsConnect; //是否连接数据库
 SQLCHAR m_pszLastError[SQL_MAX_MESSAGE_LENGTH+100]; //错误信息

 bool m_bTraning; //事务处理是否进行中
 bool m_bInit; //初始化是否正常

 char m_pszDNS[255];   //ODBC DNS名
 char m_pszUser[255];  //ODBC 用户名
 char m_pszPwd[255];   //ODBC 用户口令
};

class CQuery 
{
public:
 CQuery(CConnection** ppDBCon, int nQueryTimeOut = 3);
 virtual ~CQuery();


 //取得记录集的列数.返回值:列数
 unsigned short GetColumnCount();

 //取得影响行数,返回值:影响行数
 long GetChangedRowCount(void);

 //执行指定的sql语句,参数:要执行的sql语句,返回值:是否成功
 bool ExecuteSQL(const char* pszSQL);

 //下一个记录,返回值:是否成功
 bool Fetch();

 //前一个记录,返回值:是否成功
 bool FetchPrevious();

 //下一个记录,返回值:是否成功
 bool FecthNext();

 //当Absolute为true是，跳到nRow指定的绝对行，否则，由当前位置滚动到参数FetchOffset指定的相对行，nRow大于0表示向前滚动，nRow小于0表示向后滚动
 bool FetchRow(unsigned int nRow, bool bAbsolute = true);

 //跳到第一行,返回值:是否成功
 bool FetchFirst();

 //跳到最后一行,返回值:是否成功
 bool FetchLast();

 //取消,返回值:是否成功
 bool Cancel();

 //取得当前行的第nColumn列的值。参数：哪列，接收缓冲区，接收缓冲区大小，返回值大小，缓冲区的C语言类型。返回值:是否成功
 bool GetData(unsigned short nColumn, void* pBuffer,
     unsigned long nBufLen,
     long * nDataLen = NULL,
     int nType=SQL_C_DEFAULT);

 //取得当前行的pszName字段的值。参数：哪列，接收缓冲区，接收缓冲区大小，返回值大小，缓冲区的C语言类型。返回值:是否成功
 bool GetData(const char* pszName, void* pBuffer,
       unsigned long nBufLen,
       long * nDataLen = NULL,
       int nType=SQL_C_DEFAULT);

 //关闭连接，重新执行sql语句时，必须先断开连接，然后在分配句柄才行，否则会包非法的游标
 void Close();

 //取错误信息.返回值:错误信息
 const char* GetLastError(){return (char*)m_pszLastError;}
protected:

 //初始化，分配句柄.返回值:是否成功
 bool Init();

 //取得nColumn列的字段名.参数:哪列,字段名,字段名长度.返回值:是否成功
 bool GetColumnName(unsigned short nColumn, char* pszName, short nNameLen);

 //取得字段名为pszColumn所在的列,参数:字段名,返回值:字段名所在列
 unsigned short GetColumnByName(const char* pszColumn);

 //设置错误信息，参数:错误信息，错误代码
 void SetErrorInfo(const char* pszError, long lErrorCode);
 
 //取得错误信息，参数:句柄类型，出错的句柄，错误代码.返回值:是否成功
 bool GetErrorInfo(SQLSMALLINT nHandleType, SQLHANDLE nHandle, long lErrorCode);

 //是否成功执行了，参数:需要判断的返回值，句柄类型， 需要判断的句柄，错误代码,返回值:是否成功
 bool IsSuccess(SQLRETURN nRet, SQLSMALLINT nHandleType, SQLHANDLE nHandle, long lErrorCode);

 //是否可用.返回值:是否成功
 bool IsValid();

protected:
 SQLHSTMT m_hStmt; //STMT句柄
 SQLRETURN m_nRet; //返回值
 
private:
 CConnection** m_ppDBCon;
protected:
 SQLCHAR m_pszLastError[SQL_MAX_MESSAGE_LENGTH+100]; //错误信息
 int m_nQueryTimeOut;  //查询超时时间
};


class CDBAccess 
{
public:
 //执行SQL语句,参数:要执行的SQL语句.返回值:是否执行成功
 bool ExecuteSQL(const char* pszSQL);

 //关闭到数据库的连接,如果想继续使用,必须先调用Init函数.
 void Close();

 //重新连接数据库,返回值:是否重连成功
 bool ReConnect();

 //断开到数据库的连接.返回值:是否断开连接
 bool Disconnect();

 //连接到数据库.参数:DNS名,用户名,口令.返回值:是否连接到数据库.
 bool Connect(const char* pszDNS, const char* pszUser, const char* pszPwd);

 //初始化,参数:sql语句保存文件,连接超时,登录超时.返回值:是否初始化成功.
 bool Init(const char* pszSaveSQLFile, CConnection** ppDBCon, int nConnectTimeOut = -1, int nLoginTimeOut = -1, int nQueryTimeOut = 3);

 //是否已经连到数据库了,返回值,是否连接到数据库
 bool IsConnect();

 //开始事务处理,返回值:是否成功
 bool BeginTran(){return (*m_ppDBCon)->BeginTran();}

 //提交事务处理,参数:提交类型,返回值:是否成功
 bool EndTran(short nAction){return (*m_ppDBCon)->EndTran(nAction);}

 //add by stavck at 20051031
 //取得当前行的cszName字段的值。参数：哪列，接收缓冲区，接收缓冲区大小，返回值大小，缓冲区的C语言类型。返回值:是否成功
 bool GetData(const char* pszName, void* pBuffer, unsigned long nBufLen
  , long * nDataLen = NULL, int nType=SQL_C_DEFAULT)
 {return m_pQuery->GetData(pszName, pBuffer, nBufLen, nDataLen, nType);}

 //下一个记录,返回值:是否成功
 bool Fetch(){return m_pQuery->Fetch();}
 //end add
 //跳到第一行,返回值:是否成功
 bool FetchFirst(){return m_pQuery->FetchFirst();}

 //前一个记录,返回值:是否成功
 bool FetchPrevious(){return m_pQuery->FetchPrevious();}

 //下一个记录,返回值:是否成功
 bool FecthNext(){return m_pQuery->FecthNext();}

 //当Absolute为true是，跳到nRow指定的绝对行，否则，由当前位置滚动到参数FetchOffset指定的相对行，nRow大于0表示向前滚动，nRow小于0表示向后滚动
 bool FetchRow(unsigned int nRow, bool bAbsolute = true)
  {return m_pQuery->FetchRow(nRow, bAbsolute);}

 //跳到最后一行,返回值:是否成功
 bool FetchLast(){return m_pQuery->FetchLast();} 

 //取错误信息.返回值:错误信息
 const char* GetLastError(){return (char*)m_pszLastError;}
 
 CDBAccess();
 virtual ~CDBAccess();
 
 //add at 20051124
 //关闭语句句柄，因为退出时关闭会出错。所以每次数据库操作完毕要使用该函数来关闭
 void CloseStmt()
 {	
	 if(m_pQuery) 
		m_pQuery->Close();
 }
protected: 
 int m_nLoginTimeOut;  //登录超时时间
 int m_nConnectTimeOut;  //连接超时时间
 int m_nQueryTimeOut;  //查询超时时间

 char m_pszLastError[255]; //最后误操作的错误信息
 CConnection **m_ppDBCon;  //数据库连接

 CQuery * m_pQuery;   //查询测试任务时使用,记录集

protected:

 //设置错误信息,参数:错误信息,发生错误的位置,是否添加到日志
 void SetErrorInfo(const char *pszError, long lErrorCode); 

protected:

 bool m_bCreateCon; //是否是自己创建的数据库连接
 bool m_bEnd; //查询测试任务时使用,是否到了最后一条记录了.
};

#endif // !defined(AFX_QUERY_H__CAEAF203_40C0_4C32_BA76_9A4B0245984B__INCLUDED_)