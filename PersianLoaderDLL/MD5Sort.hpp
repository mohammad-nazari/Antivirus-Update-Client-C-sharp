/*
* @name Memory Efficient MD5 Sort
* @since Wed 24 July 2013
* @author Hossein Alizadeh
* @Dsc Fast Searching Through MD5 Keywords
*/
#ifndef MD5Sort_H
#define MD5Sort_H
#include <vector>
#include <algorithm>
#include "SigTypes.hpp"
//#include <ctime>
#define CutPoint 3
using namespace std;

class MD5Sort{
private:
	ulong *_NoSig;
	bool *Active;
	struct LeftPart{
		uchar Content[CutPoint];
	};
	struct MD5Instance{
		ulong NamePos;
		uchar MD5[16];
	};
	FILE* FL;
	vector<LeftPart> *Left;
	vector<MD5Instance> *TemporalSigs;
	vector<char> *TemporalNames;
	bool friend operator<(MD5Instance &D1,MD5Instance &D2) {
		for(int i=0;i<16;i++){
			if(D1.MD5[i]<D2.MD5[i]) return true;
			if(D1.MD5[i]>D2.MD5[i]) return false;
		}
		return false;
	}
	bool friend operator>(MD5Instance &D1,MD5Instance &D2) {
		for(int i=0;i<16;i++){
			if(D1.MD5[i]>D2.MD5[i]) return true;
			if(D1.MD5[i]<D2.MD5[i]) return false;
		}
		return false;
	}	
	bool friend operator==(MD5Instance &D1,MD5Instance &D2) {
		for(int i=15;i>=0;i--){
			if(D1.MD5[i]<D2.MD5[i]) return false;
			if(D1.MD5[i]>D2.MD5[i]) return false;
		}
		return true;
	}
	bool friend operator<(LeftPart &D1,LeftPart &D2){
		for(int i=0;i<CutPoint;i++){
			if(D1.Content[i]<D2.Content[i]) return true;
			if(D1.Content[i]>D2.Content[i]) return false;
		}
		return false;
	}
	bool friend operator>(LeftPart &D1,LeftPart &D2){
		for(int i=0;i<CutPoint;i++){
			if(D1.Content[i]<D2.Content[i]) return false;
			if(D1.Content[i]>D2.Content[i]) return true;
		}
		return false;
	}
	bool friend operator==(LeftPart &D1,LeftPart &D2){
		for(int i=CutPoint-1;i>=0;i--){
			if(D1.Content[i]<D2.Content[i]) return false;
			if(D1.Content[i]>D2.Content[i]) return false;
		}
		return true;
	}
public:
	MD5Sort(wstring StructPath){
		FL=_wfopen(StructPath.c_str(),L"r+b");
		Left=new vector<LeftPart>();
		TemporalSigs=new vector<MD5Instance>();
		TemporalNames=new vector<char>();
		_NoSig=new ulong();
		*_NoSig=0;
		Active=new bool();
		*Active=false;
	}
	~MD5Sort(){
		FreeData();
		free(_NoSig);
		free(Active);
		free(TemporalNames);
		free(TemporalSigs);
		free(Left);
		fclose(FL);
	}
	bool EndOfInput(){
		if(!FL)
			return false;
		while(*Active){Sleep(10);}
		*Active=true;
		//NO NEED FOR LEFT NOW
		Left->clear();
		Left->shrink_to_fit();
		//FIRST, WE LOAD EVERYTHING
		ulong SZ=0,LSZ=0,NSZ=0;
		ushort CP=0;
		vector<MD5Instance> AllSigs;
		vector<char> AllNames;
		vector<ulong> Indexes;
		fseek(FL,0,SEEK_SET);
		fread(&SZ,sizeof(ulong),1,FL);
		fread(&NSZ,sizeof(ulong),1,FL);
		fread(&LSZ,sizeof(ulong),1,FL);
		fread(&CP,sizeof(ushort),1,FL);
		//ALLOCATE SPACE
		AllSigs.resize(SZ+TemporalSigs->size());
		AllNames.resize(NSZ+TemporalNames->size());
		Indexes.resize(LSZ);
		//Read the Indexes
		for(ulong i=0;i<LSZ;i++)
			fread(&Indexes[i],sizeof(ulong),1,FL);
		//Add left part into AllSig
		for(ulong i=0;i<LSZ;i++)
		{
			ulong CURRENT=Indexes[i];
			ulong NEXT=(i<(LSZ-1)?Indexes[i+1]:SZ);
			fread(AllSigs[CURRENT].MD5,sizeof(uchar),CP,FL);
			for(ulong j=CURRENT+1;j<NEXT;j++)
				memcpy(AllSigs[j].MD5,AllSigs[CURRENT].MD5,sizeof(uchar)*CP);
		}
		//Add right part and NamePos into AllSig
		for(ulong i=0;i<SZ;i++){
			fread(AllSigs[i].MD5+CP,sizeof(uchar),16-CP,FL);
			fread(&AllSigs[i].NamePos,sizeof(ulong),1,FL);
		}
		if(NSZ)
			fread(&AllNames[0],sizeof(uchar),NSZ,FL);
		//NOW WE INCLUDE THE NEW ONES
		for(ulong i=0;i<TemporalSigs->size();i++){
			memcpy(AllSigs[i+SZ].MD5,TemporalSigs->at(i).MD5,16*sizeof(uchar));
			AllSigs[i+SZ].NamePos=TemporalSigs->at(i).NamePos+NSZ;
		}
		for(ulong i=0;i<TemporalNames->size();i++)
			AllNames[i+NSZ]=TemporalNames->at(i);
		//NO NEED FOR TEMPORAL DATA ANYMORE
		TemporalSigs->clear();
		TemporalSigs->shrink_to_fit();
		TemporalNames->clear();
		TemporalNames->shrink_to_fit();
		Indexes.clear();
		Indexes.shrink_to_fit();
		//THIS IS WHERE WE SORT THE DATA
		sort(AllSigs.begin(),AllSigs.end());
		for(ulong i=0;i<AllSigs.size();i++){
			bool chk=Left->size();
			if(chk)
				for(int j=(CutPoint-1);j>=0;j--)
					if(Left->at(Left->size()-1).Content[j]!=AllSigs[i].MD5[j]){
						chk=false;
						break;
					}
					if(!chk){
						LeftPart Temp;
						for(uint j=0;j<CutPoint;j++)
							Temp.Content[j]=AllSigs[i].MD5[j];
						Left->push_back(Temp);
						Indexes.push_back(i);
					}
		}
		//NOW LETS SAVE THE DATA BACK
		fseek(FL,0,SEEK_SET);
		CP=CutPoint;
		SZ=AllSigs.size();
		NSZ=AllNames.size();
		LSZ=Left->size();
		fwrite(&SZ,sizeof(ulong),1,FL);
		fwrite(&NSZ,sizeof(ulong),1,FL);
		fwrite(&LSZ,sizeof(ulong),1,FL);
		fwrite(&CP,sizeof(ushort),1,FL);
		for(ulong i=0;i<LSZ;i++)
			fwrite(&Indexes[i],sizeof(ulong),1,FL);
		for(ulong i=0;i<LSZ;i++)
			fwrite(Left->at(i).Content,sizeof(uchar),CutPoint,FL);
		for(ulong i=0;i<SZ;i++){
			fwrite(AllSigs[i].MD5+CutPoint,sizeof(uchar),16-CutPoint,FL);
			fwrite(&AllSigs[i].NamePos,sizeof(ulong),1,FL);
		}
		if(AllNames.size())
			fwrite(&AllNames[0],sizeof(uchar),AllNames.size(),FL);
		fflush(FL);
		//WE ONLY KEEP WHAT WE NEED
		Indexes.clear();
		Indexes.shrink_to_fit();
		AllSigs.clear();
		AllSigs.shrink_to_fit();
		AllNames.clear();
		AllNames.clear();
		Left->clear();
		Left->shrink_to_fit();
		*_NoSig=0;
		bool chk=LoadFromFile();
		*Active=false;
		return chk;
	}
	void AddSignature(HMDBSig *Sig){
		MD5Instance CVT;
		memcpy(CVT.MD5,Sig->_MD5,16);
		CVT.NamePos=TemporalNames->size();
		for(int i=0;i<strlen(Sig->Name);i++)
			TemporalNames->push_back(Sig->Name[i]);
		TemporalNames->push_back(0);
		TemporalSigs->push_back(CVT);
	}
	bool Search(uchar *MD5,char *MalName){
		//FIRST THE CutPoint
		LeftPart TMPLeft;
		memcpy(TMPLeft.Content,MD5,CutPoint);
		ulong mn=0,mx=Left->size(),mid;
		bool FOUND=false;
		while(mx-mn>0){
			mid=(mx+mn)/2;
			if(TMPLeft>Left->at(mid))
				if(mn==mid)
					break;
				else
					mn=mid;
			else if(TMPLeft<Left->at(mid))
				if(mx==mid)
					break;
				else
					mx=mid;
			else{
				FOUND=true;
				break;
			}
		}
		if(FOUND){
			if(!FL)
				return false;
			while(*Active){Sleep(10);}
			*Active=true;
			//SKIP SZ NSZ LSZ CP AND I1 to IN
			fseek(FL,sizeof(ulong)*3+sizeof(ushort)+sizeof(ulong)*mid,SEEK_SET);
			fread(&mn,sizeof(ulong),1,FL);
			fread(&mx,sizeof(ulong),1,FL);
			vector<MD5Instance> Right(mx-mn);
			//SKIP SZ,NSZ,LSZ,CP,I1->END,L1->END,R1->RN,N1->NN
			fseek(FL,sizeof(ulong)*3+sizeof(ushort)+sizeof(ulong)*Left->size()+sizeof(uchar)*Left->size()*CutPoint+sizeof(ulong)*mn+sizeof(uchar)*mn*(16-CutPoint),SEEK_SET);
			mx=mx-mn;
			mn=0;
			for(ulong i=0;i<mx;i++){
				memcpy(Right[i].MD5,TMPLeft.Content,CutPoint);
				fread(Right[i].MD5+CutPoint,sizeof(uchar),16-CutPoint,FL);
				fread(&Right[i].NamePos,sizeof(ulong),1,FL);
			}
			MD5Instance TMPRight;
			memcpy(TMPRight.MD5,MD5,16);
			while(mx-mn>0){
				mid=(mx+mn)/2;
				if(TMPRight>Right[mid])
					if(mn==mid)
						break;
					else
						mn=mid;
				else if(TMPRight<Right[mid])
					if(mx==mid)
						break;
					else
						mx=mid;
				else{
					//SKIP SZ,NSZ,LSZ,CP,I1->END,L1->END,R1->RN,N1->NN
					fseek(FL,sizeof(ulong)*3+sizeof(ushort)+sizeof(ulong)*Left->size()+sizeof(uchar)*Left->size()*CutPoint+sizeof(ulong)**_NoSig+sizeof(uchar)**_NoSig*(16-CutPoint)+sizeof(uchar)*Right[mid].NamePos,SEEK_SET);
					fread(MalName,sizeof(uchar),1,FL);
					while(*MalName!=0)
						fread(++MalName,sizeof(uchar),1,FL);
					*Active=false;
					return true;
				}
			}
			*Active=false;
		}
		return false;
	}
	bool LoadFromFile(){
		fseek(FL,0,SEEK_SET);
		if(!FL)
			return false;
		Left->clear();
		Left->shrink_to_fit();
		ulong LSZ;
		ushort CP;
		//READ SZ
		fread(_NoSig,sizeof(ulong),1,FL);
		//SKIP NSZ
		fseek(FL,sizeof(ulong),SEEK_CUR);
		fread(&LSZ,sizeof(ulong),1,FL);
		//CHEK CutPoint
		fread(&CP,sizeof(ushort),1,FL);
		if(CP!=CutPoint)
			return false;
		//ALLOCATE
		Left->resize(LSZ);
		//SKIP THE INDEXES
		fseek(FL,sizeof(ulong)*LSZ,SEEK_CUR);
		for(ulong i=0;i<LSZ;i++)
			fread(Left->at(i).Content,sizeof(uchar),CutPoint,FL);
		return true;
	}
	ulong NoSig(){
		return *_NoSig;
	}
	void FreeData(){
		Left->clear();
		Left->shrink_to_fit();
		*_NoSig=0;
	}
};
#endif