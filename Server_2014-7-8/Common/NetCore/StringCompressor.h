#pragma once

#include "Export.h"
#include "DS_Map.h"
#include "VEMemoryOverride.h"
#include "NativeTypes.h"

#ifdef _STD_STRING_COMPRESSOR
#include <string>
#endif

namespace VENet
{
	class BitStream;
	class VEString;
};


namespace VENet
{
class HuffmanEncodingTree;

class VE_DLL_EXPORT StringCompressor
{
public:
	
	~StringCompressor();
	
	static StringCompressor* Instance(void);

	void GenerateTreeFromStrings( unsigned char *input, unsigned inputLength, uint8_t languageId );
	
	void EncodeString( const char *input, int maxCharsToWrite, VENet::BitStream *output, uint8_t languageId=0 );
	
	bool DecodeString( char *output, int maxCharsToWrite, VENet::BitStream *input, uint8_t languageId=0 );

#ifdef _CSTRING_COMPRESSOR
	void EncodeString( const CString &input, int maxCharsToWrite, VENet::BitStream *output, uint8_t languageId=0 );
	bool DecodeString( CString &output, int maxCharsToWrite, VENet::BitStream *input, uint8_t languageId=0 );
#endif

#ifdef _STD_STRING_COMPRESSOR
	void EncodeString( const std::string &input, int maxCharsToWrite, VENet::BitStream *output, uint8_t languageId=0 );
	bool DecodeString( std::string *output, int maxCharsToWrite, VENet::BitStream *input, uint8_t languageId=0 );
#endif

	void EncodeString( const VENet::VEString *input, int maxCharsToWrite, VENet::BitStream *output, uint8_t languageId=0 );
	bool DecodeString( VENet::VEString *output, int maxCharsToWrite, VENet::BitStream *input, uint8_t languageId=0 );

	static void AddReference(void);
	
	static void RemoveReference(void);

	StringCompressor();

private:
	
	static StringCompressor *instance;
	
	DataStructures::Map<int, HuffmanEncodingTree *> huffmanEncodingTrees;
	
	static int referenceCount;
};

}