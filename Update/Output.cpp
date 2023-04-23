#include <iostream>
#include <windows.h>

#include "Output.h"

using namespace std;

bool _fastcall OutputClass::CustomOutput( wchar_t* PipMessage )
{
	return PipeCout(PipMessage);
}