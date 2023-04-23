//----------------------------------------------------------------
//----------------------Majid Abdollahi------------------------- |
//----------------------Persian---Guard------------------------- |
//----------------------Summer-----2013------------------------- |
//----------------------------------------------------------------

#include <iostream>
#include <Windows.h>
#include <vector>

using namespace std;
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

static wstring ReadRegValue(HKEY hkRootPath, wstring wstrKeyPath, wstring wstrKeyName, int iInputTaskID)
	{
		HKEY hkey;

		// open registry root key...
		if (RegOpenKeyEx(hkRootPath, wstrKeyPath.c_str(), 0, KEY_READ, &hkey) != ERROR_SUCCESS)
		{
	
			return L"NULL";
		}

		DWORD type;
		DWORD cbData;

		// open registry subkey values...
		if (RegQueryValueEx(hkey, wstrKeyName.c_str(), NULL, &type, NULL, &cbData) != ERROR_SUCCESS)
		{
			RegCloseKey(hkey);
	
			return L"NULL";
		}

		if (type != REG_SZ)
		{
			RegCloseKey(hkey);
	
			return L"NULL";
		}

		wstring value(cbData/sizeof(wchar_t), L'\0');

		if (RegQueryValueEx(hkey, wstrKeyName.c_str(), NULL, NULL, reinterpret_cast<LPBYTE>(&value[0]), &cbData) != ERROR_SUCCESS)
		{
			RegCloseKey(hkey);
	
			return L"NULL";
		}

		RegCloseKey(hkey);

		size_t firstNull = value.find_first_of(L'\0');
		if (firstNull != string::npos)
			value.resize(firstNull);
		return value;
		//wcout << ReadRegValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", L"AdobeAAMUpdater-1.0");
	}
