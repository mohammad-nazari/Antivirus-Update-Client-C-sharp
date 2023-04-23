#include <iostream>
#include <windows.h>
#include <shlwapi.h>
#include <string>
#include <string.h>
#include <conio.h>
#include "EngineCtrl.h"

using namespace std;

struct inputStruct
{
	void* DB_handle;
	int* mud_State;
	HINSTANCE* hXMLDll;
};

typedef int (*f_funci)(inputStruct*,wchar_t*);

int main()
{
	cout << "update running ..." << endl;
	do 
	{
		if (HMODULE hUpdate = LoadLibrary(L"Update.dll"))
		{
			if(f_funci PipUpdate = (f_funci)GetProcAddress(hUpdate, "CallUpdate"))
			{
				inputStruct *isObj = new inputStruct;

				EngineCtrl *ecObj = new EngineCtrl();
				isObj->DB_handle = (void*)ecObj;
				isObj->mud_State = new int;
				*(isObj->mud_State) = 1;

				if(isObj->hXMLDll = (HINSTANCE*)LoadLibrary(L"F:\\APA\\Projects\\New Project\\Update\\Client\\C++\\Update\\x64\\Debug\\Xerces.dll"))
				{
					PipUpdate(isObj,L"MyUpdate*10*C:\\DataBase\\Update\\UpdateInfo\\update.info");
				}
				else
				{
					cout << "Can Not Load 'Xerces' DLL" << endl;
				}
			}
			else
			{
				cout << "Can Not Load 'CallUpdate' Method" << endl;
			}
			FreeLibrary(hUpdate);
		}
		else
		{
			cout << "Can not Load 'Update' DLL" << endl;
		}
		cout << "\nDo you want to do Update again? (y(Yes) any else exit\n)";
	} while (_getch() == 'y');

	system("pause");
	return 0;
}