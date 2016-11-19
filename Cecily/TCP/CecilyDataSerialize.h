/********************************************************************
	created:	  2016/10/11
	filename: 	  CecilyDataSerialize.h
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_Data_Serialize_H
#define Cecily_Data_Serialize_H
#include "CecilyUVHelp.h"


using namespace clear;

typedef void (*RecvFullPackageCallBack)(int nClientID,char* lpBuffer,int nLen,void* user_data);

enum PackageStatus
{
	S_PackageOK = 0,
	S_NotComplete,
	S_NotEnoughHeader,
	S_NotBegin
};

class CecilyDataSerialize
{
public:
	CecilyDataSerialize(void);
	virtual ~CecilyDataSerialize(void);

public:
	static void SetMemoryPool(CCecilyMemoryPool* lpMemoryPool);
	void SetFullRecvCallBack(RecvFullPackageCallBack lpFunc,void* user_data)
	{
		m_lpFuncFullRecv = lpFunc;
		m_lpFullRecvUserData = user_data;
	}
	const char* Serialize(const char* lpBuffer,int nLen);
	void DeSerialize(int nClientID, const char* lpBuffer, int nLen);
	PackageStatus GetPackageStatus()
	{
		return m_status;
	}

private:
	int BufferRoundUP(int nSize);
	void ResetDeserializeBuffer(int nSize);

private:

	static CCecilyMemoryPool* m_lpSerializeMemoryPool;

	char* m_lpSerializeBuf;
	int m_nSerializeBufferSize;

	char* m_lpDeSerializeBuf;
	char* m_lpCurrentBuffer;
	
	int m_nDeSeerializeBufferSize;

	MessageHeader* m_lpSeralizeHeader;
	MessageHeader* m_lpDeSeralizeHeader;
	char* m_lpCurrentHeader;;
	RecvFullPackageCallBack m_lpFuncFullRecv;
	void*	m_lpFullRecvUserData;
	PackageStatus	m_status;
};


#endif