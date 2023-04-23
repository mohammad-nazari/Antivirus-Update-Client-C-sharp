#ifndef _SPLITTER_H_
#define _SPLITTER_H_
#include <string>
#include <vector>
#include <iostream>
using namespace std;
//! Maintains a collection of substrings that are
//! delimited by a string of one or more characters
class Splitter {
	//! Contains the split tokens
	std::vector<std::string> _tokens;
public:
	//! Subscript type for use with operator[]
	typedef std::vector<std::string>::size_type size_type;
public:

	Splitter ( const std::string& src, const std::string& delim );

	std::string& operator[] ( size_type i );

	const size_type size();

	void reset ( const std::string& src, const std::string& delim );
};

class WSplitter {
	//! Contains the split tokens
	std::vector<std::wstring> _wtokens;
public:
	//! Subscript type for use with operator[]
	typedef std::vector<std::wstring>::size_type wsize_type;
public:

	WSplitter ( const std::wstring& src, const std::wstring& delim );

	std::wstring& operator[] ( wsize_type i );

	const wsize_type wsize();

	void wreset ( const std::wstring& src, const std::wstring& delim );
};
#endif