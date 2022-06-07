#ifndef _SUMMEDPACKET_H_
#define _SUMMEDPACKET_H_
#include <Arduino.h>

class SummedPacket
{
private:
	int packSize;
	int actualDataSize;
	uint8_t* internalPack;
	uint8_t* Data();
	int GetSendSize();

	int assemblyPointer;

public:	
	SummedPacket(int packSize);

	bool IsValid();
	uint8_t &operator[](int i);

	void WriteToStream(Stream& outStream);
	bool FetchByte(Stream& inStream);
};
#endif