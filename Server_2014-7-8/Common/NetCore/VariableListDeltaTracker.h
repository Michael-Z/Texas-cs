#include "NativeTypes.h"
#include "DS_List.h"
#include "VEMemoryOverride.h"
#include "BitStream.h"
#pragma once

namespace VENet
{
class VariableListDeltaTracker
{
public:
    VariableListDeltaTracker();
    ~VariableListDeltaTracker();

    void StartWrite(void);

    bool IsPastEndOfList(void) const
    {
        return nextWriteIndex>=variableList.Size();
    }

    template <class VarType>
    bool WriteVar(const VarType &varData)
    {
        VENet::BitStream temp;
        temp.Write(varData);
        if (nextWriteIndex>=variableList.Size())
        {
            variableList.Push(VariableLastValueNode(temp.GetData(),temp.GetNumberOfBytesUsed()),_FILE_AND_LINE_);
            nextWriteIndex++;
            return true;
        }

        if (temp.GetNumberOfBytesUsed()!=variableList[nextWriteIndex].byteLength)
        {
            variableList[nextWriteIndex].lastData=(char*) rakRealloc_Ex(variableList[nextWriteIndex].lastData, temp.GetNumberOfBytesUsed(),_FILE_AND_LINE_);
            variableList[nextWriteIndex].byteLength=temp.GetNumberOfBytesUsed();
            memcpy(variableList[nextWriteIndex].lastData,temp.GetData(),temp.GetNumberOfBytesUsed());
            nextWriteIndex++;
            variableList[nextWriteIndex].isDirty=false;
            return true;
        }
        if (variableList[nextWriteIndex].isDirty==false && memcmp(temp.GetData(),variableList[nextWriteIndex].lastData, variableList[nextWriteIndex].byteLength)==0)
        {
            nextWriteIndex++;
            return false;
        }

        variableList[nextWriteIndex].isDirty=false;
        memcpy(variableList[nextWriteIndex].lastData,temp.GetData(),temp.GetNumberOfBytesUsed());
        nextWriteIndex++;
        return true;
    }
    template <class VarType>
    bool WriteVarToBitstream(const VarType &varData, VENet::BitStream *bitStream)
    {
        bool wasDifferent = WriteVar(varData);
        bitStream->Write(wasDifferent);
        if (wasDifferent)
        {
            bitStream->Write(varData);
            return true;
        }
        return false;
    }
    template <class VarType>
    bool WriteVarToBitstream(const VarType &varData, VENet::BitStream *bitStream, unsigned char *bArray, unsigned short writeOffset)
    {
        if (WriteVarToBitstream(varData,bitStream)==true)
        {
            BitSize_t numberOfBitsMod8 = writeOffset & 7;

            if ( numberOfBitsMod8 == 0 )
                bArray[ writeOffset >> 3 ] = 0x80;
            else
                bArray[ writeOffset >> 3 ] |= 0x80 >> ( numberOfBitsMod8 ); // Set the bit to 1

            return true;
        }
        else
        {
            if ( ( writeOffset & 7 ) == 0 )
                bArray[ writeOffset >> 3 ] = 0;

            return false;
        }
    }

    template <class VarType>
    static bool ReadVarFromBitstream(VarType &varData, VENet::BitStream *bitStream)
    {
        bool wasWritten;
        if (bitStream->Read(wasWritten)==false)
            return false;
        if (wasWritten)
        {
            if (bitStream->Read(varData)==false)
                return false;
        }
        return wasWritten;
    }

    void FlagDirtyFromBitArray(unsigned char *bArray);

    struct VariableLastValueNode
    {
        VariableLastValueNode();
        VariableLastValueNode(const unsigned char *data, int _byteLength);
        ~VariableLastValueNode();
        char *lastData;
        unsigned int byteLength;
        bool isDirty;
    };

protected:
    DataStructures::List<VariableLastValueNode> variableList;
    unsigned int nextWriteIndex;
};
}