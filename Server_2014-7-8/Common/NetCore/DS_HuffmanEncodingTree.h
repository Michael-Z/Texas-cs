////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DS_HuffmanEncodingTree.h
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
#include "DS_HuffmanEncodingTreeNode.h"
#include "BitStream.h"
#include "Export.h"
#include "DS_LinkedList.h"

namespace VENet
{

class VE_DLL_EXPORT HuffmanEncodingTree
{

public:
    HuffmanEncodingTree();
    ~HuffmanEncodingTree();

    void EncodeArray( unsigned char *input, size_t sizeInBytes, VENet::BitStream * output );

    unsigned DecodeArray( VENet::BitStream * input, BitSize_t sizeInBits, size_t maxCharsToWrite, unsigned char *output );
    void DecodeArray( unsigned char *input, BitSize_t sizeInBits, VENet::BitStream * output );

    void GenerateFromFrequencyTable( unsigned int frequencyTable[ 256 ] );

    void FreeMemory( void );

private:

    HuffmanEncodingTreeNode *root;

    struct CharacterEncoding
    {
        unsigned char* encoding;
        unsigned short bitLength;
    };

    CharacterEncoding encodingTable[ 256 ];

    void InsertNodeIntoSortedList( HuffmanEncodingTreeNode * node, DataStructures::LinkedList<HuffmanEncodingTreeNode *> *huffmanEncodingTreeNodeList ) const;
};

}