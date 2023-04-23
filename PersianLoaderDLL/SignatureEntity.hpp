/*
* @name SignatureEntity 
* @since Wed 24 July 2013
* @author Hossein Alizadeh
* @Dsc Class to Extract Data from one line of Database.. 
*/
#ifndef SigEntity_H
#define SigEntity_H
#include <iostream>
#include <vector>
#include "HEX.hpp"
#include "SigTypes.hpp"
using namespace std;

class SignatureEntity{
private:
	DBSig db;
	HMDBSig hmdb;
	NDBSig ndb;
	Sigs *Signature;
public:
	SignatureEntity(){
	}
	SignatureEntity(char* Line,SigDataType DataType){
		LoadFromLine(Line,DataType);
	}
	void LoadFromLine(char* Line,SigDataType DataType){
		//cout<<Line<<endl;
		register int i = 0;
		switch(DataType){
		case SigDataType::DB_SIGNATURE:
			{
			//MalwareName=HexSignature
			Signature=&db;
			for(; Line[i] && Line[i] != '='; i++);
			Line[i] = 0;
			db.Name = Line;
			db.HEX = Line + i + 1;
			}
			break;
		case SigDataType::HMDB_SIGNATURE:
			{ 
			//MD5:Size:MalwareName
			Signature=&hmdb;
			i = 33;
			Line[32] = 0;
			HexString2AsciiString((uchar*)Line, (uchar*)Line);
			hmdb._MD5=Line;
			hmdb.Name = Line + 33; 
			}
			break;
		case SigDataType::NDB_SIGNATURE:
			{
			//MalwareName:TargetType:Offset:HexSignature[:MinFL:[MaxFL]]
			Signature=&ndb;
			{
				//READING MaxFL and MinFL
				ndb.MaxFL=0;
				ndb.MinFL=0;
				char* MinFL=NULL;
				for(int i=0;i<(strlen(Line)-1);i++)
					if(Line[i]=='[' && Line[i+1]==':')
						MinFL=Line+i;
				if(MinFL)
				{
					*MinFL++=0;
					MinFL++;
					char *MaxFL=strchr(MinFL,':');
					*MaxFL++=0;
					MaxFL++;
					*strchr(MaxFL,']')=0;
					ndb.MinFL=atol(MinFL);
					ndb.MaxFL=atol(MaxFL);
				}
			}
			ndb.Name=Line;
			Line=strchr(Line,':');
			*Line++=0;
			{
				//READING TARGET TYPE
				if(Line[0]=='*')
					ndb.Type=NDBSig_TargetType::AnyFile;
				else
				{
					char* type=Line;
					Line=strchr(Line,':');
					*Line++=0;
					ndb.Type=(NDBSig_TargetType)atoi(type);
				}
			}
			{
				//READING OFFSET
				ndb.Offset.MaxShift=0;
				char *offset=strchr(Line,':');
				*offset=0;
				char *MaxShift=strchr(Line,',');
				if(MaxShift){
					*MaxShift++=0;
					ndb.Offset.MaxShift=atoi(MaxShift);
				}
				switch(Line[0]){
				case '*':
					{
					ndb.Offset.Type=NDBSig_OffsetType::Any;
					}
					break;
				case 'E':
					{
					if(Line[1]=='O'){
						//EOF-n
						Line+=4;
						ndb.Offset.Type=NDBSig_OffsetType::EOFMinus;
						ndb.Offset.N=atoi(Line);
					}
					else{
						Line+=2;
						if(Line[0]=='+')
							//EP+n
							ndb.Offset.Type=NDBSig_OffsetType::EPPlus;
						else
							//EP-n
							ndb.Offset.Type=NDBSig_OffsetType::EPMinus;
						ndb.Offset.N=atoi(++Line);
					}						
					}
					break;
				case 'S':
					{
						Line++;
						if(Line[0]=='L'){
							Line++;
							if(Line[0]=='+')
								ndb.Offset.Type=NDBSig_OffsetType::LastSectionPlus;
							else
								ndb.Offset.Type=NDBSig_OffsetType::LastSectionMinus;
							ndb.Offset.N=atoi(++Line);
						}
						else
						{
							//SX+n
							char *Delim=strchr(Line,'+');
							ndb.Offset.Type=NDBSig_OffsetType::SectionXPlus;
							if(!Delim)
							{
								//SX-n
								Delim=strchr(Line,'-');
								ndb.Offset.Type=NDBSig_OffsetType::SectionXMinus;
							}
							*Delim=0;
							ndb.Offset.X=atoi(Line);
							Line=Delim+1;
							ndb.Offset.N=atoi(Line);
						}
					}
					break;
				default:
					{
					ndb.Offset.Type=NDBSig_OffsetType::Absoulute;
					ndb.Offset.N=atoi(Line);
					}
					break;
				}
				Line=offset+1;
			}
			{
				//READING PARTS
				//char Line[100]="hos[4-4]sein{-1}ali{2-}gholi{3-4}mam?ad{5}chi(aa|bb|cc)letstalkabout(B)what(L)ok(dd|ee)done\0";
				ndb.Parts.push_back(Line);
				for(int i=0;Line[i]!=0;i++){
					NDBSig_Func QF;
					switch(Line[i]){
					case '?':
						{
						//aa?bb
						QF.Type=NDBSig_FuncType::QuestionMark;
						ndb.Funcs.push_back(QF);
						ndb.Parts.push_back(Line+i+1);
						Line[i]=0;
						break;
						}
					case '*':
						{
						//aa*bb
						QF.Type=NDBSig_FuncType::Asterick;
						ndb.Funcs.push_back(QF);
						ndb.Parts.push_back(Line+i+1);
						Line[i]=0;
						break;
						}
					case '{':
						{
						char *A,*B;
						B=strchr(Line+i,'}');
						Line[i]=0;
						*B=0;
						A=strchr(Line+i+1,'-');
						ndb.Parts.push_back(B+1);
						if(A==(Line+i+1)){
							//{-n}
							QF.Type=NDBSig_FuncType::GreaterThanBracket;
							QF.N=atoi(A+1);
						}
						else
						{
							if(A==(B-1)){
								//{n-}
								*A=0;
								QF.Type=NDBSig_FuncType::LessThanBracket;
							}else
								if(A){
									//{n-m}
									*A=0;
									QF.Type=NDBSig_FuncType::RangeBracket;
									QF.M=atoi(A+1);
								}else
									//{n}
									QF.Type=NDBSig_FuncType::AbsoluteBracket;
						QF.N=atoi(Line+i+1);
						}
						ndb.Funcs.push_back(QF);
						i=B-Line;
						break;
						}
					case '(':
						{
						char *A,*B;
						B=strchr(Line+i,')');
						Line[i]=0;
						*B=0;
						A=strchr(Line+i+1,'|');
						if (!A)
						{
							switch(Line[i+1]){
							case 'B':
								{
								//(B)
								QF.Type=NDBSig_FuncType::BWordBoundry;
								break;
								}
							case 'L':
								{
								//(L)
								QF.Type=NDBSig_FuncType::LWordBoundry;
								break;
								}
							}
						}
						else
						{
							QF.Type=NDBSig_FuncType::DecisionParent;
							QF.Decisions.clear();
							A=strtok(Line+i+1,"|");
							while(A){
								QF.Decisions.push_back(A);
								A=strtok(NULL,"|");
							}	
						}
						ndb.Funcs.push_back(QF);
						ndb.Parts.push_back(B+1);
						i=B-Line;
						break;
						}
					case '[':
						{
 						QF.Type=NDBSig_FuncType::Anchor;
						char *A,*B;
						B=strchr(Line+i,']');
						Line[i]=0;
						*B=0;
						A=strchr(Line+i+1,'-');
						*A=0;
						ndb.Parts.push_back(B+1);
						QF.N=atoi(Line+i+1);
						QF.M=atoi(A+1);
						ndb.Funcs.push_back(QF);
						i=B-Line;
						break;
						}
					case ':':
						{
						//NOT RECOGNIZED :
						Line[i]=0; //CUT
						Line[i+1]=0; //END OF SIGNATURE
						break;
						}
					}
				}
			}
			}
			break;
		}
		Signature->DataType=DataType;
	}
	char* getName(){
		return Signature->Name;
	}
	SigDataType getType(){
		return Signature->DataType;
	}
	HMDBSig* HDBAttributes(){
		return &hmdb;
	}
	DBSig* DBAttributes(){
		return &db;
	}
	NDBSig* NDBAttributes(){
		return &ndb;
	}
};
#endif