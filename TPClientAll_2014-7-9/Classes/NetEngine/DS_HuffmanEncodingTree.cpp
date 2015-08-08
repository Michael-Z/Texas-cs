#include "DS_HuffmanEncodingTree.h"
#include "DS_Queue.h"
#include "BitStream.h"
#include "VEAssert.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

using namespace VENet;

HuffmanEncodingTree::HuffmanEncodingTree()
{
    root = 0;
}

HuffmanEncodingTree::~HuffmanEncodingTree()
{
    FreeMemory();
}

void HuffmanEncodingTree::FreeMemory( void )
{
    if ( root == 0 )
        return ;

    DataStructures::Queue<HuffmanEncodingTreeNode *> nodeQueue;

    HuffmanEncodingTreeNode *node;

    nodeQueue.Push( root, _FILE_AND_LINE_  );

    while ( nodeQueue.Size() > 0 )
    {
        node = nodeQueue.Pop();

        if ( node->left )
            nodeQueue.Push( node->left, _FILE_AND_LINE_  );

        if ( node->right )
            nodeQueue.Push( node->right, _FILE_AND_LINE_  );

        VENet::OP_DELETE(node, _FILE_AND_LINE_);
    }

    for ( int i = 0; i < 256; i++ )
        veFree_Ex(encodingTable[ i ].encoding, _FILE_AND_LINE_ );

    root = 0;
}


void HuffmanEncodingTree::GenerateFromFrequencyTable( unsigned int frequencyTable[ 256 ] )
{
    int counter;
    HuffmanEncodingTreeNode * node;
    HuffmanEncodingTreeNode *leafList[ 256 ];
    DataStructures::LinkedList<HuffmanEncodingTreeNode *> huffmanEncodingTreeNodeList;

    FreeMemory();

    for ( counter = 0; counter < 256; counter++ )
    {
        node = VENet::OP_NEW<HuffmanEncodingTreeNode>( _FILE_AND_LINE_ );
        node->left = 0;
        node->right = 0;
        node->value = (unsigned char) counter;
        node->weight = frequencyTable[ counter ];

        if ( node->weight == 0 )
            node->weight = 1;

        leafList[ counter ] = node;

        InsertNodeIntoSortedList( node, &huffmanEncodingTreeNodeList );
    }

#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif
    while ( 1 )
    {
        huffmanEncodingTreeNodeList.Beginning();
        HuffmanEncodingTreeNode *lesser, *greater;
        lesser = huffmanEncodingTreeNodeList.Pop();
        greater = huffmanEncodingTreeNodeList.Pop();
        node = VENet::OP_NEW<HuffmanEncodingTreeNode>( _FILE_AND_LINE_ );
        node->left = lesser;
        node->right = greater;
        node->weight = lesser->weight + greater->weight;
        lesser->parent = node;
        greater->parent = node;

        if ( huffmanEncodingTreeNodeList.Size() == 0 )
        {
            root = node;
            root->parent = 0;
            break;
        }

        InsertNodeIntoSortedList( node, &huffmanEncodingTreeNodeList );
    }

    bool tempPath[ 256 ];
    unsigned short tempPathLength;
    HuffmanEncodingTreeNode *currentNode;
    VENet::BitStream bitStream;

    for ( counter = 0; counter < 256; counter++ )
    {
        tempPathLength = 0;

        currentNode = leafList[ counter ];

        do
        {
            if ( currentNode->parent->left == currentNode )
                tempPath[ tempPathLength++ ] = false;
            else
                tempPath[ tempPathLength++ ] = true;

            currentNode = currentNode->parent;
        }

        while ( currentNode != root );

        while ( tempPathLength-- > 0 )
        {
            if ( tempPath[ tempPathLength ] )
                bitStream.Write1();
            else
                bitStream.Write0();
        }

        encodingTable[ counter ].bitLength = ( unsigned char ) bitStream.CopyData( &encodingTable[ counter ].encoding );

        bitStream.Reset();
    }
}

void HuffmanEncodingTree::EncodeArray( unsigned char *input, size_t sizeInBytes, VENet::BitStream * output )
{
    unsigned counter;

    for ( counter = 0; counter < sizeInBytes; counter++ )
    {
        output->WriteBits( encodingTable[ input[ counter ] ].encoding, encodingTable[ input[ counter ] ].bitLength, false );
    }

    if ( output->GetNumberOfBitsUsed() % 8 != 0 )
    {
        unsigned char remainingBits = (unsigned char) ( 8 - ( output->GetNumberOfBitsUsed() % 8 ) );

        for ( counter = 0; counter < 256; counter++ )
            if ( encodingTable[ counter ].bitLength > remainingBits )
            {
                output->WriteBits( encodingTable[ counter ].encoding, remainingBits, false );
                break;
            }

#ifdef _DEBUG
        VEAssert( counter != 256 );
#endif

    }
}

unsigned HuffmanEncodingTree::DecodeArray( VENet::BitStream * input, BitSize_t sizeInBits, size_t maxCharsToWrite, unsigned char *output )
{
    HuffmanEncodingTreeNode * currentNode;

    unsigned outputWriteIndex;
    outputWriteIndex = 0;
    currentNode = root;


    for ( unsigned counter = 0; counter < sizeInBits; counter++ )
    {
        if ( input->ReadBit() == false )
            currentNode = currentNode->left;
        else
            currentNode = currentNode->right;

        if ( currentNode->left == 0 && currentNode->right == 0 )
        {
            if ( outputWriteIndex < maxCharsToWrite )
                output[ outputWriteIndex ] = currentNode->value;

            outputWriteIndex++;

            currentNode = root;
        }
    }

    return outputWriteIndex;
}

void HuffmanEncodingTree::DecodeArray( unsigned char *input, BitSize_t sizeInBits, VENet::BitStream * output )
{
    HuffmanEncodingTreeNode * currentNode;

    if ( sizeInBits <= 0 )
        return ;

    VENet::BitStream bitStream( input, BITS_TO_BYTES(sizeInBits), false );

    currentNode = root;

    for ( unsigned counter = 0; counter < sizeInBits; counter++ )
    {
        if ( bitStream.ReadBit() == false )
            currentNode = currentNode->left;
        else
            currentNode = currentNode->right;

        if ( currentNode->left == 0 && currentNode->right == 0 )
        {
            output->WriteBits( &( currentNode->value ), sizeof( char ) * 8, true );
            currentNode = root;
        }
    }
}

void HuffmanEncodingTree::InsertNodeIntoSortedList( HuffmanEncodingTreeNode * node, DataStructures::LinkedList<HuffmanEncodingTreeNode *> *huffmanEncodingTreeNodeList ) const
{
    if ( huffmanEncodingTreeNodeList->Size() == 0 )
    {
        huffmanEncodingTreeNodeList->Insert( node );
        return ;
    }

    huffmanEncodingTreeNodeList->Beginning();

    unsigned counter = 0;
#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif
    while ( 1 )
    {
        if ( huffmanEncodingTreeNodeList->Peek()->weight < node->weight )
            ++( *huffmanEncodingTreeNodeList );
        else
        {
            huffmanEncodingTreeNodeList->Insert( node );
            break;
        }

        if ( ++counter == huffmanEncodingTreeNodeList->Size() )
        {
            huffmanEncodingTreeNodeList->End();

            huffmanEncodingTreeNodeList->Add( node );
            break;
        }
    }
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
