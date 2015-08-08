////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 GridSectorizer.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "VEMemoryOverride.h"

#ifdef _USE_ORDERED_LIST
#include "DS_OrderedList.h"
#else
#include "DS_List.h"
#endif

class GridSectorizer
{
public:
    GridSectorizer();
    ~GridSectorizer();

    void Init(const float _maxCellWidth, const float _maxCellHeight, const float minX, const float minY, const float maxX, const float maxY);

    void AddEntry(void *entry, const float minX, const float minY, const float maxX, const float maxY);

#ifdef _USE_ORDERED_LIST

    void RemoveEntry(void *entry, const float minX, const float minY, const float maxX, const float maxY);

    void MoveEntry(void *entry, const float sourceMinX, const float sourceMinY, const float sourceMaxX, const float sourceMaxY,
                   const float destMinX, const float destMinY, const float destMaxX, const float destMaxY);

#endif

    void GetEntries(DataStructures::List<void*>& intersectionList, const float minX, const float minY, const float maxX, const float maxY);

    void Clear(void);

protected:
    int WorldToCellX(const float input) const;
    int WorldToCellY(const float input) const;
    int WorldToCellXOffsetAndClamped(const float input) const;
    int WorldToCellYOffsetAndClamped(const float input) const;

    bool PositionCrossesCells(const float originX, const float originY, const float destinationX, const float destinationY) const;

    float cellOriginX, cellOriginY;
    float cellWidth, cellHeight;
    float invCellWidth, invCellHeight;
    float gridWidth, gridHeight;
    int gridCellWidthCount, gridCellHeightCount;

#ifdef _USE_ORDERED_LIST
    DataStructures::OrderedList<void*, void*>* grid;
#else
    DataStructures::List<void*>* grid;
#endif
};
