/*
* @name SigDatabase 
* @since Wed 24 July 2013
* @author Hossein Alizadeh
* @Dsc Class to handle Hex Values
* Modified 12/14/2013
*/
#ifndef HEX_H
#define HEX_H
#include <iostream>
static void HexString2AsciiString(unsigned char *HEX, unsigned char *ASCII, unsigned long *Length){
	register unsigned char digit = 0;
	*Length = 0;
	register int inHighNibble = 0;
	for( register int i = 0; HEX[i] && HEX[i]!='\n' && HEX[i]!='\r'; i++)
	{
		HEX[i]=tolower(HEX[i]);
		inHighNibble ^= 1;
		if(HEX[ i ] <= '9' && HEX[ i ] >= '0')
			digit |= HEX[i] - '0';
		else if(HEX[ i ] <= 'f' && HEX[ i ] >= 'a')
			digit |= HEX[i] - 'a' + 10;
		if(inHighNibble)
			digit <<= 4;
		else{
			ASCII[(*Length)++] = digit;
			digit = 0;
		}
	}
	//ASCII[*Length]=0;
}
static void HexString2AsciiString(unsigned char *HEX, unsigned char *ASCII){
	unsigned long Length = 0;
	return HexString2AsciiString(HEX,ASCII,&Length);
}
static void AsciiString2HexString(unsigned char const* ASCII, unsigned char* HEX, unsigned long Length)
{
	unsigned long i;
	for(i=0; i<Length; i++)
	{
		HEX[2*i]=ASCII[i]/16;
		HEX[2*i+1]=ASCII[i]%16;
		if(HEX[2*i] >= 0 && HEX[2*i] <= 9)
			HEX[2*i] = '0' + HEX[2*i];
		else
			HEX[2*i] = 'a' + HEX[2*i]-10;
		if(HEX[2*i+1] >= 0 && HEX[2*i+1] <= 9)
			HEX[2*i+1] = '0' + HEX[2*i+1];
		else
			HEX[2*i+1] = 'a' + HEX[2*i+1]-10;
	}
	//HEX[2*i+2]=0;
}
static void AsciiString2HexString(unsigned char const* ASCII, unsigned char* HEX)
{
	AsciiString2HexString(ASCII,HEX,16);
}
#endif