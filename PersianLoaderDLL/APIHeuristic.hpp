/*
* @name API Heuristic Class
* @since Sat 19 April 2014
* @author Hossein Alizadeh
*/
#ifndef APIHeuristic_H
#define APIHeuristic_H
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <cstring>
#include "SigTypes.hpp"
using namespace std;
struct StrCmp
{
   bool operator()(char const *a, char const *b)
   {
      return strcmp(a, b) < 0;
   }
};

class APIHeuristic{
private:
	char* APIs;
public:
	APIHeuristic(){
	}
	bool Search(uchar* ASCIIString, ui64 Len){
		return false;
	}
	bool LoadFromFile(wchar_t *Path){
		//vector<char> APIs;
		//map<char*,bool,StrCmp> APIMap;
		FILE *tmp=_wfopen(Path,L"rb");
		if(!tmp)
			return false;
		//Number of API Characters
		ulong APICC=0;
		fread(&APICC,sizeof(ulong),1,tmp);
		APIs=new char[APICC];
		fread(APIs,sizeof(char),APICC,tmp);
	}
	void FreeData(){
	
	}
};
#endif