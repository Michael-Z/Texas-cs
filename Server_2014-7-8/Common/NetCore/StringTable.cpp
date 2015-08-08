#include "StringTable.h"
#include <string.h>
#include "VEAssert.h"
#include <stdio.h>
#include "BitStream.h"
#include "StringCompressor.h"
using namespace VENet;

StringTable* StringTable::instance=0;
int StringTable::referenceCount=0;


int VENet::StrAndBoolComp( char *const &key, const StrAndBool &data )
{
	return strcmp(key,(const char*)data.str);
}

StringTable::StringTable()
{
	
}

StringTable::~StringTable()
{
	unsigned i;
	for (i=0; i < orderedStringList.Size(); i++)
	{
		if (orderedStringList[i].b)
			veFree_Ex(orderedStringList[i].str, _FILE_AND_LINE_ );
	}
}

void StringTable::AddReference(void)
{
	if (++referenceCount==1)
	{
		instance = VENet::OP_NEW<StringTable>( _FILE_AND_LINE_ );
	}
}
void StringTable::RemoveReference(void)
{
	VEAssert(referenceCount > 0);

	if (referenceCount > 0)
	{
		if (--referenceCount==0)
		{
			VENet::OP_DELETE(instance, _FILE_AND_LINE_);
			instance=0;
		}
	}
}

StringTable* StringTable::Instance(void)
{
	return instance;
}

void StringTable::AddString(const char *str, bool copyString)
{
	StrAndBool sab;
	sab.b=copyString;
	if (copyString)
	{
		sab.str = (char*) rakMalloc_Ex( strlen(str)+1, _FILE_AND_LINE_ );
		strcpy(sab.str, str);
	}
	else
	{
		sab.str=(char*)str;
	}

	orderedStringList.Insert(sab.str,sab, true, _FILE_AND_LINE_);

	VEAssert(orderedStringList.Size() < (StringTableType)-1);	
	
}
void StringTable::EncodeString( const char *input, int maxCharsToWrite, VENet::BitStream *output )
{
	unsigned index;
	bool objectExists;
	index=orderedStringList.GetIndexFromKey((char*)input, &objectExists);
	if (objectExists)
	{
		output->Write(true);
		output->Write((StringTableType)index);
	}
	else
	{
		LogStringNotFound(input);
		output->Write(false);
		StringCompressor::Instance()->EncodeString(input, maxCharsToWrite, output);
	}
}

bool StringTable::DecodeString( char *output, int maxCharsToWrite, VENet::BitStream *input )
{
	bool hasIndex=false;
	VEAssert(maxCharsToWrite>0);

	if (maxCharsToWrite==0)
		return false;
	if (!input->Read(hasIndex))
		return false;
	if (hasIndex==false)
	{
		StringCompressor::Instance()->DecodeString(output, maxCharsToWrite, input);
	}
	else
	{
		StringTableType index;
		if (!input->Read(index))
			return false;
		if (index >= orderedStringList.Size())
		{
#ifdef _DEBUG
			VEAssert(0);
#endif
			return false;
		}
		
		strncpy(output, orderedStringList[index].str, maxCharsToWrite);
		output[maxCharsToWrite-1]=0;
	}

	return true;
}
void StringTable::LogStringNotFound(const char *strName)
{
	(void) strName;

#ifdef _DEBUG
	VENET_DEBUG_PRINTF("Efficiency Warning! Unregistered String %s sent to StringTable.\n", strName);
#endif
}
