// Query.cpp: implementation of the CQuery class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SQLServerInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConnection::CConnection()
{
 memset(m_pszLastError, 0x00, SQL_MAX_MESSAGE_LENGTH+100);
 memset(m_pszDNS, 0x00, sizeof(m_pszDNS));
 memset(m_pszUser, 0x00, sizeof(m_pszUser));
 memset(m_pszPwd, 0x00, sizeof(m_pszPwd));

 m_bIsConnect = false;

 m_hCon = INVALID_HANDLE_VALUE;
 m_hEnv = INVALID_HANDLE_VALUE;

 m_bTraning = false;
 m_nRet = SQL_SUCCESS;
 m_bInit = false;
}

CConnection::~CConnection()
{
 try
 {
  if(m_bTraning)
  {//如果没有手工断掉连接，这里会进行回滚
   EndTran(SQL_ROLLBACK);
  }

  if(IsConnect())
  {//如果连接还没有断开，那么这里断开。
   Disconnect();
  }

  if(m_hEnv != INVALID_HANDLE_VALUE)
  {
   SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
   m_hEnv = INVALID_HANDLE_VALUE;
  }

  if(m_hCon != INVALID_HANDLE_VALUE)
  {
   SQLFreeHandle(SQL_HANDLE_DBC, m_hCon);
   m_hCon = INVALID_HANDLE_VALUE;
  }
 }
 catch (...)
 {
  //
 } 
}


bool CConnection::GetErrorInfo(SQLSMALLINT nHandleType, SQLHANDLE nHandle, long lErrorCode)
{//还不完整，要进一步写一下，否则，只能取到最后一行的错误信息,不过大部分情况时够用了.
 bool bConnInd = nHandleType == SQL_HANDLE_DBC ? true :false;
 SQLRETURN nRet = SQL_SUCCESS;

 SQLCHAR pszSqlState[SQL_MAX_MESSAGE_LENGTH] = "";
    SQLCHAR pszErrorMsg[SQL_MAX_MESSAGE_LENGTH] = "";

 
 SQLINTEGER nNativeError = 0L;
 SQLSMALLINT nErrorMsg = 0;
 SQLSMALLINT nRecNmbr = 1;
 
 //执行错误时ODBC返回的是一个错误信息的结果集，需要遍历结果集合中所有行

 memset(pszSqlState, 0x00, sizeof(pszSqlState));
 memset(pszErrorMsg, 0x00, sizeof(pszErrorMsg));

 nRet = SQLGetDiagRec(nHandleType, nHandle,
  nRecNmbr, pszSqlState, &nNativeError,
  pszErrorMsg, SQL_MAX_MESSAGE_LENGTH - 1,
  &nErrorMsg);


 SetErrorInfo((char*)pszErrorMsg, lErrorCode);

 return true;
}

bool CConnection::IsSuccess(SQLRETURN nRet, SQLSMALLINT nHandleType,
       SQLHANDLE nHandle, long lErrorCode)
{
 if(nRet == SQL_SUCCESS)
 {
  return true;
 }
 else if(nRet == SQL_SUCCESS_WITH_INFO)
 {
  return true;
 }
 else
 {
  GetErrorInfo(nHandleType, nHandle, lErrorCode);
  return false;
 }

 return false; 
}

bool CConnection::Init(int nConnectTimeOut /* = -1 */, int nLoginTimeOut /* = -1 */)
{
 m_nRet = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);

 if(IsSuccess(m_nRet, SQL_HANDLE_ENV, m_hEnv, ERROR_CON_INIT_1))
 {
  //将ODBC设置成为版本3，否则某些ODBC API 函数不能被支持。
  m_nRet = SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION,
       (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_UINTEGER);

  if(!IsSuccess(m_nRet, SQL_HANDLE_ENV, m_hEnv, ERROR_CON_INIT_2))
  {
   //系统不支持ODBC3
   return false;
  }

  m_nRet = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hCon);

  if(!IsSuccess(m_nRet, SQL_HANDLE_DBC, m_hCon, ERROR_CON_INIT_3))
  {
   //分配连接句柄不成功！
   return false;
  }

 }
 else
 {
  //分配环境句柄不成功！
  return false;
 }
 
 m_nConnectTimeOut = nConnectTimeOut;
 m_nLoginTimeOut = nLoginTimeOut;

 

 m_bInit = true;
 SetTimeOut(nConnectTimeOut, nLoginTimeOut);

 return true;
}

bool CConnection::Connect(const char *pszDNS, const char *pszUser, const char *pszPwd)
{
 if(!m_bInit)
 {//如果没有初始化,是不能连接的!
  SetErrorInfo("没有初始化,不能连接到数据库！", ERROR_CON_CONNECT_4);
  return false;
 }

 if(pszDNS == NULL)
 {//DNS怎么也不能为空的啊
  SetErrorInfo("DNS名为空,无法连接数据库！", ERROR_CON_CONNECT_1);
  return false;
 }
 else
 {
  strncpy((char*)m_pszDNS, pszDNS, sizeof(m_pszDNS));
 }

 if(pszUser != NULL)
 {
  strncpy((char*)m_pszUser, pszUser, sizeof(m_pszUser));
 }
 else
 {
  m_pszUser[0] = NULL;
 }

 if(pszPwd != NULL)
 {
  strncpy((char*)m_pszPwd, pszPwd, sizeof(m_pszPwd));
 }
 else
 {
  m_pszPwd[0] = NULL;
 }

 try
 {
  //开始连接
  m_nRet = SQLConnect(m_hCon,
       (SQLCHAR*)pszDNS, SQL_NTS,
       (SQLCHAR*)pszUser, SQL_NTS,
       (SQLCHAR*)pszPwd, SQL_NTS);
 }
 catch (...)
 {
  SetErrorInfo("连接数据库时发生错误！", ERROR_CON_CONNECT_3);
  return false;
 }

 m_bIsConnect = IsSuccess(m_nRet, SQL_HANDLE_DBC, m_hCon, ERROR_CON_CONNECT_2);
 
 return m_bIsConnect;
}

void CConnection::SetErrorInfo(const char *pszError, long lErrorCode)
{//先把错误代码填到错误信息中,接着填具体的错误信息
 memset(m_pszLastError, 0x00, sizeof(m_pszLastError));
 char pszErrorCode[20] = "";
 
 //先设置错误代码
 sprintf((char*)pszErrorCode, "EC[%ld]", lErrorCode);
 int nLen = strlen((char*)pszErrorCode);
 
 strncpy((char*)m_pszLastError, (const char*)pszErrorCode, nLen);
 //不能超过了最大长度,要控制一下
 size_t nMaxSize = sizeof(m_pszLastError)-1-nLen;

 strncat((char*)m_pszLastError, pszError, nMaxSize>strlen(pszError)?strlen(pszError):nMaxSize);

}

bool CConnection::Disconnect()
{
 if(!IsConnect())
 {//如果没有连接,只是简单的返回
  return true;
 }
 
 if(m_bTraning)
 {//事务处理还没有提交呢!
  SetErrorInfo("必须先提交事务，才能断开！", ERROR_CON_DISCONNECT_1);
  return false;
 }

 try
 { 
  //断开连接
  m_nRet = SQLDisconnect(m_hCon);
  if(m_hEnv != INVALID_HANDLE_VALUE)
  {
   SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
   m_hEnv = INVALID_HANDLE_VALUE;
  }

  if(m_hCon != INVALID_HANDLE_VALUE)
  {
   SQLFreeHandle(SQL_HANDLE_DBC, m_hCon);
   m_hCon = INVALID_HANDLE_VALUE;
  }  
  m_bInit = false;

 }
 catch (...)
 {
  SetErrorInfo("断开数据库连接时发生错误！", ERROR_CON_DISCONNECT_3);
  m_bIsConnect = false;

  return false;
 }
 
 //判断是否成功断开
 m_bIsConnect = !IsSuccess(m_nRet, SQL_HANDLE_DBC, m_hCon, ERROR_CON_DISCONNECT_2);
 
 return !m_bIsConnect;
}

bool CConnection::BeginTran()
{
 if(!IsConnect())
 {//如果没有连接,怎么开始事务处理!
  SetErrorInfo("没有连接,怎么开始事务处理?", ERROR_CON_BEGINTRAN_3);
  return false;
 }

 try
 { 
  //这里是设置手动提交.
  m_nRet= SQLSetConnectAttr(m_hCon,
       SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF,
       SQL_IS_POINTER);
 }
 catch (...)
 {
  SetErrorInfo("开始事务处理时发生错误！", ERROR_CON_BEGINTRAN_2);
  return false;
 }
 //判断是否成功
 m_bTraning = IsSuccess(m_nRet, SQL_HANDLE_DBC, m_hCon, ERROR_CON_BEGINTRAN_1);

 return m_bTraning;
}

bool CConnection::EndTran(short nAction)
{
 if(!IsConnect() || !m_bTraning)
 {
  SetErrorInfo("没有连接,或者还没有开始事务处理!无法提交事务", ERROR_CON_ENDTRAN_4);
  return true;
 }

 if(nAction != SQL_COMMIT)
 {//如果不是提交,都当回滚处理
  nAction = SQL_ROLLBACK;
 }

 try
 { 
  //先结束事务处理
  m_nRet = SQLEndTran(SQL_HANDLE_DBC, m_hCon, nAction);
  //判断是否成功
  m_bTraning = !IsSuccess(m_nRet, SQL_HANDLE_DBC, m_hCon, ERROR_CON_ENDTRAN_1);
  //再改成自动提交.
  m_nRet= SQLSetConnectAttr(m_hCon,
       SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON,
       SQL_IS_POINTER);
 }
 catch (...)
 {
  SetErrorInfo("结束事务处理时发生错误！", ERROR_CON_ENDTRAN_3);
  return false;
 }
 
 //修改成功了没
 IsSuccess(m_nRet, SQL_HANDLE_DBC, m_hCon, ERROR_CON_ENDTRAN_2); 

 return !m_bTraning; 
}

bool CConnection::SetTimeOut(int nConnect, int nLogin)
{
 bool bRet = false;

 if(!m_bInit)
 {//如果没有初始化,不能使用该函数!
  SetErrorInfo("请在初始化后使用该函数", ERROR_CON_SETTIMEOUT_1);
  return bRet;
 }

 try
 { 
  if(nLogin >= 0)
  {//如果是负数,就不用管啦!
   if(m_bIsConnect)
   {//连接后,不能设置登录超时了.
    SetErrorInfo("设置登录超时必须在连接前使用该函数", ERROR_CON_SETTIMEOUT_2);
   }
   else
   {
    m_nRet = SQLSetConnectAttr(m_hCon, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER)nLogin, SQL_IS_INTEGER);
    bRet = IsSuccess(m_nRet, SQL_HANDLE_DBC, m_hCon, ERROR_CON_SETTIMEOUT_3);
   }
  }
  
  if(nConnect >= 0)
  {//如果是负数,就不用管啦!
   m_nRet = SQLSetConnectAttr(m_hCon, SQL_ATTR_CONNECTION_TIMEOUT, (SQLPOINTER)nConnect, SQL_IS_INTEGER);
   bRet = IsSuccess(m_nRet, SQL_HANDLE_DBC, m_hCon, ERROR_CON_SETTIMEOUT_4) || bRet;
  }
  else
  {
   return bRet;
  }
 }
 catch (...)
 {
  SetErrorInfo("设置系统超时时发生错误！", ERROR_CON_SETTIMEOUT_5);
  return false;
 }

 return bRet;
}


bool CConnection::ReConnect()
{
 try
 {
  //断开连接
  SQLDisconnect(m_hCon);
  m_bIsConnect = false;
  
  if(m_hEnv != INVALID_HANDLE_VALUE)
  {
   SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
   m_hEnv = INVALID_HANDLE_VALUE;
  }

  if(m_hCon != INVALID_HANDLE_VALUE)
  {
   SQLFreeHandle(SQL_HANDLE_DBC, m_hCon);
   m_hCon = INVALID_HANDLE_VALUE;
  }  
  m_bInit = false;

  if(!Init(m_nConnectTimeOut, m_nLoginTimeOut))
  {
   return false;
  }

  m_bIsConnect = false;
 }
 catch (...)
 {
  m_bIsConnect = false;
  m_bInit = false;
  return false;
 }

 return Connect(m_pszDNS, m_pszUser, m_pszPwd);
}

//add by stavck at 20051116
bool CConnection::IsConnect()
{ 
 //如果连接已经手工断开了，这里就没有必要再检查了。
 if(!m_bIsConnect)
 {
  return false;
 }
 else
 {
  return true;
 }
 //下面代码会在数据库重连后产生错误，删除。

 SQLINTEGER lRet = SQL_CD_TRUE;
 SQLINTEGER lRetSize = 0;
 try
 {
  //判断连接是否活着，不过断开时一定要等到对数据库有SQL请求后才有可以。
  m_nRet = SQLGetConnectAttr(m_hCon, SQL_ATTR_CONNECTION_DEAD, (SQLPOINTER)&lRet, SQL_IS_INTEGER, &lRetSize);

  if(!IsSuccess(m_nRet, SQL_HANDLE_DBC, m_hCon, ERROR_CON_ISCONNECT_1))
  {
   return false;
  }

 }
 catch (...)
 {
  SetErrorInfo("查询系统连接时发生错误！", ERROR_CON_ISCONNECT_2);
  return false; 
 }
 
 //连接仍然是活动的
 if(lRet == SQL_CD_FALSE)
 {
  return true;
 }
 
 //连接已经断掉了
 return false;
}
//////////////////////////////////////////////////////////////////////////

CQuery::CQuery(CConnection** ppDBCon, int nQueryTimeOut /* = 3 */)
{
 memset(m_pszLastError, 0x00, SQL_MAX_MESSAGE_LENGTH+100);
 m_nQueryTimeOut = 3;

 m_ppDBCon = ppDBCon; 

 m_nRet = SQL_SUCCESS;
 m_hStmt = INVALID_HANDLE_VALUE; 

 m_nQueryTimeOut = nQueryTimeOut;
}

CQuery::~CQuery()
{
 Close();
}


bool CQuery::Init()
{
 try
 { 
  if (m_ppDBCon == NULL || *m_ppDBCon == NULL ||!(*m_ppDBCon)->IsConnect())
  {
   return false;
  }
  
  //分配SQL语句句柄
  m_nRet = SQLAllocHandle( SQL_HANDLE_STMT,
   (*m_ppDBCon)->m_hCon, &m_hStmt );

  if(!IsSuccess(m_nRet, SQL_HANDLE_DBC,
   (*m_ppDBCon)->m_hCon, ERROR_QUERY_INIT_1))
  {
   m_hStmt = INVALID_HANDLE_VALUE; 
   return false;
  } 

  //指定要使用的游标并发级别
  m_nRet = SQLSetStmtAttr(m_hStmt, SQL_ATTR_CONCURRENCY,
         (SQLPOINTER) SQL_CONCUR_ROWVER, 0);
  if(!IsSuccess(m_nRet, SQL_HANDLE_STMT, m_hStmt, ERROR_QUERY_INIT_2))
  {
   Close(); 
   return false;
  }
  
  //设置光标类型为键集光标,
  //键集光标能够检测到行的删除和修改，但是无法检测到检测到行的添加和结果集顺序变化。
  //因为在光标创建时就创建了整个结果集，结果集合中记录和顺序已经被固定，
  //这一点和静态光标一样。所以键集光标可以说是一种介于静态光标和动态光标之间的光标类型。
  m_nRet = SQLSetStmtAttr(m_hStmt, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)
        SQL_CURSOR_DYNAMIC /* SQL_CURSOR_KEYSET_DRIVEN*/, 0);
  if(!IsSuccess(m_nRet, SQL_HANDLE_STMT, m_hStmt, ERROR_QUERY_INIT_3))
  {
   Close(); 
   return false;
  }

  m_nRet = SQLSetStmtAttr(m_hStmt, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)m_nQueryTimeOut, SQL_IS_UINTEGER);
  if(!IsSuccess(m_nRet, SQL_HANDLE_STMT, m_hStmt, ERROR_QUERY_INIT_5))
  {
   Close(); 
   return false;
  }
/*#ifdef _DEBUG
  SQLINTEGER dummy;
  m_nRet = SQLGetStmtAttr(m_hStmt, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)&lTimeOut, SQL_IS_UINTEGER, &dummy);
  if(IsSuccess(m_nRet, SQL_HANDLE_STMT, m_hStmt, ERROR_QUERY_INIT_5))
  {
   printf("query time out is: %ld\n\r", lTimeOut);
  }
  
#endif*/
 }
 catch (...)
 {
  SetErrorInfo("初始化时发生错误！", ERROR_QUERY_INIT_4);
  Close();
  return false;
 }

 return true;
}

void CQuery::Close()
{
 try
 {
  if(m_hStmt != INVALID_HANDLE_VALUE)
  {
   //释放句柄
   //SQLFreeStmt(m_hStmt, SQL_DROP);
   SQLFreeHandle(SQL_HANDLE_STMT,m_hStmt);
   m_hStmt = INVALID_HANDLE_VALUE; 

  }
 }
 catch (...)
 {
  m_hStmt = INVALID_HANDLE_VALUE;
 }
}


bool CQuery::GetErrorInfo(SQLSMALLINT nHandleType, SQLHANDLE nHandle,
         long lErrorCode)
{//还不完整，要进一步写一下，否则，只能取到最后一行的错误信息。
 bool bConnInd = nHandleType == SQL_HANDLE_DBC ? true :false;
 SQLRETURN nRet = SQL_SUCCESS;

 SQLCHAR pszSqlState[SQL_MAX_MESSAGE_LENGTH] = "";
    SQLCHAR pszErrorMsg[SQL_MAX_MESSAGE_LENGTH] = "";

 SQLINTEGER nNativeError = 0L;
 SQLSMALLINT nErrorMsg = 0;
 SQLSMALLINT nRecNmbr = 1;
 
 //执行错误时ODBC返回的是一个错误信息的结果集，需要遍历结果集合中所有行

 memset(pszSqlState, 0x00, sizeof(pszSqlState));
 memset(pszErrorMsg, 0x00, sizeof(pszErrorMsg));
 
 nRet = SQLGetDiagRec(nHandleType, nHandle,
  nRecNmbr, pszSqlState, &nNativeError,
  pszErrorMsg, SQL_MAX_MESSAGE_LENGTH - 1,
  &nErrorMsg);


 SetErrorInfo((char*)pszErrorMsg, lErrorCode);

 return true;
}

bool CQuery::IsSuccess(SQLRETURN nRet, SQLSMALLINT nHandleType,
         SQLHANDLE nHandle, long lErrorCode)
{
 if(nRet == SQL_SUCCESS)
 {
  return true;
 }
 else if(nRet == SQL_SUCCESS_WITH_INFO)
 {//表明执行成功但是带有一定错误信息,此时不应该记录到log中,否则log会与日俱增
  //GetErrorInfo(nHandleType, nHandle, lErrorCode, false);
  return true;
 }
 else
 {
  GetErrorInfo(nHandleType, nHandle, lErrorCode);
  return false;
 }

 return false; 
}

void CQuery::SetErrorInfo(const char *pszError, long lErrorCode)
{//先把错误代码填到错误信息中,接着填具体的错误信息
 memset(m_pszLastError, 0x00, sizeof(m_pszLastError));
 char pszErrorCode[20] = "";
 //先设置错误代码
 sprintf((char*)pszErrorCode, "EC[%ld]", lErrorCode);
 int nLen = strlen((char*)pszErrorCode);
 strncpy((char*)m_pszLastError, (const char*)pszErrorCode, nLen);
 //不能超过了最大长度,要控制一下
 size_t nMaxSize = sizeof(m_pszLastError)-1-nLen;

 strncat((char*)m_pszLastError, pszError,
  nMaxSize>strlen(pszError)?strlen(pszError):nMaxSize);

}

bool CQuery::IsValid()
{
 return m_hStmt != INVALID_HANDLE_VALUE;
}

unsigned short CQuery::GetColumnCount()
{
 if(!IsValid())
 {
  SetErrorInfo("STMT句柄不可用！", ERROR_QUERY_GETCOLCOUNT_1);
  return 0;
 }
 
 short nCols=0;

 try
 {
  if(!IsSuccess(m_nRet = SQLNumResultCols(m_hStmt, &nCols),
   SQL_HANDLE_STMT, m_hStmt, ERROR_QUERY_GETCOLCOUNT_2))
  {
   //如果不成功,返回0个
     return 0;
  }
 }
 catch (...)
 {
  SetErrorInfo("取得列个数时发生错误！", ERROR_QUERY_GETCOLCOUNT_3);
  Close();
  return 0;
 }

 return nCols; 
}

long CQuery::GetChangedRowCount()
{//对select语句是无效的,请选择使用
 if(!IsValid())
 {
  SetErrorInfo("STMT句柄不可用！", ERROR_QUERY_GETCROWCOUNT_1);
  return 0;
 }
 
 long nRows=0;
 
 try
 {
  if(!IsSuccess(m_nRet = SQLRowCount(m_hStmt,&nRows),
   SQL_HANDLE_STMT, m_hStmt, ERROR_QUERY_GETCROWCOUNT_2))
  {
   return 0;
  }
 }
 catch (...)
 {
  SetErrorInfo("取得影响记录集的个数数时发生错误！", ERROR_QUERY_GETCROWCOUNT_3);
  Close();
  return 0;
 }

 return nRows; 
}

bool CQuery::ExecuteSQL(const char* pszSQL)
{

 //因为一个语句句柄只能执行一次sql语句,所有,得先释放才能执行
 if(IsValid())
 {//如果有效,先关闭
  Close();
 }

 if(!Init())
 {//再初始化
  return false;
 }

 if(!IsValid())
 {//这时不能用?
  SetErrorInfo("STMT句柄不可用！", ERROR_QUERY_EXECSQL_1);
  return false;
 }

 try
 {
  //执行相应的sql语句
  m_nRet = SQLExecDirect(m_hStmt, (SQLTCHAR *)pszSQL, SQL_NTS);
 }
 catch (...)
 {
  SetErrorInfo("执行SQL语句出错!", ERROR_QUERY_EXECSQL_3);
  Close();
  return false;
 }
 
 return IsSuccess(m_nRet, SQL_HANDLE_STMT, m_hStmt, ERROR_QUERY_EXECSQL_2); 
}

bool CQuery::Fetch()
{
 if(!IsValid())
 {
  SetErrorInfo("STMT句柄不可用！", ERROR_QUERY_FETCH_1);
  return false;
 }

 try
 {
  m_nRet = SQLFetch(m_hStmt);
 }
 catch (...)
 {
  SetErrorInfo("Fetch出错!", ERROR_QUERY_FETCH_3);
  Close();
  return false;
 } 

 return IsSuccess(m_nRet, SQL_HANDLE_STMT, m_hStmt, ERROR_QUERY_FETCH_2);
}

bool CQuery::FecthNext()
{
 if(!IsValid())
 {
  SetErrorInfo("STMT句柄不可用！", ERROR_QUERY_FETCHNEXT_1);
  return false;
 }
 
 try
 {
  m_nRet = SQLFetchScroll(m_hStmt, SQL_FETCH_NEXT, 0);
 }
 catch (...)
 {
  SetErrorInfo("FecthNext出错!", ERROR_QUERY_FETCHNEXT_3);
  Close();
  return false; 
 } 
 
 return IsSuccess(m_nRet, SQL_HANDLE_STMT, m_hStmt, ERROR_QUERY_FETCHNEXT_2); 
}

bool CQuery::FetchPrevious()
{
 if(!IsValid())
 {
  SetErrorInfo("STMT句柄不可用！", ERROR_QUERY_FETCHPRE_1);
  return false;
 }

 try
 {
  m_nRet = SQLFetchScroll(m_hStmt, SQL_FETCH_PRIOR, 0);
 }
 catch (...)
 {
  SetErrorInfo("FetchPrevious出错!", ERROR_QUERY_FETCHPRE_3);
  Close();
  return false;
 } 

 return IsSuccess(m_nRet, SQL_HANDLE_STMT, m_hStmt, ERROR_QUERY_FETCHPRE_2); 
}

bool CQuery::FetchFirst()
{
 if(!IsValid())
 {
  SetErrorInfo("STMT句柄不可用！", ERROR_QUERY_FETCHFIRST_1);
  return false;
 }

 try
 {
  m_nRet = SQLFetchScroll(m_hStmt, SQL_FETCH_FIRST, 0);
 }
 catch (...)
 {
  SetErrorInfo("FetchFirst出错!", ERROR_QUERY_FETCHFIRST_3);
  Close();
  return false;
 } 
 
 return IsSuccess(m_nRet, SQL_HANDLE_STMT, m_hStmt, ERROR_QUERY_FETCHFIRST_2); 
}

bool CQuery::FetchLast()
{
 if(!IsValid())
 {
  SetErrorInfo("STMT句柄不可用！", ERROR_QUERY_FETCHLAST_1);
  return false;
 }

 try
 {
  m_nRet = SQLFetchScroll(m_hStmt,SQL_FETCH_LAST,0);
 }
 catch (...)
 {
  SetErrorInfo("FetchLast出错!", ERROR_QUERY_FETCHLAST_3);
  Close();
  return false;
 } 

 return IsSuccess(m_nRet, SQL_HANDLE_STMT, m_hStmt, ERROR_QUERY_FETCHLAST_2); 
}

bool CQuery::FetchRow(unsigned int nRow, bool bAbsolute /* = true */)
{
 if(!IsValid())
 {
  SetErrorInfo("STMT句柄不可用！", ERROR_QUERY_FETCHROW_1);
  return false;
 }

 try
 {
  m_nRet = SQLFetchScroll(m_hStmt,
       (bAbsolute ? SQL_FETCH_ABSOLUTE : SQL_FETCH_RELATIVE),
       nRow);
 }
 catch (...)
 {
  SetErrorInfo("FetchRow出错!", ERROR_QUERY_FETCHROW_3);
  Close();
  return false;
 }
 
 return IsSuccess(m_nRet, SQL_HANDLE_STMT, m_hStmt, ERROR_QUERY_FETCHROW_2);
}

bool CQuery::Cancel()
{
 if(!IsValid())
 {
  SetErrorInfo("STMT句柄不可用！", ERROR_QUERY_CANCEL_1);
  return true;
 }

 try
 {
  m_nRet = SQLCancel(m_hStmt);
 }
 catch (...)
 {
  SetErrorInfo("Cancel出错!", ERROR_QUERY_CANCEL_3);
  Close();
  return false;
 }

 return IsSuccess(m_nRet, SQL_HANDLE_STMT, m_hStmt, ERROR_QUERY_CANCEL_2);
}

bool CQuery::GetData(unsigned short nColumn, void* pBuffer,
       unsigned long nBufLen,
       long * nDataLen /* = NULL */,
       int nType/* =SQL_C_DEFAULT */)
{
 if(nColumn <= 0 || nColumn > GetColumnCount() || pBuffer == NULL)
 {
  SetErrorInfo("列范围不对，或者pBuffer为空！", ERROR_QUERY_GETDATA_1);
  return false;
 }

 if(!IsValid())
 {
  SetErrorInfo("STMT句柄不可用！", ERROR_QUERY_GETDATA_2);
  return true;
 }

 SQLINTEGER nOutLen = 0;
 
 try
 {
  m_nRet = SQLGetData(m_hStmt, nColumn, nType, pBuffer, nBufLen, &nOutLen);
 }
 catch (...)
 {
  SetErrorInfo("GetData出错!", ERROR_QUERY_GETDATA_5);
  Close();
  return false;
 }

 if(!IsSuccess(m_nRet, SQL_HANDLE_STMT, m_hStmt, ERROR_QUERY_GETDATA_3))
 {
  return false;
 }

 if(nDataLen)
 {
  *nDataLen=nOutLen;
 }

 return true; 
}

bool CQuery::GetData(const char* pszName, void* pBuffer,
        unsigned long nBufLen,
       long * nDataLen /* = NULL */,
       int nType/* =SQL_C_DEFAULT */)
{
 if(pszName == NULL || pBuffer == NULL)
 {
  SetErrorInfo("PszName or pBuffer 不能为空！", ERROR_QUERY_GETDATA_4);
  return false;
 }

 unsigned short nColumn = GetColumnByName(pszName);
 //有效性判断让GetData自己判断
 return GetData(nColumn, pBuffer, nBufLen, nDataLen, nType);
}

unsigned short CQuery::GetColumnByName(const char *pszColumn)
{
 if(!IsValid())
 {
  SetErrorInfo("STMT句柄不可用！", ERROR_QUERY_GETCOLBYNAME_1);
  return true;
 }

 unsigned short nCols = GetColumnCount();
 
 for(unsigned short i = 1; i < (nCols+1) ; i++)
 {//依次得到每个列的字段名，然后比较。
  TCHAR pszName[256] = "";

  if(GetColumnName(i, pszName, sizeof(pszName)))
  {
   if(_stricmp(pszName, pszColumn) == 0)
   {
    return i;
   }
  }
 }

 return 0;
}

bool CQuery::GetColumnName(unsigned short nColumn, char *pszName, short nNameLen)
{
 if(!IsValid())
 {
  SetErrorInfo("STMT句柄不可用！", ERROR_QUERY_GETCOLNAME_1);
  return true;
 }

 int nType = SQL_C_DEFAULT;
 SQLSMALLINT nSwCol=0, nSwType=0, nSwScale=0, nSwNull=0; //这些数据，在这个函数中，我们不用关心
 SQLUINTEGER pcbColDef=0;

 try
 {
  m_nRet = SQLDescribeCol( m_hStmt, nColumn, 
       (SQLTCHAR*)pszName, nNameLen,
       &nSwCol, &nSwType, &pcbColDef,
       &nSwScale, &nSwNull);
 }
 catch (...)
 {
  SetErrorInfo("取字段名称时出错!", ERROR_QUERY_GETCOLNAME_3);
  Close();
  return false;
 }
 
 return  IsSuccess(m_nRet, SQL_HANDLE_STMT, m_hStmt, ERROR_QUERY_GETCOLNAME_2);
}

//////////////////////////////////////////////////////////////////////////

CDBAccess::CDBAccess()
{
 m_ppDBCon = NULL;
 memset(m_pszLastError, 0x00, sizeof(m_pszLastError));

 m_nConnectTimeOut = -1;
 m_nLoginTimeOut = -1;
 m_pQuery = NULL;
 m_bEnd = false;
 m_bCreateCon = false;
}

CDBAccess::~CDBAccess()
{
 if(m_pQuery != NULL)
 {
  delete m_pQuery;
  m_pQuery = NULL;
 } 
}

void CDBAccess::SetErrorInfo(const char *pszError, long lErrorCode)
{//先把错误代码填到错误信息中,接着填具体的错误信息
 memset(m_pszLastError, 0x00, sizeof(m_pszLastError));
 char pszErrorCode[20] = "";
 //先设置错误代码
 sprintf((char*)pszErrorCode, "EC[%ld]", lErrorCode);
 int nLen = strlen((char*)pszErrorCode);
 strncpy((char*)m_pszLastError, (const char*)pszErrorCode, nLen);
 //不能超过了最大长度,要控制一下
 size_t nMaxSize = sizeof(m_pszLastError)-1-nLen;

 strncat((char*)m_pszLastError, pszError, nMaxSize>strlen(pszError)?strlen(pszError):nMaxSize);

}

bool CDBAccess::Init(const char* pszSaveSQLFile, CConnection** ppDBCon,
      int nConnectTimeOut /* = -1 */, int nLoginTimeOut /* = -1 */,
      int nQueryTimeOut /* = 3 */)
{
 m_nLoginTimeOut = nLoginTimeOut;
 m_nConnectTimeOut = nConnectTimeOut;
 m_nQueryTimeOut = nQueryTimeOut;
 
 m_ppDBCon = ppDBCon;
 

 if((*ppDBCon) != NULL)
 {
  return true;
 }
 
 try
 {
  (*m_ppDBCon) = new CConnection;

  if ((*m_ppDBCon) == NULL)
  {
   SetErrorInfo("没有足够的内存!", ERROR_DBACCESS_INIT_1);
   return false;
  }

  m_bCreateCon = true;

  if(!(*m_ppDBCon)->Init(nConnectTimeOut, nLoginTimeOut))
  {
   delete (*m_ppDBCon);
   (*m_ppDBCon) = NULL;
   
   SetErrorInfo("数据库连接初始化失败!", ERROR_DBACCESS_INIT_2);
 
   m_bCreateCon = false;

   return false;
  }

 }
 catch (...)
 {
  SetErrorInfo("初始化时出错!", ERROR_DBACCESS_INIT_3);
  return false; 
 }

 m_pQuery = new CQuery(m_ppDBCon, m_nQueryTimeOut);
 
 if(m_pQuery == NULL)
 {
  SetErrorInfo("没有足够的内存!", ERROR_DBACCESS_INIT_4);
  return false;
 }

 return true;
}

bool CDBAccess::Disconnect()
{
 if((*m_ppDBCon) == NULL)
 {
  SetErrorInfo("重新连接数据库时出错!,请稍后再试!", ERROR_DBACCESS_DISCONNECT_1);
  return false;
 }

 //注意,这里试着回滚了,请确保事务已经被提交.
 if((*m_ppDBCon)->IsInTran())
 {
  (*m_ppDBCon)->EndTran(SQL_ROLLBACK);
 }

 return (*m_ppDBCon)->Disconnect();
}


bool CDBAccess::Connect(const char *pszDNS, const char *pszUser, const char *pszPwd)
{
 if((*m_ppDBCon) == NULL)
 {
  SetErrorInfo("还没有初始化,请重新初始化后再试!", ERROR_DBACCESS_CONNECT_1);
  return false;
 }
 
 if(pszDNS == NULL)
 {
  SetErrorInfo("DNS不能为空!", ERROR_DBACCESS_CONNECT_2);
  return false;
 }

 return (*m_ppDBCon)->Connect(pszDNS, pszUser, pszPwd);

}


bool CDBAccess::ReConnect()
{
 //add at 20051124 for 关闭前，先试着关闭语句句柄，以免数据库关闭后，语句句柄无效！
 m_pQuery->Close();

 return (*m_ppDBCon)->ReConnect();
}

bool CDBAccess::IsConnect()
{
 if((*m_ppDBCon) == NULL)
 {
  return false;
 }
 else
 {
  return (*m_ppDBCon)->IsConnect();
 }
}

void CDBAccess::Close()
{
 try
 { 
  if(m_pQuery != NULL)
  {
   delete m_pQuery;
   m_pQuery = NULL;
  } 
 
  if(m_bCreateCon)
  {
   if((*m_ppDBCon) != NULL)
   {
    //强制断开,很霸道的,一定要保证所有的事务都提交在断开!
    delete (*m_ppDBCon);
    (*m_ppDBCon) = NULL;
   }
   m_bCreateCon = false;
  }

 }
 catch (...)
 {
  //
 }
}


bool CDBAccess::ExecuteSQL(const char *pszSQL)
{

 if(!IsConnect())
 {
  SetErrorInfo("没有连接到数据库,无法执行SQL语句!", ERROR_DBACCESS_EXECUTESQL_1);
  return false;
 }

 bool bRet = false;

 try
 { if(m_pQuery == NULL)
  {
   m_pQuery = new CQuery((m_ppDBCon), m_nQueryTimeOut);

   if(m_pQuery == NULL)
   {
    SetErrorInfo("没有足够的内存!", ERROR_DBACCESS_EXECUTESQL_2);
    return false;
   }
  }

  if (!m_pQuery->ExecuteSQL(pszSQL))
  {
   SetErrorInfo(m_pQuery->GetLastError(), ERROR_DBACCESS_EXECUTESQL_3);
   bRet = false;
  }
  else
  {
   bRet = true;
  }

 }
 catch (...)
 {
  SetErrorInfo("执行指定的SQL语句时出错!", ERROR_DBACCESS_EXECUTESQL_5);

  return false; 
 }

 return bRet;
}