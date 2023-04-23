/*
* @name EngineCtrl 
* @since Wed 24 July 2013
* @author Hossein Alizadeh
* @Dsc Engine to Load Signature Database
*/
#ifndef Eng_Ctrl_H
#define Eng_Ctrl_H
#include <iostream>
#include <fstream>
#include <Windows.h>
#include "SignatureEntity.hpp"
#include "MD5Sort.hpp"
#include "AwesomeAhoCorasick.hpp"
#include "MD5Sort.hpp"
#include "SigTypes.hpp"
#include "Registry.hpp"
#include "APIHeuristic.hpp"

#define BLOCK_SIZE 32 //READING AND DECRYPTING BUFFER
#define INSTALLED_PATH ReadRegValue(HKEY_LOCAL_MACHINE,L"SOFTWARE\\AV",L"",0)
#define NDB_STRUCT_PATH (wchar_t*)INSTALLED_PATH.append(L"\\DataBase\\NDB.DB").c_str()
#define HMDB_STRUCT_PATH (wchar_t*)INSTALLED_PATH.append(L"\\DataBase\\HMDB.DB").c_str()
#define APIH_STRUCT_PATH (wchar_t*)INSTALLED_PATH.append(L"\\DataBase\\APIH.DB").c_str()


class EngineCtrl{
private:
	MD5Sort *HMDB;
	APIHeuristic *APIH;
	AwesomeAhoCorasick *NDB;
	uchar* CharArrayCutNextLine(uchar* CharArray);
	bool *HMDB_Loaded,*NDB_Loaded,*APIH_Loaded;
public:
	__declspec(dllexport) EngineCtrl();
	__declspec(dllexport) ~EngineCtrl();
	__declspec(dllexport) bool Update_HMDB(uchar *DataContentPointer);
	__declspec(dllexport) bool Update_NDB(uchar *DataContentPointer);
	__declspec(dllexport) bool Load_HMDB();
	__declspec(dllexport) bool Load_NDB();
	__declspec(dllexport) bool Load_APIH();
	__declspec(dllexport) bool Unload_HMDB();
	__declspec(dllexport) bool Unload_NDB();
	__declspec(dllexport) bool Unload_APIH();
	__declspec(dllexport) bool APIHCheck(uchar* ASCIIString, ui64 Len);
	__declspec(dllexport) bool HMDBCheck(uchar* _MD5_ASCII,char *MalName);
	__declspec(dllexport) char* NDBSearch(bool NewSearch,ulong &Start,uchar* Data, size_t DataSize,NDBSig_TargetType Type,ulong EntryPoint,vector<ulong> Sections);
	__declspec(dllexport) ulong NumberOfHMDBSignatures();
	__declspec(dllexport) ulong NumberOfNDBSignatures();
};
#endif