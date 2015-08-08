#pragma once

#include "VEMemoryOverride.h"
#include "DS_Table.h"
#include "Export.h"

namespace VENet
{
	class BitStream;
}

namespace VENet
{

class VE_DLL_EXPORT TableSerializer
{
public:
	static void SerializeTable(DataStructures::Table *in, VENet::BitStream *out);
	static bool DeserializeTable(unsigned char *serializedTable, unsigned int dataLength, DataStructures::Table *out);
	static bool DeserializeTable(VENet::BitStream *in, DataStructures::Table *out);
	static void SerializeColumns(DataStructures::Table *in, VENet::BitStream *out);
	static void SerializeColumns(DataStructures::Table *in, VENet::BitStream *out, DataStructures::List<int> &skipColumnIndices);
	static bool DeserializeColumns(VENet::BitStream *in, DataStructures::Table *out);	
	static void SerializeRow(DataStructures::Table::Row *in, unsigned keyIn, const DataStructures::List<DataStructures::Table::ColumnDescriptor> &columns, VENet::BitStream *out);
	static void SerializeRow(DataStructures::Table::Row *in, unsigned keyIn, const DataStructures::List<DataStructures::Table::ColumnDescriptor> &columns, VENet::BitStream *out, DataStructures::List<int> &skipColumnIndices);
	static bool DeserializeRow(VENet::BitStream *in, DataStructures::Table *out);
	static void SerializeCell(VENet::BitStream *out, DataStructures::Table::Cell *cell, DataStructures::Table::ColumnType columnType);
	static bool DeserializeCell(VENet::BitStream *in, DataStructures::Table::Cell *cell, DataStructures::Table::ColumnType columnType);
	static void SerializeFilterQuery(VENet::BitStream *in, DataStructures::Table::FilterQuery *query);
	static bool DeserializeFilterQuery(VENet::BitStream *out, DataStructures::Table::FilterQuery *query);
	static void SerializeFilterQueryList(VENet::BitStream *in, DataStructures::Table::FilterQuery *query, unsigned int numQueries, unsigned int maxQueries);
	static bool DeserializeFilterQueryList(VENet::BitStream *out, DataStructures::Table::FilterQuery **query, unsigned int *numQueries, unsigned int maxQueries, int allocateExtraQueries=0);
	static void DeallocateQueryList(DataStructures::Table::FilterQuery *query, unsigned int numQueries);
};

}