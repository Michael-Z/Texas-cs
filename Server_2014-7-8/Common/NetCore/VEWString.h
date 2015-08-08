#pragma once

#include "Export.h"
#include "VENetTypes.h" // int64_t
#include "VEString.h"

#ifdef _WIN32

#include "WindowsIncludes.h"
#endif

namespace VENet
{
	/// \brief String class for Unicode
	class VE_DLL_EXPORT VEWString
	{
	public:
		// Constructors
		VEWString();
		VEWString( const VEString &right );
		VEWString( const wchar_t *input );
		VEWString( const VEWString & right);
		VEWString( const char *input );
		~VEWString();

		/// Implicit return of wchar_t*
		operator wchar_t* () const {if (c_str) return c_str; return (wchar_t*) L"";}

		/// Same as std::string::c_str
		const wchar_t* C_String(void) const {if (c_str) return c_str; return (const wchar_t*) L"";}

		/// Assignment operators
		VEWString& operator = ( const VEWString& right );
		VEWString& operator = ( const VEString& right );
		VEWString& operator = ( const wchar_t * const str );
		VEWString& operator = ( wchar_t *str );
		VEWString& operator = ( const char * const str );
		VEWString& operator = ( char *str );

		/// Concatenation
		VEWString& operator +=( const VEWString& right);
		VEWString& operator += ( const wchar_t * const right );
		VEWString& operator += ( wchar_t *right );

		/// Equality
		bool operator==(const VEWString &right) const;

		// Comparison
		bool operator < ( const VEWString& right ) const;
		bool operator <= ( const VEWString& right ) const;
		bool operator > ( const VEWString& right ) const;
		bool operator >= ( const VEWString& right ) const;

		/// Inequality
		bool operator!=(const VEWString &right) const;

		/// Set the value of the string
		void Set( wchar_t *str );

		/// Returns if the string is empty. Also, C_String() would return ""
		bool IsEmpty(void) const;

		/// Returns the length of the string
		size_t GetLength(void) const;

		/// Has the string into an unsigned int
		static unsigned long ToInteger(const VEWString &rs);

		/// Compare strings (case sensitive)
		int StrCmp(const VEWString &right) const;

		/// Compare strings (not case sensitive)
		int StrICmp(const VEWString &right) const;

		/// Clear the string
		void Clear(void);

		/// Print the string to the screen
		void Printf(void);

		/// Print the string to a file
		void FPrintf(FILE *fp);

		/// Serialize to a bitstream, uncompressed (slightly faster)
		/// \param[out] bs Bitstream to serialize to
		void Serialize(BitStream *bs) const;

		/// Static version of the Serialize function
		static void Serialize(const wchar_t * const str, BitStream *bs);

		/// Deserialize what was written by Serialize
		/// \param[in] bs Bitstream to serialize from
		/// \return true if the deserialization was successful
		bool Deserialize(BitStream *bs);

		/// Static version of the Deserialize() function
		static bool Deserialize(wchar_t *str, BitStream *bs);


	protected:
		wchar_t* c_str;
		size_t c_strCharLength;
	};

}

const VENet::VEWString VE_DLL_EXPORT operator+(const VENet::VEWString &lhs, const VENet::VEWString &rhs);