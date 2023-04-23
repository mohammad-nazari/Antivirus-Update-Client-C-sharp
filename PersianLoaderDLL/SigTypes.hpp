#ifndef SigTypes_H
#define SigTypes_H
#include <iostream>
#include <vector>
using namespace std;
typedef unsigned long ulong;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned __int64 ui64;
typedef unsigned int uint;

enum SigDataType{
	HMDB_SIGNATURE,
	DB_SIGNATURE,
	NDB_SIGNATURE
};
enum NDBSig_FuncType{
	QuestionMark,
	Asterick,
	AbsoluteBracket,
	LessThanBracket,
	GreaterThanBracket,
	RangeBracket,
	DecisionParent,
	BWordBoundry,
	LWordBoundry,
	Anchor,
	Null
};
enum NDBSig_TargetType{
	AnyFile,
	PE_32_64,
	InsideOLE2,
	HTML,
	MailFile,
	Graphics,
	ELF,
	ASCIItxt,
	Unused,
	MachO,
	NewType
};
enum NDBSig_OffsetType{
	Any,
	Absoulute,
	EOFMinus,
	EPPlus,
	EPMinus,
	SectionXPlus,
	SectionXMinus,
	LastSectionPlus,
	LastSectionMinus
};
struct NDBSig_Func{
	NDBSig_FuncType Type;
	vector<char*> Decisions;
	int N,M;
};
struct NDBSig_Offset{
	NDBSig_OffsetType Type;
	int N,X;
	int MaxShift;
};
struct Sigs{
	char *Name;
	SigDataType DataType;
};
struct HMDBSig:Sigs{
	char *_MD5;
	//unsigned long Size;
};
struct DBSig:Sigs{
	char *HEX;
};
struct NDBSig:Sigs{
	NDBSig_Offset Offset;
	NDBSig_TargetType Type;
	vector<char*> Parts;
	vector<NDBSig_Func> Funcs;
	ulong MinFL, MaxFL; //default 0;
};
#endif