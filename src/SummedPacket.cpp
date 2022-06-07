#include "SummedPacket.h"

/*

	Packet layout:
		0: 0x5E
		1: 0xE5
		2: data byte count
		2 to 2+packSize actual data
		-2: XOR of all previous bytes
		-1: SUM of all previous bytes (including XOR)

*/
SummedPacket::SummedPacket(int packSize){
	this->packSize = packSize;
	this->actualDataSize = packSize + 5;
	this->internalPack = new uint8_t[this->actualDataSize];
	this->internalPack[0] = 0x5e;
	this->internalPack[1] = 0xe5;
	this->internalPack[2] = (uint8_t)packSize;
	assemblyPointer = 0;
	for(int i = 3; i < actualDataSize; ++i){
		internalPack[i] = 0;
	}	
}

int SummedPacket::GetSendSize()
{
	return this->actualDataSize;
}

 uint8_t* SummedPacket::Data()
{
	this->internalPack[this->actualDataSize -2] = 0;
	for(int i = 0; i < this->actualDataSize - 2; ++i){
			this->internalPack[this->actualDataSize -2] ^= this->internalPack[i];
	}
	
	this->internalPack[this->actualDataSize -1] = 0;
	for(int i = 0; i < this->actualDataSize - 1; ++i){
			this->internalPack[this->actualDataSize - 1] += this->internalPack[i];
	}
	
	return this->internalPack;
}
bool SummedPacket::IsValid()
{
	uint8_t calcXOR = 0;
	this->internalPack[this->actualDataSize -2] = 0;
	for(int i = 0; i < this->actualDataSize - 2; ++i){
			calcXOR ^= this->internalPack[i];
	}
	
	uint8_t calcSUM = 0;
	this->internalPack[this->actualDataSize -1] = 0;
	for(int i = 0; i < this->actualDataSize - 1; ++i){
			this->internalPack[this->actualDataSize - 1] += this->internalPack[i];
	}
	
	return this->internalPack;
	
}

uint8_t &SummedPacket::operator[](int i){
	return this->internalPack[i+3];
}

void SummedPacket::WriteToStream(Stream& outStream){
	outStream.write(this->Data(), this->actualDataSize);
}
bool SummedPacket::FetchByte(Stream& inStream)
{
	while(inStream.available()){
		uint8_t newByte = inStream.read();
		if(this->assemblyPointer < 3)
		{
			if(newByte == this->internalPack[this->assemblyPointer])
			{
				++this->assemblyPointer;
			}
			else{
				this->assemblyPointer = 0;
			}
		}
		else if(this->actualDataSize > this->assemblyPointer){
			this->internalPack[this->assemblyPointer] = newByte;
			++this->assemblyPointer;
		}

		if(this->assemblyPointer == this->actualDataSize){
			this->assemblyPointer = 0;
			return this->IsValid();
		}
	}
	return false;
}