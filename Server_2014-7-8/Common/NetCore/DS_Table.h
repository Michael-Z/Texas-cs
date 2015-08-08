////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DS_Table.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#ifdef _MSC_VER
#pragma warning( push )
#endif

#include "DS_List.h"
#include "DS_BPlusTree.h"
#include "VEMemoryOverride.h"
#include "Export.h"
#include "VEString.h"

#define _TABLE_BPLUS_TREE_ORDER 16
#define _TABLE_MAX_COLUMN_NAME_LENGTH 64

namespace DataStructures
{
class VE_DLL_EXPORT Table
{
public:

    enum ColumnType
    {
        NUMERIC,

        STRING,

        BINARY,

        POINTER,
    };


    struct VE_DLL_EXPORT Cell
    {
        Cell();
        ~Cell();
        Cell(double numericValue, char *charValue, void *ptr, ColumnType type);
        void SetByType(double numericValue, char *charValue, void *ptr, ColumnType type);
        void Clear(void);

        void Set(int input);
        void Set(unsigned int input);
        void Set(double input);

        void Set(const char *input);

        void Set(const char *input, int inputLength);

        void SetPtr(void* p);

        void Get(int *output);
        void Get(double *output);

        void Get(char *output);

        void Get(char *output, int *outputLength);

        VENet::VEString ToString(ColumnType columnType);
        Cell& operator = ( const Cell& input );
        Cell( const Cell & input);

        ColumnType EstimateColumnType(void) const;

        bool isEmpty;
        double i;
        char *c;
        void *ptr;
    };

    struct VE_DLL_EXPORT ColumnDescriptor
    {
        ColumnDescriptor();
        ~ColumnDescriptor();
        ColumnDescriptor(const char cn[_TABLE_MAX_COLUMN_NAME_LENGTH],ColumnType ct);

        char columnName[_TABLE_MAX_COLUMN_NAME_LENGTH];
        ColumnType columnType;
    };

    struct VE_DLL_EXPORT Row
    {
        DataStructures::List<Cell*> cells;

        void UpdateCell(unsigned columnIndex, double value);

        void UpdateCell(unsigned columnIndex, const char *str);

        void UpdateCell(unsigned columnIndex, int byteLength, const char *data);
    };

    enum FilterQueryType
    {
        QF_EQUAL,
        QF_NOT_EQUAL,
        QF_GREATER_THAN,
        QF_GREATER_THAN_EQ,
        QF_LESS_THAN,
        QF_LESS_THAN_EQ,
        QF_IS_EMPTY,
        QF_NOT_EMPTY,
    };

    struct VE_DLL_EXPORT FilterQuery
    {
        FilterQuery();
        ~FilterQuery();
        FilterQuery(unsigned column, Cell *cell, FilterQueryType op);

        char columnName[_TABLE_MAX_COLUMN_NAME_LENGTH];
        unsigned columnIndex;
        Cell *cellValue;
        FilterQueryType operation;
    };

    enum SortQueryType
    {
        QS_INCREASING_ORDER,
        QS_DECREASING_ORDER,
    };

    struct VE_DLL_EXPORT SortQuery
    {
        unsigned columnIndex;

        SortQueryType operation;
    };

    Table();

    ~Table();

    unsigned AddColumn(const char columnName[_TABLE_MAX_COLUMN_NAME_LENGTH], ColumnType columnType);

    void RemoveColumn(unsigned columnIndex);

    unsigned ColumnIndex(char columnName[_TABLE_MAX_COLUMN_NAME_LENGTH]) const;
    unsigned ColumnIndex(const char *columnName) const;

    char* ColumnName(unsigned index) const;

    ColumnType GetColumnType(unsigned index) const;

    unsigned GetColumnCount(void) const;

    unsigned GetRowCount(void) const;

    Table::Row* AddRow(unsigned rowId);
    Table::Row* AddRow(unsigned rowId, DataStructures::List<Cell> &initialCellValues);
    Table::Row* AddRow(unsigned rowId, DataStructures::List<Cell*> &initialCellValues, bool copyCells=false);

    bool RemoveRow(unsigned rowId);

    void RemoveRows(Table *tableContainingRowIDs);

    bool UpdateCell(unsigned rowId, unsigned columnIndex, int value);
    bool UpdateCell(unsigned rowId, unsigned columnIndex, char *str);
    bool UpdateCell(unsigned rowId, unsigned columnIndex, int byteLength, char *data);
    bool UpdateCellByIndex(unsigned rowIndex, unsigned columnIndex, int value);
    bool UpdateCellByIndex(unsigned rowIndex, unsigned columnIndex, char *str);
    bool UpdateCellByIndex(unsigned rowIndex, unsigned columnIndex, int byteLength, char *data);

    void GetCellValueByIndex(unsigned rowIndex, unsigned columnIndex, int *output);
    void GetCellValueByIndex(unsigned rowIndex, unsigned columnIndex, char *output);
    void GetCellValueByIndex(unsigned rowIndex, unsigned columnIndex, char *output, int *outputLength);

    Row* GetRowByID(unsigned rowId) const;

    Row* GetRowByIndex(unsigned rowIndex, unsigned *key) const;

    void QueryTable(unsigned *columnIndicesSubset, unsigned numColumnSubset, FilterQuery *inclusionFilters, unsigned numInclusionFilters, unsigned *rowIds, unsigned numRowIDs, Table *result);

    void SortTable(Table::SortQuery *sortQueries, unsigned numSortQueries, Table::Row** out);

    void Clear(void);

    void PrintColumnHeaders(char *out, int outLength, char columnDelineator) const;

    void PrintRow(char *out, int outLength, char columnDelineator, bool printDelineatorForBinary, Table::Row* inputRow) const;

    const DataStructures::List<ColumnDescriptor>& GetColumns(void) const;

    const DataStructures::BPlusTree<unsigned, Row*, _TABLE_BPLUS_TREE_ORDER>& GetRows(void) const;

    DataStructures::Page<unsigned, Row*, _TABLE_BPLUS_TREE_ORDER> * GetListHead(void);

    unsigned GetAvailableRowId(void) const;

    Table& operator = ( const Table& input );

protected:
    Table::Row* AddRowColumns(unsigned rowId, Row *row, DataStructures::List<unsigned> columnIndices);

    void DeleteRow(Row *row);

    void QueryRow(DataStructures::List<unsigned> &inclusionFilterColumnIndices, DataStructures::List<unsigned> &columnIndicesToReturn, unsigned key, Table::Row* row, FilterQuery *inclusionFilters, Table *result);

    DataStructures::BPlusTree<unsigned, Row*, _TABLE_BPLUS_TREE_ORDER> rows;

    DataStructures::List<ColumnDescriptor> columns;
};
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
