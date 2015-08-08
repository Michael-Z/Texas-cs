////////////////////////////////////////////////////////////////////////////
//
//  Venus Engine Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:   VeDirectory.h
//  Version:     v1.00
//  Created:     6/10/2012 by Napoleon
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////

#pragma once

class VE_POWER_API VeDirectory : public VeMemObject
{
public:
	enum Type
	{
		TYPE_FILE,
		TYPE_ZIP,
		TYPE_ASSET,
		TYPE_URL,
		TYPE_NUM
	};

	enum AccessMask
	{
		ACCESS_F_OK = 0x0,
		ACCESS_W_OK = 0x2,
		ACCESS_R_OK = 0x4
	};

	enum WriteType
	{
		WRITE_CREATE_COVER,
		WRITE_CREATE_APPEND
	};

	typedef VeAbstractDelegate<const VeMemoryIStreamPtr&> ReadDelegate;

	typedef VeFunctionDelegate<const VeMemoryIStreamPtr&> ReadFuncDelegate;
	
	typedef VeEvent<const VeMemoryIStreamPtr&> ReadEvent;

	typedef VeAbstractDelegate<VE_RESULT> WriteDelegate;

	typedef VeFunctionDelegate<VE_RESULT> WriteFuncDelegate;

	typedef VeEvent<VE_RESULT> WriteEvent;

	VeDirectory(const VeChar8* pcName);

	virtual ~VeDirectory();

	const VeChar8* GetName();

	virtual bool Access(VeUInt32 u32Flag) const = 0;

	virtual bool HasFile(const VeChar8* pcFileName) const = 0;

	virtual bool HasDirectory(const VeChar8* pcDirName) const = 0;

	virtual bool Change(const VeChar8* pcDirName, bool bCreate = true) = 0;

	virtual Type GetType() const = 0;

	virtual VeBinaryIStreamPtr OpenSync(const VeChar8* pcFileName) const = 0;

	virtual void ReadAsync(const VeChar8* pcFileName, ReadDelegate& kDelegate) const = 0;

	virtual void WriteAsync(const VeChar8* pcFileName, const VeMemoryOStreamPtr& spContent, WriteDelegate& kDelegate, WriteType eType = WRITE_CREATE_COVER) const = 0;

protected:
	VeStringA m_kName;

};
