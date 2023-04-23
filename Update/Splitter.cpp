
#include <vector>
#include "Splitter.h"

typedef std::vector<std::string>::size_type size_type;
	//! Create and initialize a new Splitter
	//!
	//! \param[in] src The string to split
	//! \param[in] delim The delimiter to split the string around
	Splitter::Splitter ( const std::string& src, const std::string& delim )
	{
		reset ( src, delim );
	}

	//! Retrieve a split token at the specified index
	//!
	//! \param[in] i The index to search for a token
	//! \return The token at the specified index
	//! \throw std::out_of_range If the index is invalid
	std::string& Splitter::operator[] ( size_type i )
	{
		return _tokens.at ( i );
	}

	//! Retrieve the number of split tokens
	//!
	//! \return The number of split tokens
	const size_type Splitter::size()
	{
		return _tokens.size();
	}

	//! Re-initialize with a new source and delimiter
	//!
	//! \param[in] src The string to split
	//! \param[in] delim The delimiter to split the string around
	void Splitter::reset ( const std::string& src, const std::string& delim )
	{
		std::vector<std::string> tokens;
		std::string::size_type start = 0;
		std::string::size_type end;
		for ( ; ; ) {
			end = src.find ( delim, start );
			tokens.push_back ( src.substr ( start, end - start ) );
			// We just copied the last token
			if ( end == std::string::npos )
				break;
			// Exclude the delimiter in the next search
			start = end + delim.size();
		}
		_tokens.swap ( tokens );
	}

	//*************************************//

	typedef std::vector<std::wstring>::size_type wsize_type;
	//! Create and initialize a new Splitter
	//!
	//! \param[in] src The string to split
	//! \param[in] delim The delimiter to split the string around
	WSplitter::WSplitter ( const std::wstring& src, const std::wstring& delim )
	{
		wreset ( src, delim );
	}

	//! Retrieve a split token at the specified index
	//!
	//! \param[in] i The index to search for a token
	//! \return The token at the specified index
	//! \throw std::out_of_range If the index is invalid
	std::wstring& WSplitter::operator[] ( wsize_type i )
	{
		return _wtokens.at ( i );
	}

	//! Retrieve the number of split tokens
	//!
	//! \return The number of split tokens
	const wsize_type WSplitter::wsize() 
	{
		return _wtokens.size();
	}

	//! Re-initialize with a new source and delimiter
	//!
	//! \param[in] src The string to split
	//! \param[in] delim The delimiter to split the string around
	void WSplitter::wreset ( const std::wstring& src, const std::wstring& delim )
	{
		std::vector<std::wstring> tokens;
		std::wstring::size_type start = 0;
		std::wstring::size_type end;
		for ( ; ; ) {
			end = src.find ( delim, start );
			tokens.push_back ( src.substr ( start, end - start ) );
			// We just copied the last token
			if ( end == std::wstring::npos )
				break;
			// Exclude the delimiter in the next search
			start = end + delim.size();
		}
		_wtokens.swap ( tokens );
	}