#include "soapUpdateProxy.h"
#include "Update.nsmap"

#include "Update.Class.h"


bool CompareByFileName(const FileInfo &a, const FileInfo &b)
{
	return ((a.FileName <= b.FileName) ? true : false);
}

//************************************
// Method:    UpdateClass
// FullName:  UpdateClass::UpdateClass
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: string UpdateFileInfoAddress
// Summary:	  Read file of initialize data for update like
//			  userkey, server address, proxy setting, 
//			  files info list file address, 
//			  temporary directory,
//			  last update info file address
//			  set number of signatures (db and hmdb)
//			  generate aes rijnddael key
//************************************
UpdateClass::UpdateClass(inputStruct* InputStatus, wchar_t *UpdateFileInfoAddress)
{
	this->InsertLog(L"Start Update", L"OK");

	// Load Signature data from file
	ecObj = (EngineCtrl *)InputStatus->DB_handle;
	hGetProcIDDLL = InputStatus->hXMLDll;

	this->iServiceStatus = InputStatus->mud_State;

	this->wstrRegistryAddress = this->GetSetupAddressFromRegistry();

	// Get Initializing Data from file
	if(this->SetUpdateProperties(this->WString2String(UpdateFileInfoAddress)))
	{
		this->OK = true;
	}
	else
	{
		this->OK = false;
	}
}

//************************************
// Method:    ~UpdateClass
// FullName:  UpdateClass::~UpdateClass
// Access:    public 
// Returns:   
// Qualifier:
// Summary:	  
//************************************
UpdateClass::~UpdateClass()
{
}

//************************************
// Method:    GetAESKey
// FullName:  UpdateClass::GetAESKey
// Access:    public 
// Returns:   char *
// Qualifier:
// Summary:	  
//************************************
string UpdateClass::GetAESKey()
{
	return this->TrimString(this->strEncryptKey);
}

//************************************
// Method:    SetAESKey
// FullName:  UpdateClass::SetAESKey
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: string Value
// Summary:	  
//************************************
void UpdateClass::SetAESKey(string Value)
{
	this->strEncryptKey = Value;
}

//************************************
// Method:    GetPublicKey
// FullName:  UpdateClass::GetPublicKey
// Access:    public 
// Returns:   string
// Qualifier:
// Summary:	  
//************************************
string UpdateClass::GetPublicKey()
{
	return this->TrimString(this->strPublicKey);
}

//************************************
// Method:    SetPublicKey
// FullName:  UpdateClass::SetPublicKey
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: string Value
// Summary:	  
//************************************
void UpdateClass::SetPublicKey(string Value)
{
	this->strPublicKey = Value;
}

//************************************
// Method:    GetPrivateKey
// FullName:  UpdateClass::GetPrivateKey
// Access:    public 
// Returns:   string
// Qualifier:
// Summary:	  
//************************************
string UpdateClass::GetPrivateKey()
{
	return this->TrimString(this->strPrivateKey);
}

//************************************
// Method:    SetPrivateKey
// FullName:  UpdateClass::SetPrivateKey
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: string Value
// Summary:	  
//************************************
void UpdateClass::SetPrivateKey(string Value)
{
	this->strPrivateKey = Value;
}

//************************************
// Method:    GetUserKey
// FullName:  UpdateClass::GetUserKey
// Access:    public 
// Returns:   string
// Qualifier:
// Summary:	  
//************************************
string UpdateClass::GetUserKey()
{
	return this->TrimString(this->strUserKey);
}

//************************************
// Method:    SetUserKey
// FullName:  UpdateClass::SetUserKey
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: string Value
// Summary:	  
//************************************
void UpdateClass::SetUserKey(string Value)
{
	this->strUserKey = Value;
}

//************************************
// Method:    GetSessionCode
// FullName:  UpdateClass::GetSessionCode
// Access:    public 
// Returns:   char *
// Qualifier:
// Summary:	  
//************************************
string UpdateClass::GetSessionCode()
{
	return this->TrimString(this->strSessionCode.c_str());
}

//************************************
// Method:    SetSessionCode
// FullName:  UpdateClass::SetSessionCode
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: string Value
// Summary:	  
//************************************
void UpdateClass::SetSessionCode(string Value)
{
	this->strSessionCode = Value;
}

//************************************
// Method:		Make16BitKey
// FullName:	UpdateClass::Make16BitKey
// Access:		private 
// Returns:		bool
// Qualifier:	
// Parameter:	string PreMasterKey
// Description:	Algorithm for generate aes key
//				It Get a pre_master key and generate main key
//************************************
string UpdateClass::Make16BitKey(string PreMasterKeyArg)
{
	size_t first = 0, limit = 0, pmKeyLen = strlen(PreMasterKeyArg.c_str());

	if(pmKeyLen)
	{
		uchar *strAESKey = new uchar[(AES_KEYLEN / 8)];
		uchar md5Out[32];
		MD5((const uchar *)PreMasterKeyArg.c_str(), pmKeyLen, md5Out);

		first = md5Out[15];
		limit = md5Out[13] * md5Out[14];

		first = (first < pmKeyLen ? first : first % pmKeyLen);
		limit = (limit < (pmKeyLen - first) ? limit : limit % (pmKeyLen - first));

		// Second mD5 from first MD5 of pre_master Key in a limited string of pre_master Key
		MD5((const unsigned char *)PreMasterKeyArg.c_str() + first, limit, strAESKey);

		// Length of AES key is 256 bit(32 byte) else is 128 bit(16 byte)
		if ((AES_KEYLEN / 8) == 32)
		{
			MD5((const unsigned char *)PreMasterKeyArg.c_str() + first, limit, strAESKey + 16);
		}

		Base64 codingObj;

		string strAESEncryptKey = codingObj.base64_encode((const unsigned char *) strAESKey, (AES_KEYLEN / 8));

		delete[] strAESKey;

		return strAESEncryptKey;
	}
	else
	{
		return "";
	}
}

//************************************
// Method:    SerializeData
// FullName:  UpdateClass::SerializeData
// Access:    public 
// Returns:   string
// Qualifier:
// Parameter: string DataArg
// Summary:	  Encode string into Base64Binary
//************************************
string UpdateClass::SerializeData(string DataArg)
{
	/*int iSize = DataArg.size();
	char *strTemp = new char[iSize];
	soap_s2base64(NULL,(const unsigned char *)DataArg.c_str(),strTemp,iSize);

	return strTemp;*/

	Base64 codingObj;

	DataArg = codingObj.base64_encode((const unsigned char *) DataArg.c_str(), (unsigned int)DataArg.size());

	return DataArg;
}

//************************************
// Method:    UnSerializeData
// FullName:  UpdateClass::UnSerializeData
// Access:    public 
// Returns:   string
// Qualifier:
// Parameter: string DataArg
// Summary:	  Decode Base64Binary into string
//************************************

string UpdateClass::UnSerializeData(string DataArg)
{
	/*int iSize = DataArg.size();
	char *strTemp = new char[iSize];
	soap_base642s(NULL,DataArg.c_str(),strTemp,iSize,&iSize);

	return strTemp;*/

	Base64 codingObj;

	DataArg = codingObj.base64_decode(DataArg);

	return DataArg;
}

//************************************
// Method:    SecureEncrypt
// FullName:  UpdateClass::SecureEncrypt
// Access:    public 
// Returns:   string
// Qualifier:
// Parameter: string DataArg
// Summary:	  Encrypt by public key
//************************************
string UpdateClass::SecureEncrypt(string DataArg)
{
	DataArg = this->AESEncrypt(DataArg);

	//return Cipher data
	return DataArg;
}

//************************************
// Method:    SecureDecrypt
// FullName:  UpdateClass::SecureDecrypt
// Access:    public 
// Returns:   string
// Qualifier:
// Parameter: string DataArg
// Summary:	  Decrypt by private key
//************************************
string UpdateClass::SecureDecrypt(string DataArg)
{
	DataArg = this->AESDecrypt(DataArg);

	//return Plain data
	return DataArg;
}

//************************************
// Method:		SecureDecryptFile
// FullName:	UpdateClass::SecureDecryptFile
// Access:		private 
// Returns:		std::string
// Qualifier:	
// Parameter:	string DataArg
// Description:	
//************************************
string UpdateClass::SecureDecryptFile(string DataArg)
{
	// Encrypt received data
	DataArg = this->SecureDecrypt(DataArg);

	// Split original data from pre master key
	Splitter splObj(DataArg, "+++Pre_MasterKey+++");

	if(splObj.size() == 2)
	{
		string strAESKey = this->Make16BitKey(splObj[1]);
		Crypto crpObj2;
		if (crpObj2.setAESKey(strAESKey) == SUCCESS)
		{
			if (crpObj2.AESDecrypt(splObj[0]) == SUCCESS)
			{
				DataArg = crpObj2.strDecryptMessage;
				return DataArg;
			}
		}
	}
	return "";
}

//************************************
// Method:    AESEncrypt
// FullName:  UpdateClass::AESEncrypt
// Access:    private 
// Returns:   string
// Qualifier:
// Parameter: string DataArg
// Summary:	  Encrypt data with AES Rijndael algorithm
//			  with 16Byte(128 bit) length key and
//			  32Byte(256 bit) length encryption block
//************************************
string UpdateClass::AESEncrypt( string DataArg )
{
	Crypto crpObj;
	if (crpObj.setAESKey(this->strEncryptKey) == SUCCESS)
	{
		if (crpObj.AESEncrypt(DataArg) == SUCCESS)
		{
			DataArg = crpObj.strEncryptMessage;
		}
		else
		{
			return "";
		}
	}
	else
	{
		return "";
	}

	//return Cipher data
	return DataArg;
}

//************************************
// Method:    AESDecrypt
// FullName:  UpdateClass::AESDecrypt
// Access:    private 
// Returns:   string
// Qualifier:
// Parameter: string DataArg
// Summary:	  Decrypt data with AES Rijndael algorithm
//			  with 16Byte(128 bit) length key and
//			  32Byte(256 bit) decryption block
//************************************

string UpdateClass::AESDecrypt( string DataArg )
{
	Crypto crpObj;
	if (crpObj.setAESKey(this->strEncryptKey) == SUCCESS)
	{
		if (crpObj.AESDecrypt(DataArg) == SUCCESS)
		{
			DataArg = crpObj.strDecryptMessage;
		}
		else
		{
			return "";
		}
	}
	else
	{
		return "";
	}

	//return Plain data
	return DataArg;
}

//************************************
// Method:    SecureVerify
// FullName:  UpdateClass::SecureVerify
// Access:    private 
// Returns:   string
// Qualifier:
// Parameter: string DataArg
// Summary:	  Just Verify and Get rijndael data
//************************************
string UpdateClass::SecureVerify(string DataArg)
{
	//return Plain data
	return DataArg;
}

//************************************
// Method:    replace
// FullName:  UpdateClass::replace
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: string & str
// Parameter: const string & from
// Parameter: const string & to
// Summary:	  replace a string into a string
//************************************
bool UpdateClass::Replace( string& str, const string& from, const string& to )
{
	size_t start_pos = str.find(from,0);
	while(start_pos != string::npos)
	{
		str.replace(start_pos, from.size(), to);
		start_pos = str.find(from,start_pos);
	}
	return true;
}

//************************************
// Method:    ReplaceW
// FullName:  UpdateClass::ReplaceW
// Access:    private 
// Returns:   bool
// Qualifier:
// Parameter: wstring & str
// Parameter: const wstring & from
// Parameter: const wstring & to
// Summary:	  
//************************************
bool UpdateClass::ReplaceW( wstring& str, const wstring& from, const wstring& to )
{
	size_t start_pos = str.find(from,0);
	while(start_pos != wstring::npos)
	{
		str.replace(start_pos, from.size(), to);
		start_pos = str.find(from,start_pos);
	}
	return true;
}

//************************************
// Method:    SetMACAddress
// FullName:  UpdateClass::SetMACAddress
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: string Value
// Summary:	  
//************************************
void UpdateClass::SetMACAddress(string Value)
{
	this->strMACAddress = Value;
}

//************************************
// Method:    GetMACAddress
// FullName:  UpdateClass::GetMACAddress
// Access:    public 
// Returns:   string
// Qualifier:
// Summary:	  
//************************************
string UpdateClass::GetMACAddress()
{
	return this->TrimString(this->strMACAddress);
}

//************************************
// Method:    SetHardSerialNumber
// FullName:  UpdateClass::SetHardSerialNumber
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: string Value
// Summary:	  
//************************************
void UpdateClass::SetHardSerialNumber(string Value)
{
	this->strHardSerialNo = Value;
}

//************************************
// Method:    GetHardSerialNumber
// FullName:  UpdateClass::GetHardSerialNumber
// Access:    public 
// Returns:   string
// Qualifier:
// Summary:	  
//************************************
string UpdateClass::GetHardSerialNumber()
{
	return this->TrimString(this->strHardSerialNo);
}

//************************************
// Method:    GetProxySetting
// FullName:  UpdateClass::GetProxySetting
// Access:    private 
// Returns:   void
// Qualifier:
// Summary:	  
//************************************
void UpdateClass::GetProxySetting()
{
	try{
		// Default Proxy Setting
		this->psProxyObj.ip = "";
		this->psProxyObj.port = "8080";
		this->psProxyObj.username = "";
		this->psProxyObj.password = "";

		std::wstring TableFilePath = _T("Setting.xml");

		if (this->hGetProcIDDLL == NULL) {
			return ;
		}

		SelectAllSpecialColumn Slct = (SelectAllSpecialColumn)GetProcAddress(this->hGetProcIDDLL,"SelectAllSpecialColumn");
		if (!Slct) {
			return ;
		}

		std::vector<std::wstring> result;
		int wstrProxy = 1;
		wstring whatsUp;
		whatsUp.clear();

		wstring inputColName = _T("UseProxy,AutomaticProxy,IP,PORT,Authentication,UserName,Password");

		if(Slct(TableFilePath,result,inputColName,whatsUp) == 1)
		{
			if(result.size() > 0)
			{
				WSplitter splObj(result.at(0),_T(","));
				// 1 use proxy else don't use proxy
				if (splObj[0].compare(_T("true")) == 0)
				{
					wstrProxy = 2;
					// 2 use proxy from system (IE)

					if(splObj[1].compare(_T("false")) == 0)
					{
						wstrProxy = 3;
					}
				} 
				else
				{
					wstrProxy = 1;
				}

				switch (wstrProxy)
				{
				case 1:	//No Proxy
					{
						this->psProxyObj.ip = "";
						this->psProxyObj.port = "8080";
						this->psProxyObj.username = "";
						this->psProxyObj.password = "";
						break;
					}
				case 2:	//Get system (IE) Proxy Setting
					{
						this->GetSystemProxySetting();

						if (splObj[4].compare(_T("true")) == 0)
						{
							this->psProxyObj.username.assign(splObj[5].begin(),splObj[5].end());
							this->psProxyObj.password.assign(splObj[6].begin(),splObj[6].end());
						}

						break;
					}
				case 3:	//Manual Input
					{
						this->psProxyObj.ip.assign(splObj[2].begin(),splObj[2].end());

						this->psProxyObj.port.assign(splObj[3].begin(),splObj[3].end());

						if (splObj[4].compare(_T("true")) == 0)
						{
							this->psProxyObj.username.assign(splObj[5].begin(),splObj[5].end());
							this->psProxyObj.password.assign(splObj[6].begin(),splObj[6].end());
						}

						break;
					}
				default:
					break;
				}
			}
		}

		wstring wstrproxySetting;
		if(this->psProxyObj.username != "")
		{
			wstrproxySetting.assign(this->psProxyObj.username.begin(),this->psProxyObj.username.end());
			wstrproxySetting.append(L":");
			wstrproxySetting.append(this->psProxyObj.password.begin(),this->psProxyObj.password.end());
			wstrproxySetting.append(L"@");
			if (this->psProxyObj.ip != "")
			{
				wstrproxySetting.append(this->psProxyObj.ip.begin(),this->psProxyObj.ip.end());
				wstrproxySetting.append(L":");
				wstrproxySetting.append(this->psProxyObj.port.begin(),this->psProxyObj.port.end());
			}
		}
		else 
		{
			if (this->psProxyObj.ip != "")
			{
				wstrproxySetting.assign(this->psProxyObj.ip.begin(),this->psProxyObj.ip.end());
				wstrproxySetting.append(L":");
				wstrproxySetting.append(this->psProxyObj.port.begin(),this->psProxyObj.port.end());
			}
		}

		if(wstrproxySetting != L"")
		{
			this->InsertLog(L"Set Proxy Setting", wstrproxySetting);
		}
		else
		{
			this->InsertLog(L"Set Proxy Setting", L"No Proxy");
		}
	}
	catch(...){

	}

	/*string s = "IP: " + this->psProxyObj.ip + "::::Port: " + this->psProxyObj.port + "::::User: " + this->psProxyObj.username + "::::Password: " + this->psProxyObj.password;
	this->PrintData(UPDATEMESSAGE, s);*/
}

//************************************
// Method:    SetProxyError
// FullName:  UpdateClass::SetProxyError
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: string Value
// Summary:	  
//************************************
void UpdateClass::SetProxyError(string Value)
{
	this->psProxyObj.error = Value;
}

//************************************
// Method:    GetProxyError
// FullName:  UpdateClass::GetProxyError
// Access:    public 
// Returns:   string
// Qualifier:
// Summary:	  
//************************************
string UpdateClass::GetProxyError()
{
	return this->TrimString(this->psProxyObj.error);
}

//************************************
// Method:    SetProxyIP
// FullName:  UpdateClass::SetProxyIP
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: string Value
// Summary:	  
//************************************
void UpdateClass::SetProxyIP(string Value)
{
	this->psProxyObj.ip = Value;
}

//************************************
// Method:    GetProxyIP
// FullName:  UpdateClass::GetProxyIP
// Access:    public 
// Returns:   string
// Qualifier:
// Summary:	  
//************************************
char * UpdateClass::GetProxyIP()
{
	return this->psProxyObj.ip != "" ? (char *)this->psProxyObj.ip.c_str() : NULL;
}

//************************************
// Method:    SetProxyPort
// FullName:  UpdateClass::SetProxyPort
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: string Value
// Summary:	  
//************************************
void UpdateClass::SetProxyPort(string Value)
{
	this->psProxyObj.port = Value;
}

//************************************
// Method:    GetProxyPort
// FullName:  UpdateClass::GetProxyPort
// Access:    public 
// Returns:   string
// Qualifier:
// Summary:	  
//************************************
long int UpdateClass::GetProxyPort()
{
	return atoi(this->TrimString(this->psProxyObj.port).c_str());
}

//************************************
// Method:    SetProxyUserName
// FullName:  UpdateClass::SetProxyUserName
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: string Value
// Summary:	  
//************************************
void UpdateClass::SetProxyUserName(string Value)
{
	this->psProxyObj.username = Value;
}

//************************************
// Method:    GetProxyUserName
// FullName:  UpdateClass::GetProxyUserName
// Access:    public 
// Returns:   string
// Qualifier:
// Summary:	  
//************************************
char * UpdateClass::GetProxyUserName()
{
	return this->psProxyObj.username != "" ? (char *)this->psProxyObj.username.c_str() : NULL;
}

//************************************
// Method:    SetProxyPassWord
// FullName:  UpdateClass::SetProxyPassWord
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: string Value
// Summary:	  
//************************************
void UpdateClass::SetProxyPassWord(string Value)
{
	this->psProxyObj.password = Value;
}

//************************************
// Method:    GetProxyPassWord
// FullName:  UpdateClass::GetProxyPassWord
// Access:    public 
// Returns:   string
// Qualifier:
// Summary:	  
//************************************
char * UpdateClass::GetProxyPassWord()
{
	return this->psProxyObj.password != "" ? (char *)this->psProxyObj.password.c_str() : NULL;;
}

//************************************
// Method:    GetSystemMACAddress
// FullName:  UpdateClass::GetSystemMACAddress
// Access:    public 
// Returns:   void
// Qualifier:
// Summary:	  Get MACAddress from system
//************************************
void UpdateClass::GetSystemMACAddress()
{
	// Get the buffer length required for IP_ADAPTER_INFO.
	ULONG BufferLength = 0;
	BYTE* pBuffer = 0;
	if( ERROR_BUFFER_OVERFLOW == GetAdaptersInfo( 0, &BufferLength ))
	{
		// Now the BufferLength contain the required buffer length.
		// Allocate necessary buffer.
		pBuffer = new BYTE[ BufferLength ];
	}
	else
	{
		// Error occurred. handle it accordingly.
	}

	// Get the Adapter Information.
	PIP_ADAPTER_INFO pAdapterInfo =
		reinterpret_cast<PIP_ADAPTER_INFO>(pBuffer);
	GetAdaptersInfo( pAdapterInfo, &BufferLength );

	// Iterate the network adapters and print their MAC address.
	if( pAdapterInfo )
	{
		char buffer[18];
		// Assuming pAdapterInfo->AddressLength is 6.
		sprintf_s(buffer, 18, "%02x:%02x:%02x:%02x:%02x:%02x",
			pAdapterInfo->Address[0],
			pAdapterInfo->Address[1],
			pAdapterInfo->Address[2],
			pAdapterInfo->Address[3],
			pAdapterInfo->Address[4],
			pAdapterInfo->Address[5]);
		this->strMACAddress = buffer;
		pAdapterInfo = pAdapterInfo->Next;
	}
	delete [] pBuffer;
}

//************************************
// Method:    GetHardDriveSerial
// FullName:  UpdateClass::GetHardDriveSerial
// Access:    public 
// Returns:   void
// Qualifier:
// Summary:	  Get hard serial number from system
//************************************
void UpdateClass::GetHardDriveSerial()
{
	// Get Hard Drive Serial Number
	std::vector<char> hwId;
	MasterHardDiskSerial diskSerial;

	diskSerial.GetSerialNo(hwId);
	if (hwId.empty())
	{
		this->strHardSerialNo = "";
	}
	this->strHardSerialNo.assign(hwId.begin(),hwId.end());
}

//************************************
// Method:    GetSystemProxySetting
// FullName:  UpdateClass::GetSystemProxySetting
// Access:    public 
// Returns:   void
// Qualifier:
// Summary:	  Get System proxy from system
//************************************
void UpdateClass::GetSystemProxySetting()
{
	WINHTTP_CURRENT_USER_IE_PROXY_CONFIG MyProxyConfig;

	this->psProxyObj.error = "";
	if(!WinHttpGetIEProxyConfigForCurrentUser(&MyProxyConfig))
	{
		//check the error DWORD Err = GetLastError(); 
		DWORD Err = GetLastError();
		switch (Err) 
		{
		case ERROR_FILE_NOT_FOUND: 
			this->psProxyObj.error = "The error is ERROR_FILE_NOT_FOUND"; 
			break; 
		case ERROR_WINHTTP_INTERNAL_ERROR:
			this->psProxyObj.error = "ERROR_WINHTTP_INTERNAL_ERROR"; 
			break; 
		case ERROR_NOT_ENOUGH_MEMORY:
			this->psProxyObj.error = "ERROR_NOT_ENOUGH_MEMORY"; 
			break; 
		default: ;//this->PrintData(UPDATEMESSAGE, "Look up error in header file.\n"); 
		}//end switch 
	}//end if 
	else 
	{ 
		if(NULL != MyProxyConfig.lpszAutoConfigUrl)
		{ 
			GlobalFree(MyProxyConfig.lpszAutoConfigUrl);
		} 
		if(NULL != MyProxyConfig.lpszProxy) 
		{
			string buffer =  CW2A(MyProxyConfig.lpszProxy);
			Splitter split(buffer,":");
			this->psProxyObj.ip = split[0];
			this->psProxyObj.port = split[1];
			this->psProxyObj.username = "";
			this->psProxyObj.password = "";
			GlobalFree(MyProxyConfig.lpszProxy);
		}
		if(NULL != MyProxyConfig.lpszProxyBypass) 
		{
			GlobalFree(MyProxyConfig.lpszProxyBypass); 
		}
	}//end else
}

//************************************
// Method:    WriteFileData
// FullName:  UpdateClass::WriteFileData
// Access:    public 
// Returns:   void
// Qualifier:
// Summary:	  Write last update log into file
//************************************
bool UpdateClass::WriteFileData( string FilePath, string Data )
{
	this->tStartDate = time(0);
	string strs = to_string(this->tStartDate);
	size_t iSize = strlen(Data.c_str());
	FILE *myFile;
	fopen_s(&myFile,FilePath.c_str(),"wb");
	if (myFile)
	{
		fwrite(Data.c_str(),sizeof(char),iSize,myFile);

		/*fwrite(this->strPublicKey.c_str(),sizeof(char),this->strPublicKey.size(),myFile);
		fwrite("\n---avu---\n",sizeof(char),11,myFile);
		fwrite(this->strSessionCode.c_str(),sizeof(char),this->strSessionCode.size(),myFile);
		fwrite("\n---avu---\n",sizeof(char),11,myFile);
		fwrite(this->strEncryptKey.c_str(),sizeof(char),this->strEncryptKey.size(),myFile);
		fwrite("\n---avu---\n",sizeof(char),11,myFile);
		fwrite(strs.c_str(),sizeof(char),strs.size(),myFile);*/

		fclose(myFile);

		return true;
	}
	return false;
}

//************************************
// Method:    SetFinishedUpdate
// FullName:  UpdateClass::SetFinishedUpdate
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: bool Value
// Summary:	  
//************************************
void UpdateClass::SetFinishedUpdate( bool Value )
{
	this->bFinishedUpdate = Value;
}

//************************************
// Method:    GetFinishedUpdate
// FullName:  UpdateClass::GetFinishedUpdate
// Access:    public 
// Returns:   bool
// Qualifier:
// Summary:	  
//************************************
bool UpdateClass::GetFinishedUpdate()
{
	return this->bFinishedUpdate;
}

//************************************
// Method:    SetUpdateProperties
// FullName:  UpdateClass::SetUpdateProperties
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: string UpdateFileInfoAddress
// Summary:	  Check update file properties format and read properties for start an update
//************************************
bool UpdateClass::SetUpdateProperties( string UpdateFileInfoAddress )
{
	if (!UpdateFileInfoAddress.empty())
	{
		Splitter splobj(UpdateFileInfoAddress, "*");
		if (splobj.size() > 1)
		{
			this->iIdNumber = atoi(splobj[1].c_str());

			// Get Last Update File name From Update Log XML File
			if(this->GetLastUpdateFileName())
			{
				this->GetProxySetting();

				this->strUserKey = "wwwweeeewwwweeee";

				// It is a fake Serial number For testing
				this->strHardSerialNo = "WD-WMC4M0238191";
				this->strServerAddress = this->WString2String(SERVERADDRESS);
				this->InsertLog(L"Set Update Source Address", SERVERADDRESS);

				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}

//************************************
// Method:    ReadFileData
// FullName:  UpdateClass::ReadFileData
// Access:    private 
// Returns:   string
// Qualifier:
// Parameter: string Address
// Summary:	  read file data
//************************************
string UpdateClass::ReadFileData( string Address, size_t *FSize )
{
	FILE *fFile;
	try
	{
		fopen_s(&fFile,Address.c_str(),"rb");
		if (fFile)
		{
			fseek(fFile,0,SEEK_END);
			*FSize = ftell(fFile);
			fseek(fFile,0,SEEK_SET);
			if (*FSize>0)
			{
				char *cFileData = new char[*FSize + 1];
				fread_s(cFileData,*FSize,sizeof(char),*FSize,fFile);
				cFileData[*FSize] = 0;
				string tmp(cFileData);
				fclose(fFile);

				delete[] cFileData;

				return tmp;
			}
			fclose(fFile);
			return "";
		}
		return "";
	}
	catch (exception* e)
	{
		this->strErroCode = e->what();
	}
	return "";
}

//************************************
// Method:    SetServerAddress
// FullName:  UpdateClass::SetServerAddress
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: string Value
// Summary:	  
//************************************
void UpdateClass::SetServerAddress( string Value )
{
	this->strServerAddress = Value;
}

//************************************
// Method:    GetServerAddress
// FullName:  UpdateClass::GetServerAddress
// Access:    public 
// Returns:   string
// Qualifier:
// Summary:	  
//************************************
string UpdateClass::GetServerAddress()
{
	return this->TrimString(this->strServerAddress);
}

//************************************
// Method:    TrimString
// FullName:  UpdateClass::TrimString
// Access:    public 
// Returns:   string
// Qualifier:
// Parameter: string Input
// Summary:	  Trim begin and end of a string
//************************************
string UpdateClass::TrimString( string Input )
{
	size_t iSize = Input.size();
	if (iSize > 0)
	{
		long int iIndex = 0;
		while (Input[iIndex++] == ' ' && iIndex < iSize);
		iSize--;
		while (Input[iSize] == ' ' && iSize > -1)iSize--;
		if(iSize >= iIndex)
		{
			return Input.substr(iIndex - 1,iSize + 1);
		}
	}
	return "";
}

//************************************
// Method:    FileExist
// FullName:  UpdateClass::FileExist
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: string FilePath
// Summary:	  file exist or no
//************************************
bool UpdateClass::FileExist( string FilePath )
{
	FILE *file;
	fopen_s(&file,FilePath.c_str(), "r");
	if (file)
	{
		fclose(file);
		return true;
	} 
	else
	{
		return false;
	}
}

//************************************
// Method:    GeneratePassword
// FullName:  UpdateClass::GeneratePassword
// Access:    private 
// Returns:   string
// Qualifier:
// Parameter: long int Len
// Summary:	  Make a key with any length
//************************************
std::string UpdateClass::GeneratePassword(size_t Length)
{
	string strTemp;
	//create a random password here
	char charArray[] = {'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F', 'g', 'G', 'h', 'H', 'i', 'I', 'j', 'J', 'k', 'K', 'l', 'L', 'm', 'M', 'n', 'N', 'o', 'O', 'p', 'P', 'q', 'Q', 'r', 'R', 's', 'S',	't', 'T', 'u', 'U', 'v', 'V', 'w', 'W', 'x', 'X', 'y', 'Y', 'z', 'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '?', '<', '>', '.', ',', ';', '-', '@', '!', '#', '$', '%', '^', '&', '*', '(', ')'};
	long int max_chars = sizeof(charArray) - 1;
	srand((unsigned int)time(0) * 1000000);

	strTemp.clear();

	for (long int i = 0; i < Length; i++)
	{
		strTemp.push_back(charArray[rand() % max_chars]);
	}

	return strTemp;
}

//************************************
// Method:    GetPreMasterKey
// FullName:  UpdateClass::GetPreMasterKey
// Access:    public 
// Returns:   string
// Qualifier:
// Summary:	  
//************************************
string UpdateClass::GetPreMasterKey()
{
	return this->TrimString(this->strPreMasterKey);
}

//************************************
// Method:    SetPreMasterKey
// FullName:  UpdateClass::SetPreMasterKey
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: string Value
// Summary:	  
//************************************
void UpdateClass::SetPreMasterKey( string Value )
{
	this->strPreMasterKey = Value;
}

//************************************
// Method:    WString2String
// FullName:  UpdateClass::WString2String
// Access:    private 
// Returns:   string
// Qualifier:
// Parameter: wchar_t * InputArg
// Summary:	  Convert WideChar string into NarrowChar string
//************************************
string UpdateClass::WString2String( wstring InputArg )
{
	char *strWideChar = new char[InputArg.size() + 1];

	wcstombs_s(NULL, strWideChar, InputArg.size() + 1, InputArg.c_str(), InputArg.size());

	return strWideChar;
}

//************************************
// Method:    String2WString
// FullName:  UpdateClass::String2WString
// Access:    private 
// Returns:   string
// Qualifier:
// Parameter: wchar_t * InputArg
// Summary:	  Convert NarrowChar string into WideChar string
//************************************
std::wstring UpdateClass::String2WString( string InputArg )
{
	wchar_t * strNarrowChar = new wchar_t[InputArg.size() + 1];

	mbstowcs_s(NULL, strNarrowChar, InputArg.size() + 1, InputArg.c_str(), InputArg.size());

	return strNarrowChar;
}

//************************************
// Method:    DeleteExistFile
// FullName:  UpdateClass::DeleteExistFile
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: string Path
// Summary:	  Delete Existing File (temporary Files) From Disk
//************************************
bool UpdateClass::DeleteExistFile( string Path )
{
	// if File exist delete it
	if (this->FileExist(Path))
	{
		// Delete File
		return !(remove(Path.c_str()));
	}
	return false;
}

//************************************
// Method:    PrintData
// FullName:  UpdateClass::PrintData
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: string Output
// Summary:	  
//************************************
void UpdateClass::PrintData(long int OutputType, string PipMessage )
{
	if (PIPTYPE)
	{
		this->Replace(PipMessage,"\n","");

		// Prepare message sending to PipMethod
		string msg;

		if (OutputType == UPDATEMESSAGE)
		{
			msg = "Log*" + this->IntToString(this->iIdNumber) + "*" + PipMessage;
		} 
		else
		{
			msg = "Path*" + this->IntToString(this->iIdNumber) + "*" + PipMessage;
		}

		// Send Message to PipMethod
		wstring tmp = String2WString((char *)msg.c_str());

		// load DLL library...
		HINSTANCE hGetProcIDDLL = LoadLibrary(L"SendMsgToPipe.dll");

		if (hGetProcIDDLL != INVALID_HANDLE_VALUE)
		{
			// define function from DLL file...
			PipeCout = (funci)GetProcAddress(hGetProcIDDLL, "SendCMD");
		}

		PipeCout((wchar_t *)tmp.c_str());
		FreeLibrary(hGetProcIDDLL);
	} 
	else
	{
		cout << PipMessage.c_str();
	}

}

//************************************
// Method:    IntToString
// FullName:  UpdateClass::IntToString
// Access:    private 
// Returns:   string
// Qualifier:
// Parameter: long int IntData
// Summary:	  
//************************************
std::string UpdateClass::IntToString(size_t IntData)
{
	// Temporary string
	string result = static_cast<ostringstream*>( &(ostringstream() << IntData) )->str();
	return result;
}

//************************************
// Method:    IntToStringW
// FullName:  UpdateClass::IntToStringW
// Access:    private 
// Returns:   std::wstring
// Qualifier:
// Parameter: long int IntData
// Summary:	  
//************************************
std::wstring UpdateClass::IntToStringW(size_t IntData)
{
	// Temporary string
	wstring result = static_cast<wostringstream*>( &(wostringstream() << IntData) )->str();
	return result;
}

//************************************
// Method:    CalculateUpdateTimePeriod
// FullName:  UpdateClass::CalculateUpdateTimePeriod
// Access:    private 
// Returns:   long int
// Qualifier:
// Summary:	  Get Summation of size of all files
//				and calculate current percent of downloaded file
//************************************
double UpdateClass::CalculateUpdateTimePeriod(double CurrentPercent, size_t CurrentFileSize)
{
	return (CurrentPercent + (((double)CurrentFileSize * 98) / (double)this->iFileSizeSum));
}

//************************************
// Method:    GetSetupAddressFromRegistry
// FullName:  UpdateClass::GetSetupAddressFromRegistry
// Access:    private 
// Returns:   std::wstring
// Qualifier:
// Summary:	  
//************************************
std::wstring UpdateClass::GetSetupAddressFromRegistry()
{
	HKEY hkey,hkRootPath=HKEY_LOCAL_MACHINE;

	wstring wstrKeyPath=L"SoftWare\\AV",wstrKeyName=L"";
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
}

//************************************
// Method:    FindAllMaching
// FullName:  UpdateClass::FindAllMaching
// Access:    private 
// Returns:   long int
// Qualifier:
// Parameter: string Input
// Summary:	  
//************************************
long int UpdateClass::FindAllMaching( string Input ,string MachStr)
{
	long int counter = 0;
	size_t start_pos = 0;
	do		
	{
		start_pos = Input.find(MachStr,start_pos);
		if(start_pos != string::npos)
		{
			counter++;
			start_pos++;
		}
	}while(start_pos != string::npos);
	return counter;
}

//************************************
// Method:    InsertLog
// FullName:  UpdateClass::InsertLog
// Access:    private 
// Returns:   bool
// Qualifier:
// Parameter: wstring wstrLog
// Summary:	  
//************************************
bool UpdateClass::InsertLog( wstring wstrLogObject, wstring wstrLogEvent )
{
	wstring whatsup;
	whatsup.clear();

	timeStruct stcTime;

	this->GetLocalCurrentTime(stcTime);

	wstring strDateTime = this->IntToStringW(stcTime.iYear) + L"/" + this->IntToStringW(stcTime.iMonth) + L"/" + this->IntToStringW(stcTime.iDay) + L"   " + this->IntToStringW(stcTime.iHoure) + L":" + this->IntToStringW(stcTime.iMinut) + L":" + this->IntToStringW(stcTime.iSecend);

	wstring wstrLog = L"DateTime=" + strDateTime + L",Object=" + wstrLogObject + L",Event=" + wstrLogEvent;

	std::wstring TableFilePath = _T("Update.xml");

	if (this->hGetProcIDDLL == NULL) {
		return false;
	}

	InsertRecordOldVersion insertResult=(InsertRecordOldVersion)GetProcAddress(this->hGetProcIDDLL,"InsertRecordOldVersion");
	if (!insertResult)
	{
		return false;
	}

	int resultOfOperation = 0;
	do{
		resultOfOperation = insertResult(TableFilePath,wstrLog,whatsup);
		if(resultOfOperation==1){
			break;
		}
		else if (resultOfOperation<-1){
			return false;
		}
		else{

		}
	}while (resultOfOperation==-1);

	return true;
}

//************************************
// Method:		GetLastUpdateFileName
// FullName:	UpdateClass::GetLastUpdateFileName
// Access:		private 
// Returns:		std::string
// Qualifier:	
// Description:	Get Last Update File Name from Update Log XML File
//************************************
bool UpdateClass::GetLastUpdateFileName()
{
	// Result variable
	string strResult = "";

	// Path of Update XML File
	wstring xmlPath;
	xmlPath.clear();
	xmlPath=L"Update.xml";

	// Result of Operation
	wstring whatsup;
	whatsup.clear();

	// List Of columns that selected
	vector <wstring> resultSetRecord;
	resultSetRecord.clear();
	resultSetRecord.shrink_to_fit();

	// In this Column should do selection
	wstring strColumnList = _T("Event");

	// Condition is in this Column
	wstring strCondition = _T("Object=Update File Name");

	if (this->hGetProcIDDLL == NULL) {
		return false;
	}

	// Select Function
	SelectAllOldSpecialColumnConditional selectResult=(SelectAllOldSpecialColumnConditional)GetProcAddress(this->hGetProcIDDLL,"SelectAllOldSpecialColumnConditional");

	if(selectResult)
	{
		// Execute Query
		selectResult(xmlPath, resultSetRecord, strColumnList, strCondition, whatsup);

		vector<string> strResult;
		string strStr;

		if (resultSetRecord.size() > 0)
		{
			this->strLastFileUpdate.assign(resultSetRecord.at(resultSetRecord.size() - 1).begin(),resultSetRecord.at(resultSetRecord.size() - 1).end());
		} 
		else
		{
			return false;
		}

	}

	return (this->strLastFileUpdate != "");
}

//************************************
// Method:		CalcFilesSizeSummation
// FullName:	UpdateClass::CalcFilesSizeSummation
// Access:		private 
// Returns:		size_t
// Qualifier:	
// Description:	Calculate Sumatin of all download files
//************************************
void UpdateClass::CalcFilesSizeSummation()
{
	// Summation Of all files
	this->iFileSizeSum = 0;
	for (int i = 0; i < this->vFileList.size(); i++)
	{
		this->iFileSizeSum += this->vFileList[i].FileSize;
	}
}

//************************************
// Method:    GetCurrentTimeFunction
// FullName:  UpdateClass::GetCurrentTimeFunction
// Access:    private 
// Returns:   bool
// Qualifier:
// Parameter: timeStruct & stcDateTime
// Summary:	  
//************************************
bool UpdateClass::GetLocalCurrentTime( timeStruct &stcDateTime )
{
	time_t rawtime;
	struct tm * timeinfo = new struct tm;

	time ( &rawtime ); //get current date time
	if(localtime_s(timeinfo, &rawtime ) == NULL)
	{
		stcDateTime.iSecend = timeinfo->tm_sec;
		stcDateTime.iMinut = timeinfo->tm_min;
		stcDateTime.iHoure = timeinfo->tm_hour;
		stcDateTime.iDay = timeinfo->tm_mday;
		stcDateTime.iMonth = timeinfo->tm_mon;
		stcDateTime.iYear = timeinfo->tm_year + 1900;
		return true;
	}

	delete timeinfo;

	return false;
}

//************************************
// Method:		GenerateFileList
// FullName:	UpdateClass::GenerateFileList
// Access:		private 
// Returns:		bool
// Qualifier:	
// Parameter:	string Result
// Description:	
//************************************
bool UpdateClass::GenerateFileList(string Result)
{
	FileInfo tmpFlI;
	Splitter splObj(Result,"\n");
	for(int i=0; i< splObj.size();i++)
	{
		Splitter splobj2(splObj[i], ":");
		if (splobj2.size() == 3)
		{
			tmpFlI.FolderName = splobj2[0];
			tmpFlI.FileName = splobj2[1];
			tmpFlI.FileSize = atoi(splobj2[2].c_str());
			this->vFileList.push_back(tmpFlI);
		} 
		else
		{
			return false;
		}

	}
	return (splObj.size() > 0);
}

//************************************
// Method:    DoUpdate
// FullName:  UpdateClass::DoUpdate
// Access:    public 
// Returns:   void
// Qualifier:
// Summary:	  
//************************************
void UpdateClass::DoUpdate()
{
	if (this->OK && *(this->iServiceStatus) != SERVICESTATUS)
	{
		this->PrintData(UPDATEMESSAGE, "\nUpdate Started\n");
		// Print Percent of Progress
		this->PrintData(UPDATEPERCENT, "\n2\n");

		long int iConnectionCounter = 0;

		// Percent counter
		double iPeriodTime = 0;

		bool logOk = false;

		bool bIsUpToDate = false;
		do{
			//Server variables
			ns1__SessionInfo sessionResult;

			UpdateProxy clientObj;
			clientObj.send_timeout = SEND_TIMEOUT;
			clientObj.recv_timeout = RECV_TIMEOUT;
			clientObj.connect_timeout = CONNECT_TIMEOUT;
			clientObj.accept_timeout = ACCEPT_TIMEOUT;

			// Set server address
			clientObj.soap_endpoint = this->strServerAddress.c_str();

			// Set proxy setting
			clientObj.proxy_host = this->GetProxyIP();
			clientObj.proxy_port = this->GetProxyPort();
			clientObj.proxy_userid = this->GetProxyUserName();
			clientObj.proxy_passwd = this->GetProxyPassWord();

			// Request for new Update public key and session code
			Sleep(SLEEPTIME);
			clientObj.RequestForUpdate(sessionResult);

			if (*(this->iServiceStatus) != SERVICESTATUS)
			{
				if (clientObj.error)
				{
					this->PrintData(UPDATEMESSAGE, "\nFault (Expect - The request contains an invalid SOAP body): \n");
					clientObj.soap_stream_fault(std::cerr);
					iConnectionCounter ++;
				}
				else
				{
					if (atoi(sessionResult.Error.ErrorNumber.c_str()) == 0)
					{
						this->InsertLog(L"Connection Done",L"OK");
						this->PrintData(UPDATEMESSAGE,"\nConnection Done\n");

						this->strPublicKey = this->UnSerializeData(sessionResult.PublicKey);
						this->strSessionCode = this->UnSerializeData(sessionResult.SessionCode);

						// construct and initialize request structural variables
						ns1__UpdateArguments updateArgs;

						// Generate PremasterKey with 32 byte length
						this->strPreMasterKey = this->GeneratePassword(PREMASTERKEYLENGTH);
						this->strEncryptKey = this->Make16BitKey(this->strPreMasterKey);

						// Just base64 decode
						updateArgs.SessionCode = this->SerializeData(this->strSessionCode); //Not Encrypted
						updateArgs.PreMasterKey = this->SerializeData(this->strPreMasterKey);

						// Encrypt
						updateArgs.UserKey = this->SecureEncrypt(this->strUserKey);
						updateArgs.HardSerial = this->SecureEncrypt(this->strHardSerialNo);

						updateArgs.FilesList.FileName = this->SecureEncrypt(this->strLastFileUpdate);

						ns1__UpdateFilesList filesListResult;

						clientObj.send_timeout = SEND_TIMEOUT;
						clientObj.recv_timeout = RECV_TIMEOUT;
						clientObj.connect_timeout = CONNECT_TIMEOUT;
						clientObj.accept_timeout = ACCEPT_TIMEOUT;

						// Set server address
						clientObj.soap_endpoint = this->strServerAddress.c_str();

						// Set proxy setting
						clientObj.proxy_host = this->GetProxyIP();
						clientObj.proxy_port = this->GetProxyPort();
						clientObj.proxy_userid = this->GetProxyUserName();
						clientObj.proxy_passwd = this->GetProxyPassWord();

						// Send User Info and pre master key to server and Get Update File List
						clientObj.MakeSecureConnection(&updateArgs, filesListResult);

						if (clientObj.error)
						{
							this->PrintData(UPDATEMESSAGE, "\nFault (Expect - The request contains an invalid SOAP body): \n");
							clientObj.soap_stream_fault(std::cerr);
						}
						else
						{
							// Every things is OK
							if (atoi(filesListResult.Error.ErrorNumber.c_str()) == 0)
							{
								// List of File is not empty
								if (filesListResult.FilesList.size() > 0)
								{
									iConnectionCounter = 0;

									FileInfo fiTemp;

									this->iFileSizeSum = 0;
									for (int i=0; i<filesListResult.FilesList.size(); i++)
									{
										// Select Just files are newer than last update file
										fiTemp.FileName = this->SecureDecrypt(filesListResult.FilesList[i].FileName);
										fiTemp.FileSize = atoi(this->SecureDecrypt(filesListResult.FilesList[i].FileSize).c_str());

										this->iFileSizeSum +=fiTemp.FileSize;
										this->vFileList.push_back(fiTemp);
									}

									// Calculate download Percent Rate
									iPeriodTime = this->CalculateUpdateTimePeriod(2, 0);

									// Ok while sequential update done
									bool updateOk = true;

									//long int iSleepCounter = 0;

									if (*(this->iServiceStatus) != SERVICESTATUS)
									{
										updateOk = true;	//Ok while sequential update done

										long int i = 0;
										// Request to download one by one files
										for (i = 0;*(this->iServiceStatus) != SERVICESTATUS && i < this->vFileList.size() && updateOk; i++)
										{
											updateArgs.FilesList.FileName = this->SecureEncrypt(this->vFileList[i].FileName);

											updateOk = false;

											//Try more times a request and go to next update request else quit
											for (long int iThird=0;*(this->iServiceStatus) != SERVICESTATUS && iThird < THIRDREPEAT;iThird++)
											{
												string updateResult;
												ns1__UpdateResponse updateResponse;

												Sleep(SLEEPTIME);

												clientObj.reset();

												clientObj.send_timeout = SEND_TIMEOUT;
												clientObj.recv_timeout = RECV_TIMEOUT;
												clientObj.connect_timeout = CONNECT_TIMEOUT;
												clientObj.accept_timeout = ACCEPT_TIMEOUT;

												// Set server address
												clientObj.soap_endpoint = this->strServerAddress.c_str();

												// Set proxy setting
												clientObj.proxy_host = this->GetProxyIP();
												clientObj.proxy_port = this->GetProxyPort();
												clientObj.proxy_userid = this->GetProxyUserName();
												clientObj.proxy_passwd = this->GetProxyPassWord();

												clientObj.DoUpdate(&updateArgs, updateResponse);

												if (*(this->iServiceStatus) != SERVICESTATUS)
												{
													if (clientObj.error)
													{
														/*this->PrintData(UPDATEMESSAGE, "\nFault (Expect - The request contains an invalid SOAP body): \n");
														clientObj4.soap_stream_fault(std::cerr);*/
													}
													else
													{
														if (atoi(updateResponse.Error.ErrorNumber.c_str()) == 0)
														{
															if (!updateResponse.FileContent.empty())
															{
																// Get result of request
																updateResult = this->SecureDecryptFile(updateResponse.FileContent);

																if (!updateResult.empty())
																{
																	//if(ecObj->Update_HMDB((uchar *)updateResult.c_str()) == true)
																	if(this->WriteFileData("F:\\APA\\Projects\\New Project\\Update\\Client\\C++\\TempDir\\files\\" + this->vFileList[i].FileName, updateResult))
																	{
																		wstring tmpStr(this->vFileList[i].FileName.begin(),this->vFileList[i].FileName.end());
																		this->InsertLog(L"Update File Name",tmpStr);
																		this->PrintData(UPDATEMESSAGE,"\nUpdate File Name: " + this->vFileList[i].FileName + "\n");

																		// Print Percent of Progress 
																		iPeriodTime = this->CalculateUpdateTimePeriod(iPeriodTime, this->vFileList[i].FileSize);
																		this->PrintData(UPDATEPERCENT, "\n" + this->IntToString((long int)iPeriodTime) + "\n");

																		// update done successfully
																		iThird = THIRDREPEAT;

																		updateOk = true;	//Update request done successfully

																		break;
																	}
																}
															}
														}
													}
												}
											}
										}
										// All Update files received Successfully
										if (i == this->vFileList.size())
										{
											this->bFinishedUpdate = true;
										}
									}
								} 
								else
								{
									this->PrintData(UPDATEMESSAGE, "\nSoftware is Up to Date \n");
									this->InsertLog(L"Software is Up to Date",L"Updated");
									this->bFinishedUpdate = true;
								}

								ns1__ErrorInfo updateFinished;

								clientObj.send_timeout = SEND_TIMEOUT;
								clientObj.recv_timeout = RECV_TIMEOUT;
								clientObj.connect_timeout = CONNECT_TIMEOUT;
								clientObj.accept_timeout = ACCEPT_TIMEOUT;

								// Set server address
								clientObj.soap_endpoint = this->strServerAddress.c_str();

								// Set proxy setting
								clientObj.proxy_host = this->GetProxyIP();
								clientObj.proxy_port = this->GetProxyPort();
								clientObj.proxy_userid = this->GetProxyUserName();
								clientObj.proxy_passwd = this->GetProxyPassWord();

								Sleep(SLEEPTIME);
								clientObj.EndUpdate(&updateArgs,updateFinished);

								if (clientObj.error)
								{
									/*this->PrintData(UPDATEMESSAGE, "\nFault (Expect - The request contains an invalid SOAP body): \n");
									clientObj6.soap_stream_fault(std::cerr);*/
								}
								else
								{
									if (atoi(updateFinished.ErrorNumber.c_str()) == 0)
									{
										iConnectionCounter = FIRSTREPEAT;
									}
								}
							}
						}
					}
				}
			}
		} while(*(this->iServiceStatus) != SERVICESTATUS && (iConnectionCounter < FIRSTREPEAT) && !bIsUpToDate && this->bFinishedUpdate == false);

		if (*(this->iServiceStatus) != SERVICESTATUS)
		{
			if (this->bFinishedUpdate == true)
			{
				this->PrintData(UPDATEMESSAGE, "\nUpdate Finished Successfully \n");
				this->InsertLog(L"Update Finished Successfully",L"OK");
			} 
			else
			{
				this->PrintData(UPDATEMESSAGE, "\nAn Error Occurred in Update duration." );
				this->PrintData(UPDATEMESSAGE, "\nCheck your internet connection and try again later.\n" );

				this->InsertLog(L"An Error Occurred in Update duration", L"OK");
			}
		} 
		else
		{
			this->PrintData(UPDATEMESSAGE, "\nUpdate Canceled by user\n" );
			this->InsertLog(L"Update Canceled by user", L"OK");
		}
	}
	else if (*(this->iServiceStatus) != SERVICESTATUS)
	{
		this->InsertLog(L"Some Error Occurred in read update file info", L"OK");
		this->PrintData(UPDATEMESSAGE, "\nSome Error Occurred in read update file info ...\n");
	}
	else
	{
		this->PrintData(UPDATEMESSAGE, "\nUpdate Canceled by user\n" );
		this->InsertLog(L"Update Canceled by user", L"OK");
	}

	// Print Percent of Progress
	this->PrintData(UPDATEPERCENT, "\n100\n");

	this->PrintData(UPDATEMESSAGE,"Finish");
	this->InsertLog(L"Finish",L"OK");
}