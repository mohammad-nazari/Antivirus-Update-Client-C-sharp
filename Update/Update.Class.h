#ifndef _UPDATE_CLASS_H_
#define _UPDATE_CLASS_H_

#include "Header.h"
#include "Crypto.h"
#include "Base64.h"
#include "HardDriveSerialNumer.h"
#include "Splitter.h"
#include "EngineCtrl.h"
#include "Structures.h"

class UpdateClass
{
public:
	UpdateClass(inputStruct* InputStatus,wchar_t *UpdateFileInfoAddress);
	~UpdateClass();
	void DoUpdate();

private:
	HINSTANCE hGetProcIDDLL;
	EngineCtrl *ecObj;
	// Received stop request from service
	int * iServiceStatus;

	// load DLL library...
	funci PipeCout ;

	string strPublicKey, strPrivateKey, strEncryptKey, strSessionCode,  strUserKey, strMACAddress, strHardSerialNo, strServerAddress, strPreMasterKey, strMessage, strErroCode;
	ProxySetting psProxyObj;
	time_t tStartDate;
	bool bFinishedUpdate;
	string strLastFileUpdate;
	long int iIdNumber;
	size_t iFileSizeSum;
	wstring wstrRegistryAddress;

public:
	//Public Variables
	bool OK;

private:
	// private Methods
	string GeneratePassword(size_t Length);
	string Make16BitKey(string PreMasterKeyArg);
	string SerializeData(string DataArg);
	string UnSerializeData(string DataArg);
	string AESEncrypt(string DataArg);
	string AESDecrypt(string DataArg);
	string SecureEncrypt(string DataArg);
	string SecureDecryptFile(string DataArg);
	string SecureDecrypt(string DataArg);
	string SecureVerify(string DataArg);


	vector<FileInfo> vFileList;

	bool SetUpdateProperties(string UpdateFileInfoAddress);

	string GetAESKey();
	void SetAESKey(string Value);
	string GetPublicKey();
	void SetPublicKey(string Value);
	string GetPrivateKey();
	void SetPrivateKey(string Value);
	string GetPreMasterKey();
	void SetPreMasterKey(string Value);
	string GetUserKey();
	void SetUserKey(string Value);
	string GetSessionCode();
	void SetSessionCode(string Value);	
	void GetSystemMACAddress();
	void GetHardDriveSerial();
	void GetProxySetting();
	void GetSystemProxySetting();
	void SetProxyError(string Value);
	string GetProxyError();
	void SetProxyIP(string Value);
	char *GetProxyIP();
	void SetProxyPort(string Value);
	long int GetProxyPort();
	void SetProxyUserName(string Value);
	char *GetProxyUserName();
	void SetProxyPassWord(string Value);
	char *GetProxyPassWord();
	void SetMACAddress(string Value);
	string GetMACAddress();
	void SetHardSerialNumber(string Value);
	string GetHardSerialNumber();
	void SetFinishedUpdate(bool Value);
	bool GetFinishedUpdate();
	void SetServerAddress(string Value);
	string GetServerAddress();
	vector<FileInfo> GetVersionFile();
	void SetVersionFile(vector<FileInfo> Value);

	bool Replace(std::string& str, const std::string& from, const std::string& to);
	bool ReplaceW(std::wstring& str, const std::wstring& from, const std::wstring& to);
	string TrimString(string Input);
	wstring WTrimString(wstring Input);
	string WString2String(wstring InputArg);
	wstring String2WString(string InputArg);
	string IntToString(size_t IntData);
	wstring IntToStringW(size_t IntData);
	long int FindAllMaching(string Input, string MachStr);

	bool WriteFileData(string FilePath, string Data);
	string ReadFileData(string address, size_t *FSize);
	wstring ReadFileDataW(wstring address, size_t *FSize);
	bool FileExist(string FilePath);
	bool DeleteExistFile(string Path);

	void PrintData(long int OutputType, string Message);
	bool InsertLog(wstring wstrLogObject, wstring wstrLogEvent);
	double CalculateUpdateTimePeriod(double CurrentPercent, size_t CurrentFileSize);
	wstring GetSetupAddressFromRegistry();

	bool GetLastUpdateFileName();
	void CalcFilesSizeSummation();
	bool GetLocalCurrentTime(timeStruct &stcDateTime);
	bool GenerateFileList(string Result);

};
#endif