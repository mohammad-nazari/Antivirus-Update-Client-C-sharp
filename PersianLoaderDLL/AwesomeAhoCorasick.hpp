/*
* @name Memory Efficient AhoCorasick 
* @since Wed 24 July 2013
* @author Hossein Alizadeh
* @Dsc AhoCorasick Trie to Search Through Keywords
*/
#ifndef AwesomeAhoCorasick_H
#define AwesomeAhoCorasick_H
#include <iostream>
#include <vector>
#include <algorithm>
#include "HEX.hpp"
#include "SigTypes.hpp"
#define NamesAt Names->at
#define HashCallAt HashCall->at
#define HashTableAt HashTable->at
#define PeopleAt People->at
#define HashCallSize HashCall->size
#define HashCallFor(Person,i) HashCallAt(PeopleAt(Person).H).at(i)
#define HashCallSizeFor(Person) HashCallAt(PeopleAt(Person).H).size()
#define HashTableSize HashTable->size
#define PeopleSize People->size
#define NamesSize Names->size
#define HashTableFor(Person,i) HashTableAt(HashCallFor(Person,i))
#define HasHash(Person) PeopleAt(Person).H!=0
#define HasActivator(Person) PeopleAt(Person).A!=0
#define ActivatorAt Activator->at
#define ActivatorSize Activator->size
#define NewActivator() Activator->push_back(vector<pair<ulong,ulong>>())
#define NewActivation(Person1,Parent,Person2) ActivatorAt(PeopleAt(Person1).A).push_back(pair<ulong,ulong>(Parent,Person2))
#define ActivatorFor(Person,i) ActivatorAt(PeopleAt(Person).A).at(i)
#define ActivatorSizeFor(Person) ActivatorAt(PeopleAt(Person).A).size()
#define NewHashCaller() HashCall->push_back(vector<ulong>())
#define NewHashCall(Person,Which) HashCallAt(PeopleAt(Person).H).push_back(Which);
#define NewHashTable() HashTable->push_back(pair<short,ulong>(0,0))
#define NamesFor(Start,i) &NamesAt(HashTableFor(Start,i).second)
#define NumOfTargets 11
#define CutPoint 3
using namespace std;

struct AweAC_Node{
	//Index of the Kid
	ulong K;
	//Index of the Neighbor
	ulong N;
	//Index of Activator Vector
	ulong A;
	//Index of the Failure
	ulong F;
	//Position in HashCall
	ulong H;
	//Character
	uchar C;
	//Times of Activation
	ushort B;
};
template <class A,class B,class C>
struct triad{
	A first;
	B second;
	C third;
	triad(A a,B b,C c){
		first=a;
		second=b;
		third=c;
	}
};
class AwesomeAhoCorasick{
private:
	ulong *_NoSig;
	vector<pair<short,ulong>> *HashTable;					// IS IT ON, WHICH NAME DOES IT POINT TO
	vector<AweAC_Node> *People;				
	vector<char> *Names;
	vector<vector<pair<ulong,ulong>>> *Activator;			//WHO is its Parent, Who
	vector<vector<ulong>> *HashCall;						//Which HashTables to turn on
	vector<pair<NDBSig_Offset,ulong>> *TargetOffsetCall;	//What Offset, Which Activator 
	bool friend operator<(triad<ui64,ulong,bool> &D1,triad<ui64,ulong,bool> &D2) {
		return D1.first<D2.first;
	}
	bool FindKid(ulong Position,ulong &TheKid,uchar C,bool MustBeActive){
		if(MustBeActive)
			if(!PeopleAt(Position).B)
				return false;
		//THE SORT NODE METHOD
		ulong ThisKid=PeopleAt(Position).K;
		while(ThisKid){
			if(MustBeActive)
				if(!PeopleAt(ThisKid).B)
					return false;
			TheKid=ThisKid;
			if(PeopleAt(ThisKid).C==C)
				return true;
			ThisKid=PeopleAt(ThisKid).N;
		}
		return false;
	}
public:
	AwesomeAhoCorasick(){
		People=new vector<AweAC_Node>(1);
		_NoSig=new ulong();
		Names=new vector<char>(1);
		HashTable=new vector<pair<short,ulong>>(1);
		Activator=new vector<vector<pair<ulong,ulong>>>(1);
		HashCall=new vector<vector<ulong>>(1);
		TargetOffsetCall=new vector<pair<NDBSig_Offset,ulong>>[NumOfTargets];
		FreeData();
	}
	void WakeupCall(ulong A,ulong Parent,ulong B){
		if(!HasActivator(A))
		{
			PeopleAt(A).A=ActivatorSize();
			NewActivator();
		}
		for(int i=0;i<ActivatorSizeFor(A);i++)
			if(ActivatorFor(A,i).second==B)
				return;
		NewActivation(A,Parent,B);
	}
	void WakeUp(ulong Parent,ulong Me){
		if(PeopleAt(Me).B<65535)
			PeopleAt(Me).B++;
		if(PeopleAt(Me).B==1 && PeopleAt(Parent).K!=Me){
			ulong Left=PeopleAt(Parent).K;
			while(PeopleAt(Left).N!=Me)
				Left=PeopleAt(Left).N;
			PeopleAt(Left).N=PeopleAt(Me).N;
			PeopleAt(Me).N=PeopleAt(Parent).K;
			PeopleAt(Parent).K=Me;
		}
	}
	void RegisterHashCall(ulong Me,ulong Which){
		if(!PeopleAt(Me).H){
			PeopleAt(Me).H=HashCallSize();
			NewHashCaller();
		}
		NewHashCall(Me,Which);
	}
	void Sleep(ulong Parent,ulong Me){
		if(PeopleAt(Me).B>0){
			PeopleAt(Me).B--;
			if(PeopleAt(Me).B==0){
				ulong LeftMe=PeopleAt(Parent).K,RightMe=PeopleAt(Me).N,LastOn=Me,FirstOff;
				while(LeftMe!=Me && PeopleAt(LeftMe).N!=Me)
					LeftMe=PeopleAt(LeftMe).N;
				while(PeopleAt(LastOn).N && PeopleAt(PeopleAt(LastOn).N).B)
					LastOn=PeopleAt(LastOn).N;
				FirstOff=PeopleAt(LastOn).N;
				if(LastOn!=Me)
				{
					PeopleAt(Me).N=FirstOff;
					PeopleAt(LastOn).N=Me;
					if(PeopleAt(Parent).K==Me)
						//Me-a-Last => a-Last-Me
						PeopleAt(Parent).K=RightMe;
					else
					{
						//z-a-Me-b-Last-off => z-a-b-Last-Me-off
						PeopleAt(LeftMe).N=RightMe;	
					}
				}
			}
		}
	}
	void AddSignature(NDBSig *Sig){
		//THIS PART MAN :)
		for(int i=0;i<Sig->Parts.size();i++){
			if(strlen(Sig->Parts.at(i))<4)
				return;
		}
		ulong MyActivator;
		{
			//IN THIS AREA, WE CREATE A NEW ACTIVATOR THAT IS SPECIFICALLY FOR ACTIVATING A SIGNATURE
			MyActivator=ActivatorSize();
			TargetOffsetCall[Sig->Type].push_back(pair<NDBSig_Offset,ulong>(Sig->Offset,MyActivator));
			NewActivator();
		}
		ulong CRPos,tempPosition, l, cut, LastGuy=0;
		for(uint i=0;i<Sig->Parts.size();i++){
			CRPos=0;
			uchar* ThisPart=(uchar*)Sig->Parts.at(i);
			HexString2AsciiString(ThisPart,ThisPart,&l);
			for(cut=0;cut<l && FindKid(CRPos,tempPosition,ThisPart[cut],false);cut++){
				ulong Parent=CRPos;
				CRPos=tempPosition;
				if(i>0)
					WakeupCall(LastGuy,Parent,CRPos);
				else
					Activator->at(MyActivator).push_back(pair<ulong,ulong>(Parent,CRPos));
			}
			for(;cut<l;cut++){
				//ADDING
				ulong thisGuy=PeopleSize();
				AweAC_Node TMP;
				TMP.A=0;
				TMP.B=0;
				TMP.F=0;
				TMP.K=0;
				TMP.N=0;
				TMP.H=0;
				TMP.C=ThisPart[cut];
				People->push_back(TMP);
				if(PeopleAt(CRPos).K)
					PeopleAt(tempPosition).N=thisGuy;
				else
					PeopleAt(CRPos).K=thisGuy;
				//NOW THE LAST GUY HAS TO TURN ME ON, BUT OF COURSE, NOT THE FATHER OF US ALL
				if(i>0)
					WakeupCall(LastGuy,CRPos,thisGuy);
				else 
					Activator->at(MyActivator).push_back(pair<ulong,ulong>(CRPos,thisGuy));
				CRPos=thisGuy;
			}
			//I AM THISGUY, AND I MAY WANT TO TURN OTHERS ON
			LastGuy=CRPos;
			//I ALSO NEED TO TURN ON HASH TABLE
			RegisterHashCall(LastGuy,HashTableSize());
			NewHashTable();
		}
		HashTableFor(CRPos,HashCallSizeFor(CRPos)-1).second=NamesSize();
		for(uint i=0;i<strlen(Sig->Name);i++)
			Names->push_back(Sig->Name[i]);
		Names->push_back(0);
		(*_NoSig)++;
	}
	char* Search(uchar* ASCIIString, ui64 Len, NDBSig_TargetType Type, ulong EntryPoint, vector<ulong> Sections,ulong &Start,bool IsNew){
		vector<triad<ui64,ulong,bool>> Changes; //Where, Which Activator, on or off
		//REMEMBER TO TURN OFF EVERYTHING, BY THE WAY, WE DONT NEED CHANGES AFTER APPLIED
		if(IsNew){
			for(ulong i=1;i<PeopleSize();i++)
				PeopleAt(i).B=0;
			for(int i=0;i<HashTableSize();i++)
				HashTableAt(i).first=0;
		}
		//ANYFILE OR THIS TYPE SHOULD BE INCLUDED IN OFFSET INFLUENCES
		ulong AnyFileTypeSize=TargetOffsetCall[AnyFile].size();
		for(int i=0;i<AnyFileTypeSize+(Type!=AnyFile?TargetOffsetCall[Type].size():0);i++){
			ui64 OffsetStart, OffsetEnd;
			NDBSig_Offset Offset;
			ulong ACT;
			if(i>=AnyFileTypeSize){
				Offset=TargetOffsetCall[Type][i-AnyFileTypeSize].first;
				ACT=TargetOffsetCall[Type][i-AnyFileTypeSize].second;
			}
			else
			{
				Offset=TargetOffsetCall[AnyFile][i].first;
				ACT=TargetOffsetCall[AnyFile][i].second;
			}
			switch(Offset.Type){
			case Absoulute:
				OffsetStart=Offset.N;
				break;
			case Any:
				OffsetStart=0;
				break;
			case EOFMinus:
				OffsetStart=Len-Offset.N;
				break;
			case EPMinus:
				OffsetStart=EntryPoint-Offset.N;
				break;
			case EPPlus:
				OffsetStart=EntryPoint+Offset.N;
				break;
			case LastSectionMinus:
				if(Sections.size())
					OffsetStart=Sections[Sections.size()-1]-Offset.N;
				else
					OffsetStart=Len;
				break;
			case LastSectionPlus:
				if(Sections.size())
					OffsetStart=Sections[Sections.size()-1]+Offset.N;
				else
					OffsetStart=Len;
				break;
			case SectionXMinus:
				if(Sections.size()>Offset.X)
					OffsetStart=Sections[Offset.X]-Offset.N;
				else
					OffsetStart=Len;
				break;
			case SectionXPlus:
				if(Sections.size()>Offset.X)
					OffsetStart=Sections[Offset.X]+Offset.N;
				else
					OffsetStart=Len;
				break;
			}
			if(Offset.Type!=Any)
				OffsetEnd=OffsetStart+Offset.MaxShift+1;
			else
				OffsetEnd=Len;
			if(OffsetStart<Len)
				Changes.push_back(triad<ui64,ulong,bool>(OffsetStart,ACT,true));
			if(OffsetEnd<Len)
				Changes.push_back(triad<ui64,ulong,bool>(OffsetEnd,ACT,false));
		}
		sort(Changes.begin(),Changes.end());
		ulong tempPosition;
		for(register ui64 tmp=0;tmp<Len;tmp++){
			int CHNG;
			for(CHNG=0;CHNG<Changes.size();CHNG++)
				if(Changes[CHNG].first==tmp)
					for(ulong ACTi=0;ACTi<ActivatorAt(Changes[CHNG].second).size();ACTi++)
						if(Changes[CHNG].third)
							WakeUp(ActivatorAt(Changes[CHNG].second)[ACTi].first,ActivatorAt(Changes[CHNG].second)[ACTi].second);
						else
							Sleep(ActivatorAt(Changes[CHNG].second)[ACTi].first,ActivatorAt(Changes[CHNG].second)[ACTi].second);
				else
					break;
			if(CHNG)
				Changes.erase(Changes.begin(),Changes.begin()+CHNG);	
			bool FOUND=true;
			while(!FindKid(Start,tempPosition,ASCIIString[tmp],true)){
				if(Start==0){
					FOUND=false;
					break;
				}else
					Start=PeopleAt(Start).F;
			}                  
			if(FOUND){
				Start=tempPosition;
				if(HasActivator(Start)){
					Changes.push_back(triad<ui64,ulong,bool>(tmp+1,PeopleAt(Start).A,true));
					sort(Changes.begin(),Changes.end());
				}
				if(HasHash(Start))
					for(int i2=0;i2<HashCallSizeFor(Start);i2++)
						if(HashTableAt(HashCallFor(Start,i2)-1).second || HashTableAt(HashCallFor(Start,i2)-1).first){
							HashTableFor(Start,i2).first++;
							if(HashTableFor(Start,i2).second)
								return NamesFor(Start,i2);
						}
			}
		}
		return NULL;
	}
	void EndOfSignature(){
		//FAILURE CONSTRUCTION
		vector<ulong> Queue;
		ulong OneKid=PeopleAt(0).K;
		while(OneKid){
			Queue.push_back(OneKid);
			PeopleAt(OneKid).F=0;
			OneKid=PeopleAt(OneKid).N;
		}
		while(Queue.size()>0){
			ulong r=Queue[0];
			Queue.erase(Queue.begin(),Queue.begin()+1);
			OneKid=PeopleAt(r).K;
			ulong S_backup=PeopleAt(r).F;
			while(OneKid){
				ulong State=S_backup,tempPosition;
				ulong S=OneKid;
				uchar a=PeopleAt(S).C;
				Queue.push_back(S);
				while(!FindKid(State,tempPosition,a,false))
					if(State==0)
						break;
					else
						State=PeopleAt(State).F;
				if(FindKid(State,tempPosition,a,false))
					State=tempPosition;
				PeopleAt(S).F=State;
				OneKid=PeopleAt(OneKid).N;
			}
		}
	}
	bool LoadFromFile(wchar_t *Path){
		FreeData();
		FILE *tmp=_wfopen(Path,L"rb");
		if(!tmp)
			return false;
		ulong CNT=0,HTSZ=0,ASZ=0,HCSZ=0,PSZ=0,NSZ=0,TSZ[NumOfTargets];
		memset(TSZ,0,sizeof(ulong)*NumOfTargets); 
		//Number of Signatures
		fread(_NoSig,sizeof(ulong),1,tmp);
		//Number of People
		fread(&PSZ,sizeof(ulong),1,tmp);
		//Number of Activators
		fread(&ASZ,sizeof(ulong),1,tmp);
		//Number of HashCalls
		fread(&HCSZ,sizeof(ulong),1,tmp);
		//Number of HashTables
		fread(&HTSZ,sizeof(ulong),1,tmp);
		//Number of TargetTypes
		fread(TSZ,sizeof(ulong),NumOfTargets,tmp);
		//Number of Names
		fread(&NSZ,sizeof(ulong),1,tmp);
		//Allocation
		People->resize(PSZ);
		Activator->resize(ASZ);
		HashCall->resize(HCSZ);
		HashTable->resize(HTSZ);
		Names->resize(NSZ);
		//People
		if(PSZ)
			fread(&PeopleAt(0),sizeof(AweAC_Node),PSZ,tmp);
		//Activators
		for(ulong i=0;i<ASZ;i++){
			ulong ANSZ=0;
			fread(&ANSZ,sizeof(ulong),1,tmp);
			ActivatorAt(i).resize(ANSZ);
			if(ANSZ)			
				fread(&ActivatorAt(i).at(0),sizeof(pair<ulong,ulong>),ANSZ,tmp);
		}
		//HashCalls
		for(ulong i=0;i<HCSZ;i++){
			ushort HCTSZ=0;
			fread(&HCTSZ,sizeof(ushort),1,tmp);
			HashCallAt(i).resize(HCTSZ);
			//HashCallTo
			if(HCTSZ)
				fread(&HashCallAt(i).at(0),sizeof(ulong),HCTSZ,tmp);
		}
		//HashTables
		if(HTSZ)
			fread(&HashTableAt(0),sizeof(pair<short,ulong>),HTSZ,tmp);
		//TargetCalls
		for(int i=0;i<NumOfTargets;i++){
			TargetOffsetCall[i].resize(TSZ[i]);
			if(TSZ[i])
				fread(&TargetOffsetCall[i].at(0),sizeof(pair<NDBSig_Offset,ulong>),TSZ[i],tmp);
		}
		//Names
		if(NSZ)
			fread(&NamesAt(0),sizeof(char),NSZ,tmp);
		fclose(tmp);
		return true;
	}
	bool SaveToFile(wchar_t *Path){
		FILE *tmp=_wfopen(Path,L"wb");
		if(!tmp)
			return false;
		ulong CNT=*_NoSig,HTSZ=HashTableSize(),ASZ=ActivatorSize(),HCSZ=HashCallSize(),PSZ=PeopleSize(),NSZ=NamesSize();
		//Number of Signatures
		fwrite(&CNT,sizeof(ulong),1,tmp);
		//Number of People
		fwrite(&PSZ,sizeof(ulong),1,tmp);
		//Number of Activators
		fwrite(&ASZ,sizeof(ulong),1,tmp);
		//Number of HashCalls
		fwrite(&HCSZ,sizeof(ulong),1,tmp);
		//Number of HashTables
		fwrite(&HTSZ,sizeof(ulong),1,tmp);
		//Number of TargetTypes
		for(int i=0;i<NumOfTargets;i++)
		{
			ulong TSZ=TargetOffsetCall[i].size();
			fwrite(&TSZ,sizeof(ulong),1,tmp);
		}
		//Number of Names
		fwrite(&NSZ,sizeof(ulong),1,tmp);
		//People
		if(PSZ)
			fwrite(&PeopleAt(0),sizeof(AweAC_Node),PSZ,tmp);
		//Activators
		for(ulong i=0;i<ASZ;i++){
			ulong ANSZ=ActivatorAt(i).size();
			fwrite(&ANSZ,sizeof(ulong),1,tmp);
			//Activations
			if(ANSZ)			
				fwrite(&ActivatorAt(i).at(0),sizeof(pair<ulong,ulong>),ANSZ,tmp);
		}
		//HashCalls
		for(ulong i=0;i<HCSZ;i++){
			ushort HCTSZ=HashCallAt(i).size();
			fwrite(&HCTSZ,sizeof(ushort),1,tmp);
			//HashCallTo
			if(HCTSZ)
				fwrite(&HashCallAt(i).at(0),sizeof(ulong),HCTSZ,tmp);
		}
		//HashTables
		if(HTSZ)
			fwrite(&HashTableAt(0),sizeof(pair<short,ulong>),HTSZ,tmp);
		//TargetCalls
		for(int i=0;i<NumOfTargets;i++){
			ulong TSZ=TargetOffsetCall[i].size();
			if(TSZ)
				fwrite(&TargetOffsetCall[i].at(0),sizeof(pair<NDBSig_Offset,ulong>),TSZ,tmp);
		}
		//Names
		if(NSZ)
			fwrite(&NamesAt(0),sizeof(char),NSZ,tmp);
		fclose(tmp);
		return true;
	}
	ulong NoSig(){
		return *_NoSig;
	}
	void FreeData(){
		People->resize(1);
		Names->resize(1);
		HashTable->resize(1);
		Activator->resize(1);
		HashCall->resize(1);
		for(int i=0;i<NumOfTargets;i++){
			TargetOffsetCall[i].clear();
			TargetOffsetCall[i].shrink_to_fit();
		}
		People->shrink_to_fit();
		Names->shrink_to_fit();
		HashTable->shrink_to_fit();
		Activator->shrink_to_fit();
		HashCall->shrink_to_fit();
		HashTableAt(0).second=1000;//1000 awesome people
		//This is going to take a little time + NULL space in activator
		//HashCall is the function to HashTable + NULL of courses
		//TargetCall is to aim at targets, 11 is the number of targets available, Last one is new and unknown
		//NULL space in names
		NamesAt(0)='A'; // 'A' stands for 'Awesome'
		//0 Father
		PeopleAt(0).A=0;
		PeopleAt(0).B=1;
		PeopleAt(0).F=0;
		PeopleAt(0).K=0;
		PeopleAt(0).N=0;
		PeopleAt(0).H=0;
		PeopleAt(0).C=0;
		//of course, no signatures yet
		*_NoSig=0;
	}
};
#endif