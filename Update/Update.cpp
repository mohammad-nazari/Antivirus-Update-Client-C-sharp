// Update.cpp : Defines the entry point for the console application.
//
#pragma once

//#include "stdafx.h"

#include "Update.Class.h"
#include <string>
#include <iostream>
#include <tchar.h>
#include <conio.h>

using namespace std;

const char server[] = "http://172.16.7.92/webservice/Server/Services.php";

int _tmain(int argc, _TCHAR* argv[])
{
	inputStruct *isObj = new inputStruct;

	EngineCtrl *ecObj = new EngineCtrl();
	isObj->DB_handle = (void*)ecObj;
	isObj->mud_State = new int;
	*(isObj->mud_State) = 1;

	// Generate an object from Update client class
	UpdateClass *updateObj = new UpdateClass(isObj, argv[1]);
	do 
	{
		// run update module
		updateObj->DoUpdate();

		cout << "\nDo you want to do Update again? (y(Yes) any else exit\n)";

	} while (_getch() == 'y');
	// delete object
	delete(updateObj);

	system("pause");
	return 0;
}