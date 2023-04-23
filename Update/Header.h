#ifndef _Definitions_H
#define _Definitions_H

#include <Windows.h>
#include <iphlpapi.h>
#include <iptypes.h>
#include <string>
#include <winhttp.h>
#include <time.h>
#include <iosfwd>
#include <winver.h>
#include <algorithm>
#include <regex>
#include <io.h>
#include <iosfwd>
#include <fstream>
#include <iosfwd>
#include <sstream>
#include <list>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <regex>

using namespace std;

// Switch in PipMethod and Console output
#define PIPTYPE				1

// Number of signatures gathered per request
#define UPDATEROWCOUNT		100000

// Is Up to date
#define UPTODATE			true

// Length of Pre master key to create main key of aes cryptography
#define PREMASTERKEYLENGTH	32

// Repeat values
#define FIRSTREPEAT			5
#define SECONDREPEAT		5
#define THIRDREPEAT			5

// SOAP timeout data
#define SEND_TIMEOUT		60
#define RECV_TIMEOUT		600
#define CONNECT_TIMEOUT		30
#define ACCEPT_TIMEOUT		30

// Define GUI Codes Sending to PipMethode
#define UPDATESTART			101
#define UPDATEPAUSE			102
#define UPDATEPERCENT		103
#define UPDATEMESSAGE		150
#define UPDATESTOP			198
#define UPDATEFINISH		199
#define UPDATELOG			"Log"
#define UPDATEPATH			"Path"
#define UPDATESTAR			"*"

#define SERVERADDRESS		L"http://91.98.146.4:8080"
//#define SERVERADDRESS		L"http://192.168.1.107:8080"

// Sleep Time
#define SLEEPTIME			1000

// Service Status 
#define SERVICESTATUS		2

typedef DWORD (*SelectAllSpecialColumn)( wstring &tableName,vector<wstring> &Resultset,wstring &columnList,wstring &whatsup );
typedef int (*SelectAllOldSpecialColumnConditional)(wstring &tableName,vector<wstring> &ResultSet,wstring &columns,wstring &condition,wstring &whatsups);
typedef int (*InsertRecordOldVersion)( wstring &tableName,wstring &record,wstring &whatsup );


typedef int (*funci)(wchar_t  SnedMSG[]);

typedef unsigned long ulong;
// Proxy Setting
struct ProxySetting{
	string ip;
	string port;
	string username;
	string password;
	string error;
};

// Service Status
struct inputStruct
{
	void* DB_handle;
	int* mud_State;
	HINSTANCE hXMLDll; 
};

// Time Attributes
struct timeStruct
{
	size_t iSecend;
	size_t iMinut;
	size_t iHoure;
	size_t iDay;
	size_t iMonth;
	size_t iYear;
};

struct FileInfo
{
	string FileName;
	int FileSize;
	string FolderName;
};

#endif // !_Definitions_H