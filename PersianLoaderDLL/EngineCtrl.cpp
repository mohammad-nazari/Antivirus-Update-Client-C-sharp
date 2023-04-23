#include "EngineCtrl.h"

EngineCtrl::EngineCtrl(){
	HMDB=new MD5Sort(HMDB_STRUCT_PATH);
	NDB=new AwesomeAhoCorasick();
	APIH=new APIHeuristic();
	HMDB_Loaded=new bool();
	NDB_Loaded=new bool();
	APIH_Loaded=new bool();
	*HMDB_Loaded=false;
	*NDB_Loaded=false;
	*APIH_Loaded=false;
}
EngineCtrl::~EngineCtrl(){
	delete(HMDB);
	delete(NDB);
	delete(APIH);
	delete(HMDB_Loaded);
	delete(NDB_Loaded);
	delete(APIH_Loaded);
}
uchar* EngineCtrl::CharArrayCutNextLine(uchar* CharArray){
	ulong i=0,j;
	for(;CharArray[i]!=0 && CharArray[i]!='\n' && CharArray[i]!='\r';i++);
	j=i;
	for(;CharArray[i]!=0 && (CharArray[i]=='\n' || CharArray[i]=='\r');i++);
	CharArray[j]=0;
	return CharArray+i;
}
bool EngineCtrl::Load_NDB(){
	if(!*NDB_Loaded){
		if(NDB->LoadFromFile(NDB_STRUCT_PATH))
		{
			*NDB_Loaded=true;
			return true;
		}
		else
			return false;
	}
}
bool EngineCtrl::Load_HMDB(){
	if(!*HMDB_Loaded){
		if(HMDB->LoadFromFile())
		{
			*HMDB_Loaded=true;
			return true;
		}else
			return false;
	}
	return true;
}
bool EngineCtrl::Load_APIH(){
	if(!*APIH_Loaded){
		if(APIH->LoadFromFile(APIH_STRUCT_PATH))
		{
			*APIH_Loaded=true;
			return true;
		}else
			return false;
	}
	return true;
}
bool EngineCtrl::Unload_NDB(){
	if(*NDB_Loaded){
		NDB->FreeData();
		*NDB_Loaded=false;
	}
	return true;
}
bool EngineCtrl::Unload_HMDB(){
	if(*HMDB_Loaded){
		HMDB->FreeData();
		*HMDB_Loaded=false;
	}
	return true;
}
bool EngineCtrl::Unload_APIH(){
	if(*APIH_Loaded){
		APIH->FreeData();
		*APIH_Loaded=false;
	}
	return true;
}
bool EngineCtrl::Update_HMDB(uchar *DataContentPointer){
	if(!Unload_HMDB())
		return false;
	//if(!HMDB_LOAD())
	//	return false;
	register ui64 FSize,NoSig;
	register uchar *Line, *DataContent;
	register char *param1,*param2,*param3;
	//Reading First Line
	Line=DataContentPointer;
	DataContent=CharArrayCutNextLine(Line);
	//Reading Three Parameters
	param1=strtok((char*)Line,":");
	param2=strtok(NULL,":");
	param3=strtok(NULL,":");
	//NUMBER OF UPDATES
	NoSig=atol(param2);
	while(NoSig>0){
		NoSig--;
		//Reading a Line
		Line=DataContent;
		DataContent=CharArrayCutNextLine(Line);
		//Casting a Line into Signature Entity
		SignatureEntity tmp((char*)Line,SigDataType::HMDB_SIGNATURE);
		//Inserting into SigDB
		HMDB->AddSignature(tmp.HDBAttributes());
	}
	if(!(HMDB->EndOfInput()))
		return false;
	return true;
}
bool EngineCtrl::Update_NDB(uchar *DataContentPointer){
	if(!Unload_NDB())
		return false;
	//if(!Load_NDB())
		//return false;
	register ui64 FSize,NoSig;
	register uchar *Line, *DataContent;
	register char *param1,*param2,*param3;
	//Reading First Line
	Line=DataContentPointer;
	DataContent=CharArrayCutNextLine(Line);
	//Reading Three Parameters
	param1=strtok((char*)Line,":");
	param2=strtok(NULL,":");
	param3=strtok(NULL,":");
	//NUMBER OF UPDATES
	NoSig=atoi(param2);
	while(NoSig>0){
		NoSig--;
		//Reading a Line
		Line=DataContent;
		DataContent=CharArrayCutNextLine(Line);
		if(!DataContent)
			break;
		//Casting a Line into Signature Entity
		SignatureEntity tmp((char*)Line,SigDataType::NDB_SIGNATURE);
		//Inserting into SigDB
		//cout<<NoSig<<endl;
		NDB->AddSignature(tmp.NDBAttributes());
	}
 	NDB->EndOfSignature();
	return NDB->SaveToFile(NDB_STRUCT_PATH);
}
bool EngineCtrl::APIHCheck(uchar* ASCIIString, ui64 Len){
	return APIH->Search(ASCIIString,Len);
}
bool EngineCtrl::HMDBCheck(uchar* _MD5_ASCII,char *MalName){
	return HMDB->Search(_MD5_ASCII,MalName);
}
char* EngineCtrl::NDBSearch(bool NewSearch,ulong &Start,uchar* Data, size_t DataSize,NDBSig_TargetType Type,ulong EntryPoint,vector<ulong> Sections){
	return NDB->Search(Data,DataSize,Type,EntryPoint,Sections,Start,NewSearch);
}
ulong EngineCtrl::NumberOfHMDBSignatures(){
	return HMDB->NoSig();
}
ulong EngineCtrl::NumberOfNDBSignatures(){
	//return NDB->NoSig();
	return NULL;
}
