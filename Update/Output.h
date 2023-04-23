#ifndef OUTPUT_H
#define OUTPUT_H 

#include <windows.h>

typedef int (*funci)(wchar_t  SnedMSG[]);

#define LogMsg L"Log"
#define PathMsg L"Path"
#define Delimeter L"*"
#define MaxTaskIDSize 4
#define MaxPercentSize 4
#define MaxCommandSize 512

static class OutputClass
{
private:
	// load Dll library...
	funci PipeCout ;
	HMODULE hGetProcIDDLL;

public:

	OutputClass()
	{
		// load Dll library...
		hGetProcIDDLL = LoadLibrary(L"SendMsgToPipe.dll");

		if (hGetProcIDDLL != INVALID_HANDLE_VALUE)
		{
			// define function from Dll file...
			PipeCout = (funci)GetProcAddress(hGetProcIDDLL, "SendCMD");
		}
	}

	~OutputClass()
	{
		FreeLibrary(hGetProcIDDLL);
	}

	///<summary>CustomOutput print input string on console or pipe channel.</summary>
	///<param name="wcInputWideChar">input wide char string.</param>
	///<param name="wcType">input type string.</param>
	///<param name="iPercent">Scan Percent.</param>
	///<returns>Return true if success.</returns>
	bool _fastcall CustomOutput( wchar_t* PipMessage);
};
#endif