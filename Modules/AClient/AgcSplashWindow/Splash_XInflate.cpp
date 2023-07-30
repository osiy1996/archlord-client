#define _WINSOCKAPI_
#include "Splash_XInflate.h"

/**********************************************************************************

                                    -= history =-

* 2010/4/5 �ڵ� ����
* 2010/4/9 1�� �ϼ�
	- �ӵ� �� (34.5MB ¥�� gzip ����, Q9400 CPU)
		. zlib : 640ms
		. xinflate : 1900ms

* 2010/4/29 
	- �ӵ� ��� ����ȭ 
		. �Ϻ� �Լ� ȣ���� ��ũ��ȭ ��Ű�� : 1900ms
		. ��Ʈ ��Ʈ���� ���� ����ȭ ��Ű��: 1800ms
		. Write �Լ� ��ũ��ȭ: 1750ms
		. Window ��� ����ȭ: 1680ms
		. Window ��ũ��ȭ + ���� ����ȭ : 1620ms
		. InputBuffer ���� ����ȭ : 1610ms
		. ��� ������ ���� ����ȭ : 1500ms;
		. while() �������� m_bCompleted �� ���� : 1470ms;
		. while() ���������� m_error �� ���� : 1450ms
		. m_state ���� ����ȭ : 1450ms
		. m_windowLen, m_windowDistCode ���� ����ȭ : 1390ms

	- ���� ����ȭ�� �ӵ� ���� �� (34.5MB ¥�� gzip ����, Q9400 CPU)
		. zlib : 640MS
		. Halibut : 1750ms
		. XInflate : 1370ms

* 2010/4/30
	- �ڵ� ����
	- v1.0 ���� ����

* 2010/5/11
	- static table �� �ʿ��Ҷ��� �����ϵ��� ����
	- ����ȭ
		. ����ȭ �� : 1340ms
		. CHECK_TABLE ���� : 1330ms
		. FILLBUFFER() ����: 1320ms
		. table�� ��ũ�� ����Ʈ���� �迭�� ���� : 1250ms

* 2010/5/12
	- ����ȭ ���
		. STATE_GET_LEN �� break ���� : 1220ms
		. STATE_GET_DISTCODE �� break ���� : 1200ms
		. STATE_GET_DIST �� break ���� : 1170ms

* 2010/5/13
	- ����ȭ ���
		. FastInflate() �� �и��� FILLBUFFER_FAST, HUFFLOOKUP_FAST ���� : 1200ms
		. ������ Ʈ�� ó�� ��� ����(���� ���̺� ������ Ʈ�� Ž���� ���ְ� �޸� ��뷮�� ���̰�) : 900ms
		. HUFFLOOKUP_FAST �ٽ� ���� : 890ms
		. WRITE_FAST ���� : 810ms
		. lz77 ������ ��½� while() �� do-while �� ���� : 800ms

* 2010/5/19
	- ��� ���۸� ���� alloc ��� �ܺ� ���۸� �̿��� �� �ֵ��� ��� �߰�
	- m_error ���� ����

* 2010/5/25
	- �ܺι��� ��� ����� ���� ����
	- direct copy �� �ణ ����

***********************************************************************************/



// DEFLATE �� ���� Ÿ��
static enum BTYPE
{
	BTYPE_NOCOMP			= 0,
	BTYPE_FIXEDHUFFMAN		= 1,
	BTYPE_DYNAMICHUFFMAN	= 2,
	BTYPE_RESERVED			= 3		// ERROR
};

// len, dist ���̺� - halibut �� deflate.c ���� ������
struct coderecord
{
    short code, extrabits;
    int min, max;
} ;

static const coderecord lencodes[] = 
{
    {257, 0, 3, 3},		{258, 0, 4, 4},		{259, 0, 5, 5},		{260, 0, 6, 6},		{261, 0, 7, 7},		{262, 0, 8, 8},		{263, 0, 9, 9},		{264, 0, 10, 10},	{265, 1, 11, 12},	{266, 1, 13, 14},	{267, 1, 15, 16},	{268, 1, 17, 18},
    {269, 2, 19, 22},	{270, 2, 23, 26},	{271, 2, 27, 30},	{272, 2, 31, 34},	{273, 3, 35, 42},	{274, 3, 43, 50},	{275, 3, 51, 58},	{276, 3, 59, 66},	{277, 4, 67, 82},	{278, 4, 83, 98},	{279, 4, 99, 114},	{280, 4, 115, 130},
    {281, 5, 131, 162},	{282, 5, 163, 194},	{283, 5, 195, 226},	{284, 5, 227, 257},	{285, 0, 258, 258},	
};

static const coderecord distcodes[] = 
{
    {0, 0, 1, 1},			{1, 0, 2, 2},			{2, 0, 3, 3},			{3, 0, 4, 4},			{4, 1, 5, 6},			{5, 1, 7, 8},			{6, 2, 9, 12},			{7, 2, 13, 16},			{8, 3, 17, 24},			{9, 3, 25, 32},			{10, 4, 33, 48},		{11, 4, 49, 64},		{12, 5, 65, 96},		{13, 5, 97, 128},		{14, 6, 129, 192},		{15, 6, 193, 256},
    {16, 7, 257, 384},		{17, 7, 385, 512},		{18, 8, 513, 768},		{19, 8, 769, 1024},		{20, 9, 1025, 1536},	{21, 9, 1537, 2048},	{22, 10, 2049, 3072},	{23, 10, 3073, 4096},	{24, 11, 4097, 6144},	{25, 11, 6145, 8192},	{26, 12, 8193, 12288},	{27, 12, 12289, 16384},	{28, 13, 16385, 24576},	{29, 13, 24577, 32768},
};

// code length 
static const unsigned char lenlenmap[] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};


// ��ƿ ��ũ��
#define HUFFTABLE_VALUE_NOT_EXIST	-1
#ifndef ASSERT
#	define ASSERT(x) {}
#endif
#define SAFE_DEL(x) if(x) {delete x; x=NULL;}
#define SAFE_FREE(x) if(x) {free(x); x=NULL;}



////////////////////////////////////////////////
//
// ��Ʈ ��Ʈ�� ��ũ��ȭ
//
#define BS_EATBIT()								\
	bits & 0x01;								\
	bitLen --;									\
	bits >>= 1;									\
	if(bitLen<0) {}

#define BS_EATBITS(count)						\
	(bits & ((1<<(count))-1));					\
	bitLen -= count;							\
	bits >>= count;								\
	if(bitLen<0) {}

#define BS_REMOVEBITS(count)					\
	bits >>= (count);							\
	bitLen -= (count);							\
	if(bitLen<0) {} 

#define BS_MOVETONEXTBYTE						\
	BS_REMOVEBITS((bitLen % 8));

#define BS_ADDBYTE(byte)						\
	bits |= (byte) << bitLen;					\
	bitLen += 8;
//
// ��Ʈ ��Ʈ�� ��ũ�� ó��
//
////////////////////////////////////////////////


////////////////////////////////////////////////
//
// ������ ��ũ��
//
#ifdef _DEBUG
#	define ADD_INPUT_COUNT		m_inputCount ++
#	define ADD_OUTPUT_COUNT		m_outputCount ++
#else
#	define ADD_INPUT_COUNT	
#	define ADD_OUTPUT_COUNT
#endif


////////////////////////////////////////////////
//
// �ݺ� �۾� ��ũ��ȭ
//
#define FILLBUFFER()							\
	while(bitLen<=24)							\
	{											\
		if(inBufferRemain==0)					\
			break;								\
		BS_ADDBYTE(*inBuffer);					\
		inBufferRemain--;						\
		inBuffer++;								\
		ADD_INPUT_COUNT;						\
	}											\
	if(bitLen<=0)								\
		goto END;


#define HUFFLOOKUP(result, pTable)								\
		/* ����Ÿ ���� */										\
		if(pTable->bitLenMin > bitLen)							\
		{														\
			result = -1;										\
		}														\
		else													\
		{														\
			pItem = &(pTable->pItem[pTable->mask & bits]);		\
			/* ����Ÿ ���� */									\
			if(pItem->bitLen > bitLen)							\
			{													\
				result = -1;									\
			}													\
			else												\
			{													\
				result = pItem->symbol;							\
				BS_REMOVEBITS(pItem->bitLen);					\
			}													\
		}


#define WRITE(byte)												\
	ADD_OUTPUT_COUNT;											\
	WIN_ADD(byte);												\
	CHECK_AND_INCREATE_OUT_BUFFER								\
	outBuffer[outBufferPos] = byte;								\
	outBufferPos++;												

#define CHECK_AND_INCREATE_OUT_BUFFER							\
	if(outBufferPos>=outBufferAlloc)							\
	{															\
		if(m_isExternalOutBuffer)/* ���۸� �ø����� ���� ��Ȳ*/	\
			DOERR(XINFLATE_ERR_INSUFFICIENT_OUT_BUFFER);		\
																\
		/* ���� �ø��� */										\
		outBufferAlloc = outBufferAlloc * 2;					\
		BYTE* temp = outBuffer;									\
		outBuffer = (BYTE*)realloc(outBuffer, outBufferAlloc);	\
		/* alloc ���� ó�� */									\
		if(outBuffer==NULL)										\
		{														\
			{}											\
			ret = XINFLATE_ERR_ALLOC_FAIL;						\
			outBuffer = temp;									\
			outBufferPos = 0; /* �׳� ó�� ��ġ�� �ǵ�����.*/	\
			outBufferAlloc /= 2;								\
			goto END;											\
		}														\
	}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              FastInflate
//


// �Է� ���۰� ������� üũ���� �ʴ´�.
#define FILLBUFFER_FAST()								\
	while(bitLen<=24)									\
	{													\
		BS_ADDBYTE(*inBuffer);							\
		inBufferRemain--;								\
		inBuffer++;										\
		ADD_INPUT_COUNT;								\
	}													\

// ��Ʈ��Ʈ�� ����Ÿ(bits + bitLen)�� ������� üũ���� �ʴ´�.
#define HUFFLOOKUP_FAST(result, pTable)					\
		pItem = &(pTable->pItem[pTable->mask & bits]);	\
		result = pItem->symbol;							\
		BS_REMOVEBITS(pItem->bitLen);					


// ��� ���۰� ������� üũ���� �ʴ´�.
#define WRITE_FAST(byte)										\
	ADD_OUTPUT_COUNT;											\
	WIN_ADD(byte);												\
	outBuffer[outBufferPos] = byte;								\
	outBufferPos++;												


////////////////////////////////////////////////
//
// ������ ��ũ��ȭ
//

#define WIN_ADD(b)												\
	windowBuf[windowPos] = b;									\
	windowPos = (windowPos+1) & (DEFLATE_WINDOW_SIZE-1);

#define WIN_GETBUF(dist)										\
	(windowBuf +  ((windowPos - dist) & (DEFLATE_WINDOW_SIZE-1)) )

//
// ������ ��ũ��ȭ
//
////////////////////////////////////////////////



/////////////////////////////////////////////////////
//
// ������ Ʈ���� ���� Ž���� �� �ְ� �ϱ� ���ؼ�
// Ʈ�� ��ü�� �ϳ��� �迭�� ����� ������.
//   
//

struct XFastHuffItem									// XFastHuffTable �� ������
{
#ifdef _DEBUG
	XFastHuffItem() 
	{
		bitLen = 0;
		symbol = HUFFTABLE_VALUE_NOT_EXIST;
	}
#endif
	int		bitLen;										// ��ȿ�� ��Ʈ��
	int		symbol;										// ��ȿ�� �ɺ�
};

class XFastHuffTable
{
public :
	XFastHuffTable()
	{
		pItem = NULL;
	}
	~XFastHuffTable()
	{
		if(pItem){ delete[] pItem; pItem=NULL;}
	}
	// �迭 ����
	void	Create(int _bitLenMin, int _bitLenMax)
	{
		if(pItem) {}							// �߻� �Ұ�

		bitLenMin = _bitLenMin;
		bitLenMax = _bitLenMax;
		mask	  = (1<<bitLenMax) - 1;					// ����ũ
		itemCount = 1<<bitLenMax;						// ���� ������ �ִ� ���̺� ũ��
		pItem     = new XFastHuffItem[itemCount];		// 2^maxBitLen ��ŭ �迭�� �����.
	}
	// �ɺ� ����Ÿ�� ������ ��ü �迭�� ä���.
	void	SetValue(int symbol, int bitLen, UINT bitCode)
	{
		/*
			���� ������ Ʈ�� ��忡�� 0->1->1 �̶�� ����Ÿ�� 'A' ���
			symbol = 'A',   bitLen = 3,   bitCode = 3  �� �Ķ���ͷ� ���޵ȴ�.

			* ���� bitstream �� �������� ������ ������ ���߿� ������ ���� �ϱ� ���ؼ�
			  0->1->1 �� 1<-1<-0 ���� �����´�.

			* ���� bitLenMax �� 5 ��� �������� 1<-1<-0 �� �� �� 2bit �� ���� ������
			  00110, 01110, 10110, 11110 �� ���ؼ��� ������ �ɺ��� ������ �� �ֵ��� �����.
		*/

		UINT revBitCode = 0;
		// ������
		int i;
		for(i=0;i<bitLen;i++)
		{
			revBitCode <<= 1;
			revBitCode |= (bitCode & 0x01);
			bitCode >>= 1;
		}

		int		add2code = (1<<bitLen);		// bitLen �� 3 �̶�� add2code ���� 1000(bin) �� ����

		// �迭 ä���
		for(;;)
		{
#ifdef _DEBUG
			if(pItem[revBitCode].symbol!=  HUFFTABLE_VALUE_NOT_EXIST) 
				{}
#endif
			pItem[revBitCode].bitLen = bitLen;
			pItem[revBitCode].symbol = symbol;

			// ���� ������ bit code �� ó���ϱ� ���ؼ� ���� ��� ���Ѵ�.
			revBitCode += add2code;

			// ���� ������ ������ ��� ��� ������
			if(revBitCode >= itemCount)
				break;
		}
	}

	XFastHuffItem*	pItem;							// (huff) code 2 symbol ������, code�� �迭�� ��ġ ������ �ȴ�.
	int				bitLenMin;						// ��ȿ�� �ּ� ��Ʈ��
	int				bitLenMax;						// ��ȿ�� �ִ� ��Ʈ��
	UINT			mask;							// bitLenMax �� ���� bit mask
	UINT			itemCount;						// bitLenMax �� ���� ������ �ִ� ������ ����
};



// ��� ������ ���÷� �����ϰ� ���� ������ ��� ������ ��ȯ�ϱ�

#define	SAVE										\
	bits = m_bits;									\
	bitLen = m_bitLen;								\
	windowBuf = m_windowBuf;						\
	windowPos = m_windowPos;						\
	outBuffer = m_outBuffer;						\
	outBufferAlloc = m_outBufferAlloc;				\
	outBufferPos = m_outBufferPos;					\
	state = m_state;								\
	windowLen = m_windowLen;						\
	windowDistCode = m_windowDistCode;				\
	symbol = m_symbol;

#define RESTORE										\
	m_bits = bits;									\
	m_bitLen = bitLen;								\
	m_windowBuf = windowBuf;						\
	m_windowPos = windowPos;						\
	m_outBuffer = outBuffer;						\
	m_outBufferAlloc = outBufferAlloc;				\
	m_outBufferPos = outBufferPos;					\
	m_state = state;								\
	m_windowLen = windowLen;						\
	m_windowDistCode = windowDistCode;				\
	m_symbol = symbol;


const char* xinflate_copyright = 
"[XInflate - Copyright(C) 2010, by kippler]";


XInflate::XInflate()
{
	m_pStaticInfTable = NULL;
	m_pStaticDistTable = NULL;
	m_pDynamicInfTable = NULL;
	m_pDynamicDistTable = NULL;
	m_pCurrentTable = NULL;
	m_pCurrentDistTable = NULL;

	m_pLenLenTable = NULL;
	m_outBuffer = NULL;
	m_windowBuf = NULL;
	m_copyright = NULL;
	m_isExternalOutBuffer = FALSE;
}

XInflate::~XInflate()
{
	Free();
}

// ���� �޸� alloc ����
void XInflate::Free()
{
	SAFE_DEL(m_pStaticInfTable);
	SAFE_DEL(m_pStaticDistTable);
	SAFE_DEL(m_pDynamicInfTable);
	SAFE_DEL(m_pDynamicDistTable);
	SAFE_DEL(m_pLenLenTable);

	SAFE_FREE(m_windowBuf);


	if(m_isExternalOutBuffer==FALSE)	// ���� ����
	{
		SAFE_FREE(m_outBuffer);
	}
	else								// �ܺ� ����
		m_outBuffer = NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         ���� ���� �ʱ�ȭ
/// @param  
/// @return 
/// @date   Thursday, April 08, 2010  4:17:46 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
void XInflate::Init()
{
	SAFE_DEL(m_pLenLenTable);
	SAFE_DEL(m_pDynamicInfTable);
	SAFE_DEL(m_pDynamicDistTable);

	m_pCurrentTable = NULL;
	m_pCurrentDistTable = NULL;

	m_state = STATE_START;
	m_uncompRemain = 0;
	m_bFinalBlock = FALSE;
	m_symbol = 0;
	m_windowLen = 0;
	m_windowDistCode = 0;
	m_bCompleted = FALSE;

	BS_Init();
	Win_Init();

	m_copyright = xinflate_copyright;

#ifdef _DEBUG
	m_inputCount = 0;
	m_outputCount = 0;
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         ��� ���۸� ���ο��� alloc ������ �ʰ�, �ܺ��� ���۸� ����Ѵ�.
/// @param  
/// @return 
/// @date   Wednesday, May 19, 2010  12:44:50 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
void XInflate::SetOutBuffer(BYTE* outBuf, int outBufLen)
{
	// ������ ����ϴ� ���� ������ ���� �����.
	if(m_isExternalOutBuffer==FALSE)
	{
		SAFE_FREE(m_outBuffer);
	}

	if(outBuf)
	{
		// �ܺ� ���۸� ��� ���۷� �����ϱ�
		m_outBuffer = outBuf;
		m_outBufferAlloc = outBufLen;
		m_outBufferPos = 0;
		m_isExternalOutBuffer = TRUE;
	}
	else
	{
		// outBuf==NULL �̸� �ܺ� ���۸� ���̻� ��� ���ϴ� ����̴�.
		m_outBuffer = outBuf;
		m_outBufferAlloc = 0;
		m_outBufferPos = 0;
		m_isExternalOutBuffer = FALSE;
	}

}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         ���� ���� - ȣ���� Init() �� �ݵ�� ȣ���� ��� �Ѵ�.
/// @param  
/// @return 
/// @date   Thursday, April 08, 2010  4:18:55 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
#define	DOERR(x) { {} ret = x; goto END; }
XINFLATE_ERR XInflate::Inflate(const BYTE* inBuffer, int inBufferRemain)
{
	// ��� ���� üũ
	if(m_outBuffer==NULL)
	{
		m_outBuffer = (BYTE*)malloc(DEFAULT_ALLOC_SIZE);
		m_outBufferAlloc = DEFAULT_ALLOC_SIZE;
	}


	XINFLATE_ERR	ret = XINFLATE_ERR_OK;

	// ��� ���� ��ġ �ʱ�ȭ
	m_outBufferPos = 0;

	// �ӵ� ����� ���ؼ� ��� ������ ���÷� ����
	UINT		bits;
	int			bitLen;
	BYTE*		windowBuf;
	int			windowPos;
	BYTE*		outBuffer;
	int			outBufferAlloc;
	int			outBufferPos;
	STATE		state;
	int			windowLen;
	int			windowDistCode;
	int			symbol;

	SAVE;


	// ���� ����
	const coderecord*	rec;
	XFastHuffItem*		pItem;			// HUFFLOOKUP() ���� ���
	BYTE				byte;			// �ӽ� ����
	XINFLATE_ERR		err;


	// �̹� �Ϸ�Ǿ����� �˻�
	if(m_bCompleted)
		DOERR(XINFLATE_ERR_BLOCK_COMPLETED);

	// �ʱ�ȭ �Լ� ȣ�� �Ǿ����� �˻�
	if(m_copyright==NULL)
		DOERR(XINFLATE_ERR_NOT_INITIALIZED);


	// ���� ���鼭 ���� ����
	for(;;)
	{
		FILLBUFFER();

		switch(state)
		{
			// ��� �м� ����
		case STATE_START :
			if(bitLen<3) 
				goto END;

			// ������ �� ����
			m_bFinalBlock = BS_EATBIT();

			// ��Ÿ�� ����� 2bit 
			{
				int bType = BS_EATBITS(2);

				if(bType==BTYPE_DYNAMICHUFFMAN)
					state = STATE_DYNAMIC_HUFFMAN;
				else if(bType==BTYPE_NOCOMP)
					state = STATE_NO_COMPRESSION;
				else if(bType==BTYPE_FIXEDHUFFMAN)
					state = STATE_FIXED_HUFFMAN;
				else
					DOERR(XINFLATE_ERR_HEADER);
			}
			break;

			// ���� �ȵ�
		case STATE_NO_COMPRESSION :
			BS_MOVETONEXTBYTE;
			state = STATE_NO_COMPRESSION_LEN;
			break;

		case STATE_NO_COMPRESSION_LEN :
			// LEN
			if(bitLen<16) goto END;
			m_uncompRemain = BS_EATBITS(16);
			state = STATE_NO_COMPRESSION_NLEN;

			break;

		case STATE_NO_COMPRESSION_NLEN :
			// NLEN
			if(bitLen<16) goto END;
			{
				UINT32 nlen = BS_EATBITS(16);
				// one's complement 
				if( (nlen^0xffff) != m_uncompRemain) 
					DOERR(XINFLATE_ERR_INVALID_NOCOMP_LEN);
			}
			state = STATE_NO_COMPRESSION_BYPASS;
			break;

		case STATE_NO_COMPRESSION_BYPASS :
			// ����Ÿ ��������
			if(bitLen<8) goto END;

			//////////////////////////////////////////////
			//
			// ���� �ڵ�
			//
			/*
			{
				byte = BS_EATBITS(8);
				WRITE(byte);
			}
			m_uncompRemain--;
			*/

			if(bitLen%8!=0) {}			// �߻��Ұ�, �׳� Ȯ�ο�

			//////////////////////////////////////////////
			//
			// �Ʒ��� ������ �ڵ�. �׷��� �� ���̰� ���� ��.��
			//

			// ��Ʈ ��Ʈ���� ���� ����
			while(bitLen && m_uncompRemain)
			{
				byte = BS_EATBITS(8);
				WRITE(byte);
				m_uncompRemain--;
			}

			// ������ ����Ÿ�� ����Ʈ �״�� ����
			{
				int	toCopy, toCopy2;
				toCopy = toCopy2 = min((int)m_uncompRemain, inBufferRemain);

				/* �����ڵ�
				while(toCopy)
				{
					WRITE(*inBuffer);
					inBuffer++;
					toCopy--;
				}
				*/

				if(outBufferAlloc-outBufferPos > toCopy)
				{
					// ��� ���۰� ����� ��� - ��� ���۰� ������� ���θ� üũ���� �ʴ´�.
					while(toCopy)
					{
						WRITE_FAST(*inBuffer);
						inBuffer++;
						toCopy--;
					}
				}
				else
				{
					while(toCopy)
					{
						WRITE(*inBuffer);
						inBuffer++;
						toCopy--;
					}
				}


				m_uncompRemain-=toCopy2;
				inBufferRemain-=toCopy2;
			}
			//
			// ������ �ڵ� ��
			//
			//////////////////////////////////////////////

			if(m_uncompRemain==0)
			{
				if(m_bFinalBlock)
					state = STATE_COMPLETED;
				else
					state = STATE_START;
			}
			break;

			// ���� ������
		case STATE_FIXED_HUFFMAN :
			if(m_pStaticInfTable==NULL)
				CreateStaticTable();

			m_pCurrentTable = m_pStaticInfTable;
			m_pCurrentDistTable = m_pStaticDistTable;
			state = STATE_GET_SYMBOL;
			break;

			// ���� ��������
		case STATE_GET_LEN :
			// zlib �� inflate_fast ȣ�� ���� �䳻����
			if(inBufferRemain>FASTINFLATE_MIN_BUFFER_NEED)
			{
				if(symbol<=256)	{}

				RESTORE;
				XINFLATE_ERR result = FastInflate(&inBuffer, &inBufferRemain);
				if(result!=XINFLATE_ERR_OK)
					return result;
				SAVE;
				break;
			}

			rec = &lencodes[symbol - 257];
			if (bitLen < rec->extrabits) goto END;

			// RFC 1951 3.2.5
			// �⺻ ���̿� extrabit ��ŭ�� ��Ʈ�� ������ ���ϸ� ��¥ ���̰� ���´�
			{
				int extraLen = BS_EATBITS(rec->extrabits);
				windowLen = rec->min + extraLen;	
			}
			state = STATE_GET_DISTCODE;
			//break;	�ʿ� ����..

			// �Ÿ� �ڵ� ��������
		case STATE_GET_DISTCODE :
			HUFFLOOKUP(windowDistCode, m_pCurrentDistTable);

			if(windowDistCode<0)
				goto END;

			if(windowDistCode==30 || windowDistCode==31)			// 30 �� 31�� ���� �� ����. RFC1951 3.2.6
				DOERR(XINFLATE_ERR_INVALID_DIST);

			state = STATE_GET_DIST;

			FILLBUFFER();
			//break;		// �ʿ����

			// �Ÿ� ��������
		case STATE_GET_DIST:
			rec = &distcodes[windowDistCode];
			// DIST ���ϱ�
			if(bitLen<rec->extrabits)
				goto END;
			{
				int dist = rec->min + BS_EATBITS(rec->extrabits);

				// lz77 ���
				while(windowLen)
				{
					byte = *WIN_GETBUF(dist);
					WRITE(byte);
					windowLen--;
				}
			}
	
			state = STATE_GET_SYMBOL;

			FILLBUFFER();
			//break;		// �ʿ� ����

			// �ɺ� ��������.
		case STATE_GET_SYMBOL :
			HUFFLOOKUP(symbol, m_pCurrentTable);

			if(symbol<0) 
				goto END;
			else if(symbol<256)
			{
				byte = (BYTE)symbol;
				WRITE(byte);
				break;
			}
			else if(symbol==256)	// END OF BLOCK
			{
				if(m_bFinalBlock)
					state = STATE_COMPLETED;
				else
					state = STATE_START;
				break;
			}
			else if(symbol<286)
			{
				state = STATE_GET_LEN;
			}
			else
				DOERR(XINFLATE_ERR_INVALID_SYMBOL);		// �߻� �Ұ�

			break;

			// ���̳��� ������ ����
		case STATE_DYNAMIC_HUFFMAN :
			if(bitLen<5+5+4) 
				goto END;

			// ���̺� �ʱ�ȭ
			SAFE_DEL(m_pDynamicInfTable);
			SAFE_DEL(m_pDynamicDistTable);

			m_literalsNum  = 257 + BS_EATBITS(5);
			m_distancesNum = 1   + BS_EATBITS(5);
			m_lenghtsNum   = 4   + BS_EATBITS(4);

			if(m_literalsNum > 286 || m_distancesNum > 30)
				DOERR(XINFLATE_ERR_INVALID_LEN);

			memset(m_lengths, 0, sizeof(m_lengths));
			m_lenghtsPtr = 0;

			state = STATE_DYNAMIC_HUFFMAN_LENLEN ;
			break;

			// ������ ���� ��������.
		case STATE_DYNAMIC_HUFFMAN_LENLEN :

			if(bitLen<3) 
				goto END;

			// 3bit �� �ڵ� ������ �ڵ� ���� ���� ��������.
			while (m_lenghtsPtr < m_lenghtsNum && bitLen >= 3) 
			{
				if(m_lenghtsPtr>sizeof(lenlenmap))						// �Է°� üũ..
					DOERR(XINFLATE_ERR_INVALID_LEN);

				m_lengths[lenlenmap[m_lenghtsPtr]] = BS_EATBITS(3);
				m_lenghtsPtr++;
			}

			// �� ����������..
			if (m_lenghtsPtr == m_lenghtsNum)
			{
				// ���̿� ���� ������ ���̺� �����
				if(CreateTable(m_lengths, 19, m_pLenLenTable, err)==FALSE)
					DOERR(err);
				state = STATE_DYNAMIC_HUFFMAN_LEN;
				m_lenghtsPtr = 0;
			}
			break;

			// ����� ���� ������ m_pLenLenTable �� ���ļ� ��������.
		case STATE_DYNAMIC_HUFFMAN_LEN:

			// �� ����������
			if (m_lenghtsPtr >= m_literalsNum + m_distancesNum) 
			{
				// ���� ���̳��� ���̺� ���� (literal + distance)
				if(	CreateTable(m_lengths, m_literalsNum, m_pDynamicInfTable, err)==FALSE ||
					CreateTable(m_lengths + m_literalsNum, m_distancesNum, m_pDynamicDistTable, err)==FALSE)
					DOERR(err);

				// lenlen ���̺��� ���� ���̻� �ʿ����.
				SAFE_DEL(m_pLenLenTable);

				// ���̺� ����
				m_pCurrentTable = m_pDynamicInfTable;
				m_pCurrentDistTable = m_pDynamicDistTable;

				// ��¥ ���� ���� ����
				state = STATE_GET_SYMBOL;
				break;
			}

			{
				// ���� ���� �ڵ� ��������
				int code=-1;
				HUFFLOOKUP(code, m_pLenLenTable);

				if (code == -1)
					goto END;

				if (code < 16) 
				{
					if(m_lenghtsPtr>sizeof(m_lengths))		// �� üũ
						DOERR(XINFLATE_ERR_INVALID_LEN);

					m_lengths[m_lenghtsPtr] = code;
					m_lenghtsPtr++;
				} 
				else 
				{
					m_lenExtraBits = (code == 16 ? 2 : code == 17 ? 3 : 7);
					m_lenAddOn = (code == 18 ? 11 : 3);
					m_lenRep = (code == 16 && m_lenghtsPtr > 0 ? m_lengths[m_lenghtsPtr - 1] : 0);
					state = STATE_DYNAMIC_HUFFMAN_LENREP;
				}
			}
			break;

		case STATE_DYNAMIC_HUFFMAN_LENREP:
			if (bitLen < m_lenExtraBits)
				goto END;

			{
				int repeat = m_lenAddOn + BS_EATBITS(m_lenExtraBits);

				while (repeat > 0 && m_lenghtsPtr < m_literalsNum + m_distancesNum) 
				{
					m_lengths[m_lenghtsPtr] = m_lenRep;
					m_lenghtsPtr++;
					repeat--;
				}
			}

			state = STATE_DYNAMIC_HUFFMAN_LEN;
			break;

		case STATE_COMPLETED :
			m_bCompleted = TRUE;
			goto END;
			break;

		default :
			{}
		}
	}

END :
	// ���� ���� ��� ������ �ٽ� ����
	RESTORE;

	
	return ret;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         lengh ������ ������ code �� �����Ѵ�. 
//          RFC 1951 3.2.2
/// @param  
/// @return 
/// @date   Wednesday, April 07, 2010  1:53:36 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XInflate::CreateCodes(BYTE* lengths, int numSymbols, int* codes)
{
	int		bits;
	int		code = 0;
	int		bitLenCount[MAX_CODELEN+1];
	int		next_code[MAX_CODELEN+1];
	int		i;
	int		n;
	int		len;

	memset(bitLenCount, 0, sizeof(bitLenCount));
	for(i=0;i<numSymbols;i++)
	{
		bitLenCount[lengths[i]] ++;
	}

	bitLenCount[0] = 0;

	for(bits=1;bits<=MAX_CODELEN;bits++)
	{
		code = (code + bitLenCount[bits-1]) << 1;
		next_code[bits] = code;
	}

	for(n=0; n<numSymbols; n++)
	{
		len = lengths[n];
		if(len!=0)
		{
			codes[n] = next_code[len];
			next_code[len]++;
		}
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         codes + lengths ������ ������ ���̺��� �����.
/// @param  
/// @return 
/// @date   Wednesday, April 07, 2010  3:43:21 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XInflate::CreateTable(BYTE* lengths, int numSymbols, XFastHuffTable*& pTable, XINFLATE_ERR& err)
{
	int			codes[MAX_SYMBOLS];
	// lengths ������ ������ �ڵ����� codes ������ �����Ѵ�.
	if(CreateCodes(lengths, numSymbols, codes)==FALSE) 
	{	
		err = XINFLATE_ERR_CREATE_CODES;
		{} 
		return FALSE;
	}
	if(_CreateTable(codes, lengths, numSymbols, pTable)==FALSE)
	{
		err = XINFLATE_ERR_CREATE_TABLE;
		{} 
		return FALSE;
	}
	return TRUE;
}
BOOL XInflate::_CreateTable(int* codes, BYTE* lengths, int numSymbols, XFastHuffTable*& pTable)
{
	int		bitLenCount[MAX_CODELEN+1];
	int		symbol;
	int		bitLen;

	// bit length ���ϱ�
	memset(bitLenCount, 0, sizeof(bitLenCount));
	for(symbol=0;symbol<numSymbols;symbol++)
		bitLenCount[lengths[symbol]] ++;


	// ������ Ʈ������ ��ȿ�� �ּ� bitlen �� �ִ� bitlen ���ϱ�
	int	bitLenMax = 0;
	int	bitLenMin = MAX_CODELEN;

	for(bitLen=1;bitLen<=MAX_CODELEN;bitLen++)
	{
		if(bitLenCount[bitLen])
		{
			bitLenMax = max(bitLenMax, bitLen);
			bitLenMin = min(bitLenMin, bitLen);
		}
	}

	// ���̺� ����
	pTable = new XFastHuffTable;
	if(pTable==0) {{} return FALSE;}			// �߻� �Ұ�.
	pTable->Create(bitLenMin, bitLenMax);


	// ���̺� ä���
	for(symbol=0;symbol<numSymbols;symbol++)
	{
		bitLen = lengths[symbol];
		if(bitLen)
			pTable->SetValue(symbol, bitLen, codes[symbol]);
	}

#ifdef _DEBUG
	for(UINT i=0;i<pTable->itemCount;i++)
	{
		if(pTable->pItem[i].symbol==-1)
			{}
	}
#endif

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         static �������� ����� ���̺� �����
/// @param  
/// @return 
/// @date   Thursday, April 08, 2010  4:17:06 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
void XInflate::CreateStaticTable()
{
	BYTE		lengths[MAX_SYMBOLS];

	// static symbol ���̺� �����
	// RFC1951 3.2.6
    memset(lengths, 8, 144);
    memset(lengths + 144, 9, 256 - 144);
    memset(lengths + 256, 7, 280 - 256);
    memset(lengths + 280, 8, 288 - 280);

	XINFLATE_ERR err;
	CreateTable(lengths, MAX_SYMBOLS, m_pStaticInfTable, err);

	// static dist ���̺� �����
	// RFC1951 3.2.6
	memset(lengths, 5, 32);
	CreateTable(lengths, 32, m_pStaticDistTable, err);
}

// ��Ʈ��Ʈ�� �ʱ�ȭ
void XInflate::BS_Init()
{
	m_bits = 0;
	m_bitLen = 0;
}


// lz77 ������ ����
void XInflate::Win_Init()
{
	if(m_windowBuf==NULL)
		m_windowBuf = (BYTE*)malloc(DEFLATE_WINDOW_SIZE);
	m_windowPos = 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         �Է� ���۰� ����� ��� ���� ���ڵ��� �����Ѵ�.
/// @param  
/// @return 
/// @date   Thursday, May 13, 2010  1:43:34 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
XINFLATE_ERR XInflate::FastInflate(const BYTE** ppinBuffer, int* pinBufferRemain)
{
	XINFLATE_ERR	ret = XINFLATE_ERR_OK;

	// �Ķ���� ����
	const BYTE* inBuffer = *ppinBuffer;
	int  inBufferRemain = *pinBufferRemain;


	// �ӵ� ����� ���ؼ� ��� ������ ���÷� ����
	UINT		bits;
	int			bitLen;
	BYTE*		windowBuf;
	int			windowPos;
	BYTE*		outBuffer;
	int			outBufferAlloc;
	int			outBufferPos;
	STATE		state;
	int			windowLen;
	int			windowDistCode;
	int			symbol;

	SAVE;


	// ���� ����
	const coderecord*	rec;
	XFastHuffItem*		pItem;			// HUFFLOOKUP() ���� ���
	BYTE				byte;			// �ӽ� ����
	int					extraLen;


	// ���� ���鼭 ���� ����
	while(inBufferRemain>FASTINFLATE_MIN_BUFFER_NEED)
	{
		FILLBUFFER_FAST();

		/////////////////////////////////////
		// ���� ��������
		rec = &lencodes[symbol - 257];

		// RFC 1951 3.2.5
		// �⺻ ���̿� extrabit ��ŭ�� ��Ʈ�� ������ ���ϸ� ��¥ ���̰� ���´�
		extraLen = BS_EATBITS(rec->extrabits);
		windowLen = rec->min + extraLen;	

		
		/////////////////////////////////////
		// �Ÿ� �ڵ� ��������
		HUFFLOOKUP(windowDistCode, m_pCurrentDistTable);

		if(windowDistCode==30 || windowDistCode==31)			// 30 �� 31�� ���� �� ����. RFC1951 3.2.6
			DOERR(XINFLATE_ERR_INVALID_DIST);

		FILLBUFFER_FAST();


		/////////////////////////////////////
		// �Ÿ� ��������
		rec = &distcodes[windowDistCode];
		// DIST ���ϱ�
		int dist = rec->min + BS_EATBITS(rec->extrabits);


		/////////////////////////////////////
		// lz77 ���
		if(outBufferAlloc-outBufferPos > windowLen)
		{
			// ��� ���۰� ����� ��� - ��� ���۰� ������� ���θ� üũ���� �ʴ´�.
			do
			{
				byte = *WIN_GETBUF(dist);
				WRITE_FAST(byte);
			}while(--windowLen);
		}
		else
		{
			// ��� ���۰� ������� ���� ���
			do
			{
				byte = *WIN_GETBUF(dist);
				WRITE(byte);
			}while(--windowLen);
		}


		/////////////////////////////////////
		// �ɺ� ��������.
		for(;;)
		{
			// �Է� ���� üũ
			if(!(inBufferRemain>FASTINFLATE_MIN_BUFFER_NEED)) 
			{
				state = STATE_GET_SYMBOL;
				goto END;
			}

			FILLBUFFER_FAST();

			HUFFLOOKUP_FAST(symbol, m_pCurrentTable);

			if(symbol<0) 
			{{} goto END;}					// �߻� �Ұ�.
			else if(symbol<256)
			{
				byte = (BYTE)symbol;
				WRITE(byte);
			}
			else if(symbol==256)	// END OF BLOCK
			{
				if(m_bFinalBlock)
					state = STATE_COMPLETED;
				else
					state = STATE_START;
				// �Լ� ����
				goto END;
			}
			else if(symbol<286)
			{
				// ���� ��������� �����Ѵ�.
				break;
			}
			else
				DOERR(XINFLATE_ERR_INVALID_SYMBOL);		// �߻� �Ұ�
		}
	}

END :
	RESTORE;

	// �Ķ���� ����
	*ppinBuffer = inBuffer;
	*pinBufferRemain = inBufferRemain;

	return ret;
}

