/********************************************************************
	created:	  2016/10/11
	filename: 	  CecilyDataSerialize.cpp
	author:		  李文禹
	E-Mail：	  hynulwy@126.com
	description:  
*********************************************************************/
#include "StdAfx.h"
#include "CecilyDataSerialize.h"

CCecilyMemoryPool* CecilyDataSerialize::m_lpSerializeMemoryPool = NULL;

CecilyDataSerialize::CecilyDataSerialize(void)
{
	m_lpSerializeBuf = NULL;
	m_nSerializeBufferSize = 0;
	m_lpDeSerializeBuf = NULL;
	m_nDeSeerializeBufferSize = 0;
	m_lpSeralizeHeader = NULL;
	m_lpDeSeralizeHeader = NULL;
	m_status = S_NotBegin;
	m_lpFuncFullRecv = NULL;
}

CecilyDataSerialize::~CecilyDataSerialize(void)
{
	if ( NULL != m_lpSerializeMemoryPool )
	{
		if ( NULL != m_lpSerializeBuf )
		{
			m_lpSerializeMemoryPool->Deallocate((void*)m_lpSerializeBuf,m_nSerializeBufferSize);
		}
		if ( NULL != m_lpDeSerializeBuf)
		{
			m_lpSerializeMemoryPool->Deallocate((void*)m_lpDeSerializeBuf,m_nDeSeerializeBufferSize);
		}
	}else
	{
		if ( NULL != m_lpSerializeBuf )
		{
			delete []m_lpSerializeBuf;
			m_lpSerializeBuf = NULL;
		}
		if ( NULL != m_lpDeSerializeBuf )
		{
			delete []m_lpDeSerializeBuf;
			m_lpDeSerializeBuf = NULL;
		}
	}
	if ( NULL != m_lpSeralizeHeader )
	{
		delete m_lpSeralizeHeader;
		m_lpSeralizeHeader = NULL;
	}
	if ( NULL != m_lpDeSeralizeHeader )
	{
		delete m_lpDeSeralizeHeader;
		m_lpDeSeralizeHeader = NULL;
	}
}

void CecilyDataSerialize::SetMemoryPool(CCecilyMemoryPool* lpMemoryPool)
{
	m_lpSerializeMemoryPool = lpMemoryPool;
}

int CecilyDataSerialize::BufferRoundUP(int nBytes)
{
	if ( nBytes < 0 )
	{
		return 0;
	}
	size_t nSize = nBytes / POOL_ALIGN ;
	size_t nRemainder = nBytes % POOL_ALIGN;
	if ( nRemainder > 0 )
	{
		nSize = nSize + 1;
	}
	return ( nSize * POOL_ALIGN );
}

void CecilyDataSerialize::ResetDeserializeBuffer(int nSize)
{
	if ( NULL != m_lpDeSerializeBuf )
	{
		if ( NULL != m_lpSerializeMemoryPool )
		{
			if ( BufferRoundUP(m_nDeSeerializeBufferSize) >= BufferRoundUP(nSize) )
			{
				int nRealSize = BufferRoundUP(m_nDeSeerializeBufferSize);
				memset(m_lpDeSerializeBuf,0,nRealSize);
				//m_nDeSeerializeBufferSize = m_nDeSeerializeBufferSize > nSize ? m_nDeSeerializeBufferSize : nSize;
				m_nDeSeerializeBufferSize = nSize;
			}
			else
			{
				m_lpSerializeMemoryPool->Deallocate((void*)m_lpDeSerializeBuf,m_nDeSeerializeBufferSize);
				m_lpDeSerializeBuf = (char*)m_lpSerializeMemoryPool->Allocate(nSize);
				m_nDeSeerializeBufferSize = nSize;
			}
		}else
		{
			if ( m_nDeSeerializeBufferSize >= nSize )
			{
				memset(m_lpDeSerializeBuf,0,m_nDeSeerializeBufferSize);
			}
			else
			{
				delete []m_lpDeSerializeBuf;
				m_lpDeSerializeBuf = NULL;
				m_lpDeSerializeBuf = new char[nSize];
				memset(m_lpDeSerializeBuf,0,nSize);
				m_nDeSeerializeBufferSize = nSize;
			}
		}	
	}
	else
	{
		if ( NULL != m_lpSerializeMemoryPool )
		{
			m_lpDeSerializeBuf = (char*)m_lpSerializeMemoryPool->Allocate(nSize);
			m_nDeSeerializeBufferSize = nSize;
		}
		else
		{
			m_lpDeSerializeBuf = new char[nSize];
			memset(m_lpDeSerializeBuf,0,nSize);
			m_nDeSeerializeBufferSize = nSize;
		}

	}
	m_lpCurrentBuffer = m_lpDeSerializeBuf;

}

const char* CecilyDataSerialize::Serialize(const char* lpBuffer,int nLen)
{
	if ( NULL == lpBuffer || nLen <= 0 )
	{
		return NULL;
	}
	if ( NULL != m_lpSeralizeHeader )
	{
		memset(m_lpSeralizeHeader,0,Message_Header_Size);
	}
	else
	{
		m_lpSeralizeHeader = new MessageHeader;
	}
	if ( NULL != m_lpSerializeBuf )
	{
		if ( m_lpSerializeMemoryPool )
		{
			m_lpSerializeMemoryPool->Deallocate((void*)m_lpSerializeBuf,m_nSerializeBufferSize);
		}
		else
		{
			delete []m_lpSerializeBuf;
			m_lpSerializeBuf = NULL;
		}
	}
	int nBufferSize = Message_Header_Size + 2 + nLen;
	if ( NULL != m_lpSerializeMemoryPool )
	{
		m_lpSerializeBuf = (char*)m_lpSerializeMemoryPool->Allocate(nBufferSize);
		m_nSerializeBufferSize = nBufferSize;
	}
	else
	{
		m_lpSerializeBuf = new char[nBufferSize];
		memset(m_lpSerializeBuf,0,nBufferSize);
		m_nSerializeBufferSize = nBufferSize;
	}
	char* m_lpBufferCurrent = m_lpSerializeBuf;
	memcpy_s(m_lpBufferCurrent,1,&header,1);
	m_lpBufferCurrent = m_lpBufferCurrent + sizeof(header);
	m_lpSeralizeHeader->packagelength = nLen;
	memcpy_s(m_lpBufferCurrent,Message_Header_Size,m_lpSeralizeHeader,Message_Header_Size);
	m_lpBufferCurrent = m_lpBufferCurrent + Message_Header_Size;
	memcpy_s(m_lpBufferCurrent,nLen,lpBuffer,nLen);
	m_lpBufferCurrent = m_lpBufferCurrent + nLen;
	memcpy_s(m_lpBufferCurrent,1,&tail,1);
	return m_lpSerializeBuf;
}


void CecilyDataSerialize::DeSerialize(int nClientID, const char* lpBuffer, int nLen)
{
	if ( nClientID < 0 || NULL  == lpBuffer || nLen <= 0 )
	{
		return ;
	}
	if ( m_status == S_NotBegin ||  m_status == S_PackageOK )
	{

		//首先判断包头 
		char* lpBufferCurrent = (char*)lpBuffer;
		unsigned char tmpHeader = lpBufferCurrent[0];
		if ( tmpHeader != header )
		{
			//非法包
			return ;
		}
		if ( NULL != m_lpDeSeralizeHeader )
		{
			memset(m_lpDeSeralizeHeader,0,Message_Header_Size);
		}else
		{
			m_lpDeSeralizeHeader = new MessageHeader();
		}
		m_lpCurrentHeader = (char*)m_lpDeSeralizeHeader;
		if ( nLen < Message_Header_Size + 1 )  //发过来的缓冲区还不够一个包头
		{
			memcpy_s(m_lpDeSeralizeHeader,nLen-1,lpBuffer+1,nLen-1);
			m_lpCurrentHeader = (char*)m_lpDeSeralizeHeader + nLen -1;
			m_status = S_NotEnoughHeader;
			return ;
		}
		//够一个包头
		memcpy_s(m_lpDeSeralizeHeader,Message_Header_Size,lpBufferCurrent + 1,Message_Header_Size);
		lpBufferCurrent = lpBufferCurrent + 1 + Message_Header_Size;
		int nDataLength = m_lpDeSeralizeHeader->packagelength;
		ResetDeserializeBuffer(nDataLength);
		if ( nDataLength == nLen - Message_Header_Size - 2)//刚好够一个完整的数据包
		{
			memcpy_s(m_lpDeSerializeBuf,nDataLength,lpBufferCurrent,nDataLength);
			lpBufferCurrent = lpBufferCurrent + nDataLength;
			if ( tail != lpBufferCurrent[0] ) //判断包尾的标识符
			{
				//非法的数据包
				memset(m_lpDeSerializeBuf,0,m_nDeSeerializeBufferSize);
				m_status = S_PackageOK;
				return ;
			}
			else
			{
				//调用回调函数,通知收到了完整的数据包
				if( NULL != m_lpFuncFullRecv )
				{
					m_lpFuncFullRecv(nClientID,m_lpDeSerializeBuf,m_nDeSeerializeBufferSize,m_lpFullRecvUserData);
				}
			}
			m_status = S_PackageOK;
			return ;
		}
		if ( nDataLength > int(nLen - Message_Header_Size - 1) )  //数据包还没有完全接收
		{
			memcpy_s(m_lpDeSerializeBuf,nLen - Message_Header_Size - 1,lpBufferCurrent,nLen - Message_Header_Size - 1);
			m_lpCurrentBuffer = m_lpCurrentBuffer + nLen - Message_Header_Size - 1;
			m_status = S_NotComplete;
			return ;
		}
		if ( nDataLength ==int(nLen - Message_Header_Size - 1) )  //还有一个包尾验证字节在下一个缓冲区
		{
			memcpy_s(m_lpDeSerializeBuf,nLen - Message_Header_Size - 1,lpBufferCurrent,nLen - Message_Header_Size - 1);
			m_lpCurrentBuffer = m_lpCurrentBuffer + nLen - Message_Header_Size - 1;
			m_status = S_NotComplete;
			return ;
		}
		if ( nDataLength < int(nLen - Message_Header_Size - 1) )
		{
			memcpy_s(m_lpDeSerializeBuf,nDataLength,lpBufferCurrent,nDataLength);
			m_status = S_PackageOK;
			lpBufferCurrent = lpBufferCurrent + nDataLength;
			unsigned char tempTail = lpBufferCurrent[0];
			if ( tail != tempTail )
			{
				int a = 10;
				//非法包
				memset(m_lpDeSerializeBuf,0,m_nDeSeerializeBufferSize);
			}
			else
			{
				if( NULL != m_lpFuncFullRecv )
				{
					m_lpFuncFullRecv(nClientID,m_lpDeSerializeBuf,m_nDeSeerializeBufferSize,m_lpFullRecvUserData);
				}
			}
			lpBufferCurrent = lpBufferCurrent + 1;
			m_status = S_PackageOK;
			int nLeft = nLen - Message_Header_Size - 2 - nDataLength;
			int noffset = lpBufferCurrent - lpBuffer;
			return DeSerialize(nClientID,lpBufferCurrent,nLeft);
		}
	}
	else if ( m_status == S_NotEnoughHeader )
	{
		int nHeaderLeft = Message_Header_Size - (m_lpCurrentHeader - (char*)m_lpDeSeralizeHeader) ;
		if ( nLen < nHeaderLeft )
		{
			memcpy_s(m_lpCurrentHeader,nLen,lpBuffer,nLen);
			m_status = S_NotEnoughHeader;
			return ;
		}
		else if ( nLen == nHeaderLeft )
		{
			memcpy_s(m_lpCurrentHeader,nLen,lpBuffer,nLen);
			int nDataLength = m_lpDeSeralizeHeader->packagelength;
			ResetDeserializeBuffer(nDataLength);
			m_status = S_NotComplete;
			return ;
		}
		else if ( nLen > nHeaderLeft )
		{
			memcpy_s(m_lpCurrentHeader,nHeaderLeft,lpBuffer,nHeaderLeft);
			int nDataLength = m_lpDeSeralizeHeader->packagelength;
			ResetDeserializeBuffer(nDataLength);
			m_status = S_NotComplete;
			const char *lpCurrentBuffer = lpBuffer + nHeaderLeft;
			DeSerialize(nClientID,(const char*)lpCurrentBuffer,nLen-nHeaderLeft);
			return ;
		}
	}
	else if ( m_status == S_NotComplete )
	{
		int nRead = m_lpCurrentBuffer - m_lpDeSerializeBuf;   //已经读取的部分
		int nLeft = m_nDeSeerializeBufferSize - nRead;		//还剩余没有读取的部分
		if ( nLeft == 0 )//这个时候第一个字节是上一个包的包尾，判断一下包尾的标志
		{
			if ( tail != lpBuffer[0] )
			{
				//非法包，清空保存的缓冲区
				memset(m_lpDeSerializeBuf,0,m_nDeSeerializeBufferSize);
			}
			else
			{
				if ( NULL != m_lpFuncFullRecv )
				{
					m_lpFuncFullRecv(nClientID,m_lpDeSerializeBuf,m_nDeSeerializeBufferSize,m_lpFullRecvUserData);
				}
			}
			m_status = S_PackageOK;
			const char* lpTemp = lpBuffer + 1;
			DeSerialize(nClientID,lpTemp,nLen -1);
			return ;
		}
		if ( nLen < nLeft )
		{
			memcpy_s(m_lpCurrentBuffer,nLen,lpBuffer,nLen);
			m_lpCurrentBuffer = m_lpCurrentBuffer + nLen;
			return ;
		}
		if ( nLen == nLeft )
		{
			memcpy_s(m_lpCurrentBuffer,nLen,lpBuffer,nLen);
			m_lpCurrentBuffer = m_lpCurrentBuffer + nLen;
			return ;
		}
		if ( nLen > nLeft )
		{
			memcpy_s(m_lpCurrentBuffer,nLeft,lpBuffer,nLeft);
			m_lpCurrentBuffer = m_lpCurrentBuffer + nLeft;
			const char* lpCurrentBuf = lpBuffer + nLeft;
			//判断下包尾
			if ( tail == lpCurrentBuf[0] )
			{
				if ( NULL != m_lpFuncFullRecv )
				{
					m_lpFuncFullRecv(nClientID,m_lpDeSerializeBuf,m_nDeSeerializeBufferSize,m_lpFullRecvUserData);
				}
			}else
			{
				memset(m_lpDeSerializeBuf,0,m_nDeSeerializeBufferSize);
			}
			m_status = S_PackageOK;
			const char* lpTemp = lpCurrentBuf + 1;
			DeSerialize(nClientID,lpTemp,nLen - nLeft - 1);
			return ;
		}
	}
}