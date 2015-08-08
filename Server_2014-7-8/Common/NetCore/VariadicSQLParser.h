#pragma once

#include "DS_List.h"

#include <stdarg.h>

namespace VariadicSQLParser
{
	struct IndexAndType
	{
		unsigned int strIndex;
		unsigned int typeMappingIndex;
	};
	const char* GetTypeMappingAtIndex(int i);
	void GetTypeMappingIndices( const char *format, DataStructures::List<IndexAndType> &indices );

	void ExtractArguments( va_list argptr, const DataStructures::List<IndexAndType> &indices, char ***argumentBinary, int **argumentLengths );
	void FreeArguments(const DataStructures::List<IndexAndType> &indices, char **argumentBinary, int *argumentLengths);
}
