////////////////////////////////////////////////////////////////////////////
//
//  Venus Casino Solution Header File.
//  Copyright (C), Venus Interactive Entertainment.2012
// -------------------------------------------------------------------------
//  File name:	 DS_Tree.h
//  Version:     v1.00
//  Created:     22/5/2011 by Neo
//  Compilers:   Visual Studio.NET
//  Description:
// -------------------------------------------------------------------------
//  History:
//  http://www.venusie.com
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Export.h"
#include "DS_List.h"
#include "DS_Queue.h"
#include "VEMemoryOverride.h"

namespace DataStructures
{
template <class TreeType>
class VE_DLL_EXPORT Tree
{
public:
    Tree();
    Tree(TreeType &inputData);
    ~Tree();
    void LevelOrderTraversal(DataStructures::List<Tree*> &output);
    void AddChild(TreeType &newData);
    void DeleteDecendants(void);

    TreeType data;
    DataStructures::List<Tree *> children;
};

template <class TreeType>
Tree<TreeType>::Tree()
{

}

template <class TreeType>
Tree<TreeType>::Tree(TreeType &inputData)
{
    data=inputData;
}

template <class TreeType>
Tree<TreeType>::~Tree()
{
    DeleteDecendants();
}

template <class TreeType>
void Tree<TreeType>::LevelOrderTraversal(DataStructures::List<Tree*> &output)
{
    unsigned i;
    Tree<TreeType> *node;
    DataStructures::Queue<Tree<TreeType>*> queue;

    for (i=0; i < children.Size(); i++)
        queue.Push(children[i]);

    while (queue.Size())
    {
        node=queue.Pop();
        output.Insert(node, _FILE_AND_LINE_);
        for (i=0; i < node->children.Size(); i++)
            queue.Push(node->children[i]);
    }
}

template <class TreeType>
void Tree<TreeType>::AddChild(TreeType &newData)
{
    children.Insert(VENet::OP_NEW<Tree>(newData, _FILE_AND_LINE_));
}

template <class TreeType>
void Tree<TreeType>::DeleteDecendants(void)
{
    unsigned int i;
    for (i=0; i < children.Size(); i++)
        VENet::OP_DELETE(children[i], _FILE_AND_LINE_);
}
}
