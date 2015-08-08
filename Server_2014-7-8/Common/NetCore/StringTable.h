#pragma once

#include "DS_OrderedList.h"
#include "Export.h"
#include "VEMemoryOverride.h"

namespace VENet
{
	class BitStream;
};

typedef unsigned char StringTableType;

struct StrAndBool
{
	char *str;
	bool b;
};

namespace VENet
{
	int VE_DLL_EXPORT StrAndBoolComp( char *const &key, const StrAndBool &data );

	class VE_DLL_EXPORT StringTable
	{
	public:

		~StringTable();

		static StringTable* Instance(void);

		void AddString(const char *str, bool copyString);

		void EncodeString( const char *input, int maxCharsToWrite, VENet::BitStream *output );

		bool DecodeString( char *output, int maxCharsToWrite, VENet::BitStream *input );

		static void AddReference(void);

		static void RemoveReference(void);

		StringTable();

	protected:
		void LogStringNotFound(const char *strName);

		static StringTable *instance;
		static int referenceCount;

		DataStructures::OrderedList<char *, StrAndBool, StrAndBoolComp> orderedStringList;
	};
}