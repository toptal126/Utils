///////////  Generic string helper functions that work on both std::string and std::wstring  ////////////////////
#pragma once
#include <string>
#include <stdarg.h>
#include <tchar.h>
#include <iostream>
#include <locale>
#include <windows.h>


//Returns C-style formatted std string
#define _F(...)		format(##__VA_ARGS__)
#define _FN(...)	format(##__VA_ARGS__).append(1, '\n')



using namespace std;


//Trim the given string from the left and return reference
template<class CHARTYPE>
inline static basic_string<CHARTYPE>& triml(basic_string<CHARTYPE>& s)
{
	s.erase(0, s.find_first_not_of(' '));
	return s;
}

//Trim the given string from the right and return reference
template<class CHARTYPE>
inline static basic_string<CHARTYPE>& trimr(basic_string<CHARTYPE>& s)
{
	s.erase(s.find_last_not_of(' ') + 1, s.size());
	return s;
}

//Trim the given string from both sides and return reference
template<class CHARTYPE>
inline static basic_string<CHARTYPE>& trim(basic_string<CHARTYPE>& s)
{
	return trimr(triml(s));
}


#pragma region format() Helpers
//Helper function used in format()
//Base Template for CHARTYPEs
template<class CHARTYPE>
int my_vsnprintf(
	_Out_writes_(_BufferCount)    CHARTYPE*       const _Buffer,
	_In_                          size_t          const _BufferCount,
	_In_z_ _Printf_format_string_ CHARTYPE const* const _Format,
	va_list           _ArgList
)
{
	std::cerr << "ERROR: Unknown CHARTYPE!! Provide template specialisation!" << std::endl;
	return -1;
}

//Helper function used in format()
//Template Specialisation for ASCII strings (CHARTYPE = char)
template<>
int my_vsnprintf(
	_Out_writes_(_BufferCount)    char*       const _Buffer,
	_In_                          size_t      const _BufferCount,
	_In_z_ _Printf_format_string_ char const* const _Format,
	va_list           _ArgList
)
{
	return vsnprintf(_Buffer, _BufferCount, _Format, _ArgList);
}

//Helper function used in format()
//Template Specialisation for Unicode strings (CHARTYPE = TCHAR)
template<>
int my_vsnprintf(
	_Out_writes_(_BufferCount)    TCHAR*          const _Buffer,
	_In_                          size_t          const _BufferCount,
	_In_z_ _Printf_format_string_ TCHAR const*    const _Format,
	va_list           _ArgList
)
{
	return vswprintf(_Buffer, _BufferCount, _Format, _ArgList);
}

#pragma endregion


//C-style string formatting on arbitrary-length input
//Supply larger InitialBufferSize for larger strings for better performance
template<size_t InitialBufferSize = 1024u, class CHARTYPE>
std::basic_string<CHARTYPE> format(const CHARTYPE* fmt, ...)
{
	va_list args;

	std::basic_string<CHARTYPE> result;
	int rc = -1;
	size_t length = InitialBufferSize;

	while (true)
	{
		va_start(args, fmt);
		result.resize(length);
		rc = my_vsnprintf(const_cast<CHARTYPE*>(result.data()), length, fmt, args);
		va_end(args);

		if (rc > -1 && static_cast<unsigned int>(rc) < length)
		{//all ok
			result.resize(rc);
			return result;
		}

		if (rc > -1)
			//needed size returned, allocate 1 byte for the null terminator
			length = rc + 1;
		else
			//Error occurred. Try doubling buffer size
			length *= 2;
	}

	return result;
}