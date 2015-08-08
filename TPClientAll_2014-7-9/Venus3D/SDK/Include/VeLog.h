////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeLog.h
//  Version:     v1.00
//  Created:     9/8/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

enum VeLogType
{
	VE_LOG_TYPE_DEBUG,
	VE_LOG_TYPE_INFO,
	VE_LOG_TYPE_WARN,
	VE_LOG_TYPE_ERROR,
	VE_LOG_TYPE_MAX
};

#define VE_LOG_BUFFER_SIZE (1024)

VeSmartPointer(VeLogger);

class VE_POWER_API VeLogger : public VeRefObject
{
public:
	enum LogOutput
	{
		OUTPUT_CONSOLE,
		OUTPUT_DEBUGOUTPUT,
		OUTPUT_MAX
	};

	virtual ~VeLogger() {}

	virtual void AppendLog(VeLogType eType, const VeChar8* pcTag, const VeChar8* pcFormat, va_list kArgs) = 0;

	static void AppendLog(VeLogType eType, const VeChar8* pcTag, const VeChar8* pcFormat, ...);

	static void SetLog(LogOutput eOutput);

	static void SetLog(const VeLoggerPtr& spLog);
};

#ifdef VE_DEBUG
#	define VE_LOG_D(tag,format,...) VeLogger::AppendLog(VE_LOG_TYPE_DEBUG,tag,format,__VA_ARGS__)
#	define VE_LOG_DString(tag,format) VeLogger::AppendLog(VE_LOG_TYPE_DEBUG,tag,format)
#	define VeDebugOutputString(format) VeLogger::AppendLog(VE_LOG_TYPE_DEBUG,"Venus3D",format)
#else
#	define VE_LOG_D(tag,format,...)
#	define VE_LOG_DString(tag,format)
#	define VeDebugOutputString(format)
#endif

#define VE_LOG_IString(tag,format) VeLogger::AppendLog(VE_LOG_TYPE_INFO,tag,format)
#define VE_LOG_WString(tag,format) VeLogger::AppendLog(VE_LOG_TYPE_WARN,tag,format)
#define VE_LOG_EString(tag,format) VeLogger::AppendLog(VE_LOG_TYPE_ERROR,tag,format)

#define VeDebugOutput(format,...) VE_LOG_D("Venus3D",format,__VA_ARGS__)

#define VE_LOG_I(tag,format,...) VeLogger::AppendLog(VE_LOG_TYPE_INFO,tag,format,__VA_ARGS__)
#define VE_LOG_W(tag,format,...) VeLogger::AppendLog(VE_LOG_TYPE_WARN,tag,format,__VA_ARGS__)
#define VE_LOG_E(tag,format,...) VeLogger::AppendLog(VE_LOG_TYPE_ERROR,tag,format,__VA_ARGS__)
