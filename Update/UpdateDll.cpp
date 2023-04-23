// Update.cpp : Defines the entry point for the console application.

#include "Crypto.h"
#include "Update.Class.h"
#include <string>
#include <iostream>
#include <tchar.h>
#include "Output.h"

using namespace std;

EXTERN_C __declspec(dllexport) int CallUpdate(inputStruct *inputStatus,wchar_t *UpdateInfoFileAddress)
{
	//Generate an object from Update client class
	UpdateClass *updateObj = new UpdateClass(inputStatus, UpdateInfoFileAddress);
	
	updateObj->DoUpdate();
	
	// delete object
	delete(updateObj);

	return 0;
}