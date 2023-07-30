#define _WINSOCKAPI_
#include "Splash_XPng.h"
#include "Splash_XInflate.h"

/* 
                           -= 작업 기록 =-

- 2010/05/06 코딩 시작
- 2010/05/10 24bit, 32bit 처리 및 필터 일부 구현
- 2010/05/11 FILTER_AVERAGE 구현. 1차 구현 완료
- 2010/05/13 최적화
	- 최적화전: 6780ms (XDib: 2300ms)
	- 멤버 변수를 로컬 변수화 : 5650ms
	- FILTER_SUB, 24비트 에서 테이블 참조 안하도록 수정 : 4860ms
	- FILTER_UP, 24비트 에서 테이블 참조 안하도록 수정 : 3150ms
	- FILTER_PAETH , 24비트 에서 테이블 참조 안하도록 수정 : 2800ms
	- 24BPP, 포인터 이동을 한번에 3바이트씩 : 2720ms
	- 최적화후: XDib(libpng) 대비 120~130% 정도 시간 걸림
- 2010/10/28
	- IDAT 가 여러개일때 처리못하던 버그 수정
	- ALPHA 채널 처리를 제대로 못하던 버그 수정
- 2010/11/01
	- 256 컬러(COLORTYPE_COLOR_PAL) 지원 추가
	- m_filter_b_dummy 추가
*/


#define DOERR(err)						{m_err = err; return FALSE;}
#define BE2HOST32(a)					((((a)&0xff)<<24)+(((a>>8)&0xff)<<16)+(((a>>16)&0xff)<<8)+(((a>>24)&0xff)))
#define BE2HOST16(a)					(((a)&0xff)<<8)+(((a>>8)&0xff))
#define XPNGFOURCC(ch0, ch1, ch2, ch3)  ((UINT32)(BYTE)(ch0) | ((UINT32)(BYTE)(ch1) << 8) | ((UINT32)(BYTE)(ch2) << 16) | ((UINT32)(BYTE)(ch3) << 24 ))
#define XPNG_ABS(x)						(x)< 0 ?  -(x) : (x)


#ifndef _XPNG_NO_CRC
	static const DWORD _CRC32[256] = {0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,      0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,      0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,      0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,      0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,      0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,      0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,      0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,      0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,      0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,      0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,      0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,      0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,      0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,      0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,      0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,      0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,      0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,      0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,      0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,      0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,      0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,      0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,      0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,      0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,      0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,      0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,      0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,      0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,      0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,      0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,      0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,      0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,      0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,      0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,      0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,      0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,      0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d };
	#define DO1(buf) crc = _CRC32[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
	#define DO8(buf)  DO1(buf); DO1(buf); DO1(buf); DO1(buf); DO1(buf); DO1(buf); DO1(buf); DO1(buf); 
	static UINT32 CRC32(UINT32 crc, const BYTE *buf, UINT len)
	{
		if (buf == NULL) return 0L;
		crc = crc ^ 0xffffffffL;
		while (len >= 8) {DO8(buf);len -= 8;}
		if(len) do {DO1(buf);} while (--len);
		return crc ^ 0xffffffffL;
	}
#endif

// RGB BGR 변환 테이블
static const int	colorMapTable_RGBA_2_BGRA[] = {2, 0, -2, 0};
static const int	colorMapTable_RGB_2_BGR[] = {2, 0, -2};


// 청크 헤더 구조체
struct XPngChunkHeader
{
	UINT32	length;							// chunk data 필드의 길이
	UINT32	chunkType;
};



XPng::XPng()
{
	Init();
}

XPng::~XPng()
{
	if(m_palette)
		free(m_palette);

	if(m_trans)
		free(m_trans);

	if(m_pInflate)
		delete m_pInflate;

	if(m_filter_b_dummy)
		free(m_filter_b_dummy);
}

void XPng::Init()
{
	m_pStream = NULL;
	m_err = XPNG_ERR_OK;
#ifndef _XPNG_NO_CRC
	m_crc32 = 0;
#endif

	m_pInflate = NULL;
	m_bEnd = FALSE;

	m_imageBuffer = NULL;
	m_imageStride = 0;
	m_imageWidthByte = 0;
	m_imageBytePerPixel = 0;

	m_imageCurBuffer = NULL;
	m_imageCurWidthByte = 0;
	m_imagePaddingByte = 0;

	m_bFirst = TRUE;

	m_ColorMapTableCur = NULL;
	m_ColorMapTablePos = 0;
	m_ColorMapTableLen = 0;

	m_scanline_before = NULL;
	m_scanline_current = NULL;

	m_filter = 0;
	m_filter_b = NULL;
	m_palette = NULL;
	m_trans = NULL;

	m_filter_b_dummy = NULL;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///         메모리 해제하고 닫기
/// @param  
/// @return 
/// @date   Thursday, May 06, 2010  1:05:19 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
void XPng::Close()
{
	if(m_pInflate)
		delete m_pInflate;
	m_pInflate = NULL;

	if(m_palette)
		free(m_palette);
	m_palette = NULL;

	if(m_trans)
		free(m_trans);
	m_trans = NULL;

	if(m_filter_b_dummy)
		free(m_filter_b_dummy);
	m_filter_b_dummy = NULL;


	Init();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         스트림으로 파일의 헤더 정보를 읽는다.
/// @param  
/// @return 
/// @date   Thursday, May 06, 2010  11:42:04 AM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XPng::Open(XPngStream* pStream)
{
	Close();

	m_pStream = pStream;


	//////////////////////////////////
	//
	// SIGNATURE 확인  RFC 3.1
	BYTE	signature[LEN_PNG_SIGNATURE];

	if(m_pStream->Read(signature, sizeof(signature))==FALSE)
		DOERR(XPNG_ERR_NOT_PNG_FILE);
	
	if(!(signature[0]==0x89 && signature[1]==0x50 && signature[2]==0x4e && signature[3]==0x47 &&
		 signature[4]==0x0d && signature[5]==0x0a && signature[6]==0x1a && signature[7]==0x0a))
		DOERR(XPNG_ERR_NOT_PNG_FILE);


	//////////////////////////////////
	//
	// IHDR 읽기
	// IHDR 은 맨 처음에 나와야 한다. (IEND 는 맨 나중에) 
	XPngChunkHeader	ihdr;
	if(ReadChunkHeader(&ihdr)==FALSE || ihdr.chunkType!=XPNGFOURCC('I', 'H', 'D', 'R'))
		DOERR(XPNG_ERR_CANT_READ_HEADER);

	if(	ReadUINT32(m_header.width)==FALSE ||
		ReadUINT32(m_header.height)==FALSE ||
		ReadUINT8(m_header.bitDepth)==FALSE ||
		ReadUINT8(m_header.colorType)==FALSE ||
		ReadUINT8(m_header.compressionMethod)==FALSE ||
		ReadUINT8(m_header.filterMode)==FALSE ||
		ReadUINT8(m_header.interlaceMethod)==FALSE)
		DOERR(XPNG_ERR_CANT_READ_HEADER);

	if(CheckCRC()==FALSE)
		DOERR(XPNG_ERR_CANT_READ_HEADER);

	//////////////////////////////////
	//
	// 헤더 내용 체크하기 TR 11.2.1
	//
	if(m_header.width==0 || m_header.height==0)
		DOERR(XPNG_ERR_CANT_READ_HEADER);

	if( !(m_header.bitDepth==1 || m_header.bitDepth==2 || m_header.bitDepth==4 || m_header.bitDepth==8 || m_header.bitDepth==16))
		DOERR(XPNG_ERR_CANT_READ_HEADER);

	if( !(m_header.colorType==COLORTYPE_COLOR_ALPHA || m_header.colorType==COLORTYPE_COLOR || m_header.colorType==COLORTYPE_COLOR_PAL))
		DOERR(XPNG_ERR_UNSUPPORTED_FORMAT);

	if(m_header.bitDepth!=8)
		DOERR(XPNG_ERR_UNSUPPORTED_FORMAT);

	if(m_header.compressionMethod!=0)				// 압축방식 0 는 deflate
		DOERR(XPNG_ERR_CANT_READ_HEADER);

	if(m_header.filterMode!=0)						// 0 이외에 정의된 값 없음
		DOERR(XPNG_ERR_CANT_READ_HEADER);

	if(m_header.interlaceMethod!=0)					// 0 이면 non interlace, 1 이면 Adam7 Interlace
		DOERR(XPNG_ERR_UNSUPPORTED_FORMAT);


	//////////////////////////////////
	//
	// 나머지 초기화
	//
	m_pInflate = new XInflate;
	m_pInflate->Init();


	m_ColorMapTablePos = 0;
	m_bPaletted = FALSE;

	if(m_header.colorType==COLORTYPE_COLOR_ALPHA)
	{
		m_ColorMapTableCur = colorMapTable_RGBA_2_BGRA;
		m_ColorMapTableLen = sizeof(colorMapTable_RGBA_2_BGRA) / sizeof(colorMapTable_RGBA_2_BGRA[0]);	// 결국은 4
	}
	else if(m_header.colorType==COLORTYPE_COLOR)
	{
		m_ColorMapTableCur = colorMapTable_RGB_2_BGR;
		m_ColorMapTableLen = sizeof(colorMapTable_RGB_2_BGR) / sizeof(colorMapTable_RGB_2_BGR[0]);		// 결국은 3
	}
	else if(m_header.colorType==COLORTYPE_COLOR_PAL)
	{
		// 빨레트를 쓴다. ALPHA 는 없다. 그래서 RGB 2 BGR 사용
		m_ColorMapTableCur = colorMapTable_RGB_2_BGR;
		m_ColorMapTableLen = sizeof(colorMapTable_RGB_2_BGR) / sizeof(colorMapTable_RGB_2_BGR[0]);		// 결국은 3
		m_bPaletted = TRUE;
	}



	return TRUE;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///         청크헤더(8바이트) 읽기
/// @param  
/// @return 
/// @date   Thursday, May 06, 2010  11:41:57 AM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XPng::ReadChunkHeader(XPngChunkHeader* chunkHeader)
{
	// 길이
	if(ReadUINT32(chunkHeader->length)==FALSE)
		return FALSE;

	// CRC 초기화. length 는 crc 정보에 포함이 안된다.
	CRCInit();

	// type
	if(m_pStream->Read((BYTE*)&(chunkHeader->chunkType), sizeof(UINT32))==FALSE)
		{{} return FALSE;}

#ifndef _XPNG_NO_CRC
	// CRC 
	m_crc32 = CRC32(m_crc32, (BYTE*)&(chunkHeader->chunkType), sizeof(UINT32));
#endif

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         4바이트 정수 읽기
/// @param  
/// @return 
/// @date   Thursday, May 06, 2010  11:42:55 AM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XPng::ReadUINT32(UINT32& value)
{
	if(m_pStream->Read((BYTE*)&value, sizeof(UINT32))==FALSE)
	{{} return FALSE;}

#ifndef _XPNG_NO_CRC
	// CRC 
	m_crc32 = CRC32(m_crc32, (BYTE*)&value, sizeof(UINT32));
#endif

	// 엔디안 처리
	value = BE2HOST32(value);

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         1바이트 읽기
/// @param  
/// @return 
/// @date   Thursday, May 06, 2010  12:17:32 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XPng::ReadUINT8(BYTE& value)
{
	if(m_pStream->Read((BYTE*)&value, sizeof(BYTE))==FALSE)
	{{} return FALSE;}

#ifndef _XPNG_NO_CRC
	// CRC 
	m_crc32 = CRC32(m_crc32, &value, sizeof(BYTE));
#endif

	return TRUE;
}

void XPng::CRCInit()
{
#ifndef _XPNG_NO_CRC
	m_crc32 = 0;
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         청크 맨 마지막의 crc 를 읽어서 체크한다.
/// @param  
/// @return 
/// @date   Thursday, May 06, 2010  12:15:54 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XPng::CheckCRC()
{
	UINT32	crc32;

	if(m_pStream->Read((BYTE*)&crc32, sizeof(UINT32))==FALSE)
	{{} return FALSE;}

#ifndef _XPNG_NO_CRC

	// 엔디안 처리
	crc32 = BE2HOST32(crc32);

	// CRC 정보가 동일한가?
	if(crc32!=m_crc32)
	{{} return FALSE;}

#endif

	return TRUE;
}




////////////////////////////////////////////////////////////////////////////////////////////////////
///         이미지 데이터를 읽는다
/// @param  
/// @return 
/// @date   Thursday, May 06, 2010  12:32:25 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XPng::Decode()
{
	if(m_bEnd) {{} return FALSE;}

	XPngChunkHeader	chunkHeader;
	BOOL			bSkipChunk;

	for(;;)
	{
		if(ReadChunkHeader(&chunkHeader)==FALSE)
			DOERR(XPNG_ERR_CANT_READ);

		bSkipChunk = FALSE;

		switch(chunkHeader.chunkType)
		{
		case XPNGFOURCC('s', 'R', 'G', 'B' ) :
		case XPNGFOURCC('g', 'A', 'M', 'A' ) :
		case XPNGFOURCC('p', 'H', 'Y', 's' ) :
		case XPNGFOURCC('t', 'E', 'X', 't' ) :
		case XPNGFOURCC('t', 'I', 'M', 'E' ) :
		case XPNGFOURCC('i', 'C', 'C', 'P' ) :
		case XPNGFOURCC('s', 'B', 'I', 'T' ) :
		case XPNGFOURCC('b', 'K', 'G', 'D' ) :
		case XPNGFOURCC('o', 'F', 'F', 's' ) :
		case XPNGFOURCC('p', 'C', 'A', 'L' ) :
		case XPNGFOURCC('s', 'T', 'E', 'R' ) :
		case XPNGFOURCC('v', 'p', 'A', 'g' ) :
		case XPNGFOURCC('z', 'T', 'X', 't' ) :
		case XPNGFOURCC('c', 'H', 'R', 'M' ) :
			bSkipChunk = TRUE;					// 무시한다.
			break;

		case XPNGFOURCC('I', 'D', 'A', 'T' ) :
			if(ProcessIDATChunk(chunkHeader.length)==FALSE)
			{{} return FALSE;}
			break;
			
		case XPNGFOURCC('I', 'E', 'N', 'D' ) :
			m_bEnd = TRUE;
			// 정상 종료
			return TRUE;

		case XPNGFOURCC('P', 'L', 'T', 'E' ) :
			if(ProcessPalette(chunkHeader.length)==FALSE)
			{{} return FALSE;}
			break;

		case XPNGFOURCC('t', 'R', 'N', 'S' ) :
			if(ProcessTrans(chunkHeader.length)==FALSE)
			{{} return FALSE;}
			break;

		default :
			{}
			bSkipChunk = TRUE;					// 무시한다.
			break;
		}

		if(bSkipChunk)
		{
			// skip chunk;
			if(	m_pStream->SkipData(chunkHeader.length)==FALSE ||		// chunk data
				m_pStream->SkipData(sizeof(UINT32))==FALSE)				// chunk crc;
				DOERR(XPNG_ERR_CANT_READ);
		}
	}

	return FALSE;
}


void XPng::SetDecodeData(BYTE* imageBuffer, int stride, int paddingByte, int bytePerPixel)
{
	m_imageBuffer = imageBuffer;
	m_imageStride = stride;
	m_imageWidthByte = stride - paddingByte;
	m_imageBytePerPixel = bytePerPixel;
	m_imagePaddingByte = paddingByte;

	m_imageCurBuffer = imageBuffer;
	m_imageCurWidthByte = m_imageWidthByte;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         IDAT 청크를 처리한다.
/// @param  
/// @return 
/// @date   Thursday, May 06, 2010  4:22:08 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XPng::ProcessIDATChunk(int dataLen)
{
	// 처음 압축 해제 시작인 경우
	if(m_bFirst)
	{
		BYTE	method;
		BYTE	flag;

		// RFC1950 2.2 참조
		if(	ReadUINT8(method)==FALSE ||
			ReadUINT8(flag)==FALSE)
			DOERR(XPNG_ERR_CANT_READ);

		// method, flag
		dataLen -= 2;

		m_bFirst = FALSE;
	}

	// 멤버 변수를 로컬 변수로
	BYTE*		imageCurBuffer = m_imageCurBuffer;
	int			imageCurWidthByte = m_imageCurWidthByte;
	int			ColorMapTablePos = m_ColorMapTablePos;
	BYTE		filter = m_filter;
	BYTE		filter_a[RGBA];
	BYTE*		filter_b = m_filter_b;	
	BYTE		filter_c[RGBA];
	*(UINT32*)filter_a = *(UINT32*)m_filter_a;
	*(UINT32*)filter_c = *(UINT32*)m_filter_c;


	// 로컬 변수
	int			ColorMapTableLen = m_ColorMapTableLen;
	const BYTE*	outBuf;
	UINT32		outBufLen;
	int			imageWidthByte = m_imageWidthByte;
	int			imagePaddingByte = m_imagePaddingByte;
	const int*	ColorMapTableCur = m_ColorMapTableCur;


	// 필터용 더미 버퍼
	if(m_filter_b_dummy==NULL)
		m_filter_b_dummy = (BYTE*)calloc(1, m_imageStride);


	// 입력 버퍼 구하기
	BYTE* data = m_pStream->GetBuffer(dataLen);
	if(data==NULL)
		DOERR(XPNG_ERR_CANT_READ);

	// 데이타 압축 해제
	if(m_pInflate->Inflate(data, dataLen)!=XINFLATE_ERR_OK)
		DOERR(XPNG_ERR_CANT_READ);

#ifndef _XPNG_NO_CRC
	// crc 체크..
	m_crc32 = CRC32(m_crc32, data, dataLen);
#endif
	if(CheckCRC()==FALSE)
		DOERR(XPNG_ERR_INVALID_CRC);

	// 디코딩 결과 저장
	outBuf = m_pInflate->GetOutBuffer();
	outBufLen = m_pInflate->GetOutBufferLen();


	// 빨레트 처리 체크하기
	if(m_bPaletted)
	{
		if(m_palette==NULL || m_trans==NULL)
			DOERR(XPNG_ERR_UNSUPPORTED_FORMAT);		// 빨레트 테이블이 존재하지 않는다!.
	}



////////////////////////////////////////////////
//
// 중복 되는 부분 - 속도를 위해서 매크로로 처리
//
#define MOVETONEXTBUFFER													\
		imageCurBuffer++;													\
		imageCurWidthByte--;												\
		ColorMapTablePos = (ColorMapTablePos+1) % ColorMapTableLen;			\
																			\
		outBuf++;															\
		outBufLen--;														\
																			\
		if(imageCurWidthByte==0)											\
		{																	\
			imageCurBuffer += imagePaddingByte;								\
			imageCurWidthByte = imageWidthByte;								\
			break;															\
		}

#define MOVETONEXTBUFFER_4_PAL												\
		imageCurBuffer+=4;													\
		imageCurWidthByte-=4;												\
																			\
		outBuf++;															\
		outBufLen--;														\
																			\
		if(imageCurWidthByte==0)											\
		{																	\
			imageCurBuffer += imagePaddingByte;								\
			imageCurWidthByte = imageWidthByte;								\
			break;															\
		}


// 테이블 처리를 하지 않는다.
#define MOVETONEXTBUFFER_FAST(len)											\
		imageCurWidthByte-=len;												\
		outBufLen-=len;														\
		outBuf +=len ;														\
		imageCurBuffer += len;
				


	// 출력 버퍼에 쓰기.
	while(outBufLen)
	{
		// 매 스캔라인 첫번째 바이트는 filter 정보이다.
		if(imageCurWidthByte == imageWidthByte)
		{
			filter = *outBuf;

			// 필터에 사용할 변수 초기화
			*((UINT32*)filter_a) = 0;
			*((UINT32*)filter_c) = 0;
			filter_b = m_scanline_current;
			if(filter_b==NULL)
				filter_b = m_filter_b_dummy;			// NULL인 경우 임시 버퍼로 포워딩 해준다..

			// 기본 초기화
			ColorMapTablePos = 0;

			// 이전 스캔라인 기억 처리
			m_scanline_before = m_scanline_current;
			m_scanline_current = imageCurBuffer;

			outBuf++;
			outBufLen--;
		}

		// 영상 데이타 복사
		switch(filter)
		{
		case FILTER_NONE :

			if(m_bPaletted)
			{
				BYTE palPixel;
				while(outBufLen)
				{
					palPixel = *outBuf;

					// 빨레트를 참고해서 RGBA 값 세팅하기
					*(imageCurBuffer + 2) = m_palette[palPixel * 3 + 0];		// R
					*(imageCurBuffer + 1) = m_palette[palPixel * 3 + 1];		// G
					*(imageCurBuffer + 0) = m_palette[palPixel * 3 + 2];		// B
					*(imageCurBuffer + 3) = m_trans[palPixel];					// A

					MOVETONEXTBUFFER_4_PAL
				}
			}
			else
			{
				if(ColorMapTablePos==0)
				{
					if(ColorMapTableCur == colorMapTable_RGB_2_BGR)
					{
						while(outBufLen>3 && imageCurWidthByte>3)
						{
							*(imageCurBuffer + 2) = *(outBuf+0);		// R
							*(imageCurBuffer + 1) = *(outBuf+1);		// G
							*(imageCurBuffer + 0) = *(outBuf+2);		// B

							MOVETONEXTBUFFER_FAST(3);
						}
					}
					else if(ColorMapTableCur == colorMapTable_RGBA_2_BGRA)
					{
						while(outBufLen>4 && imageCurWidthByte>4)
						{
							*(imageCurBuffer + 2) = *(outBuf+0);		// R
							*(imageCurBuffer + 1) = *(outBuf+1);		// G
							*(imageCurBuffer + 0) = *(outBuf+2);		// B
							*(imageCurBuffer + 3) = *(outBuf+3);		// A

							MOVETONEXTBUFFER_FAST(4);
						}
					}
				}

				while(outBufLen)
				{
					// RGB 를 BGR 로 혹은 RGBA 를 BGRA 로 바꾸기 위해서 테이블 참조
					*(imageCurBuffer + ColorMapTableCur[ColorMapTablePos]) = *outBuf;

					MOVETONEXTBUFFER
				}
			}

			break;

		case FILTER_SUB :

			if(m_bPaletted)
			{
				{}
			}
			else
			{
				// 한번에 3,4 바이트씩 복사한다.
				if(ColorMapTablePos==0)
				{
					if(ColorMapTableCur == colorMapTable_RGB_2_BGR)
					{
						while(outBufLen>3 && imageCurWidthByte>3)
						{
							filter_a[0] = *(imageCurBuffer + 2) = *(outBuf+0) + filter_a[0];	// R
							filter_a[1] = *(imageCurBuffer + 1) = *(outBuf+1) + filter_a[1];	// G
							filter_a[2] = *(imageCurBuffer + 0) = *(outBuf+2) + filter_a[2];	// B

							MOVETONEXTBUFFER_FAST(3);
						}
					}
					else if(ColorMapTableCur == colorMapTable_RGBA_2_BGRA)
					{
						while(outBufLen>4 && imageCurWidthByte>4)
						{
							filter_a[0] = *(imageCurBuffer + 2) = *(outBuf+0) + filter_a[0];	// R
							filter_a[1] = *(imageCurBuffer + 1) = *(outBuf+1) + filter_a[1];	// G
							filter_a[2] = *(imageCurBuffer + 0) = *(outBuf+2) + filter_a[2];	// B
							filter_a[3] = *(imageCurBuffer + 3) = *(outBuf+3) + filter_a[3];	// A

							MOVETONEXTBUFFER_FAST(4);
						}
					}
				}

				while(outBufLen)
				{
					// RGB 를 BGR 로 혹은 RGBA 를 BGRA 로 바꾸기 위해서 테이블 참조
					// + 왼쪽 픽셀 참조
					// + 필터값 저장(좀전 픽셀)
					filter_a[ColorMapTablePos] = *(imageCurBuffer + ColorMapTableCur[ColorMapTablePos]) = *outBuf + filter_a[ColorMapTablePos];
					MOVETONEXTBUFFER
				}
			}
			break;

		case FILTER_UP :

			if(m_bPaletted)
			{
				{}
			}
			else
			{
				if(filter_b==NULL)
					DOERR(XPNG_ERR_INVALID_DATA);		// 에러 상황이다.

				// 한번에 3, 4바이트씩 복사한다.
				if(ColorMapTablePos==0)
				{
					if(ColorMapTableCur == colorMapTable_RGB_2_BGR)
					{
						while(outBufLen>3 && imageCurWidthByte>3)
						{
							*(imageCurBuffer + 2) = *(outBuf+0) + *(filter_b + 2);	// R
							*(imageCurBuffer + 1) = *(outBuf+1) + *(filter_b + 1);	// G
							*(imageCurBuffer + 0) = *(outBuf+2) + *(filter_b + 0);	// B

							MOVETONEXTBUFFER_FAST(3);
							filter_b+= 3;
						}
					}
					else if(ColorMapTableCur == colorMapTable_RGBA_2_BGRA)
					{
						while(outBufLen>4 && imageCurWidthByte>4)
						{
							*(imageCurBuffer + 2) = *(outBuf+0) + *(filter_b + 2);	// R
							*(imageCurBuffer + 1) = *(outBuf+1) + *(filter_b + 1);	// G
							*(imageCurBuffer + 0) = *(outBuf+2) + *(filter_b + 0);	// B
							*(imageCurBuffer + 3) = *(outBuf+3) + *(filter_b + 3);	// A

							MOVETONEXTBUFFER_FAST(4);
							filter_b+= 4;
						}
					}
				}

				while(outBufLen)
				{
					// RGB 를 BGR 로 혹은 RGBA 를 BGRA 로 바꾸기 위해서 테이블 참조
					// + 이전 라인 참조
					*(imageCurBuffer + ColorMapTableCur[ColorMapTablePos]) = 
						*outBuf +
						*(filter_b + ColorMapTableCur[ColorMapTablePos]);					// 이전 스캔 라인

					filter_b++;

					MOVETONEXTBUFFER
				}
			}
			break;


		case FILTER_AVERAGE :
			if(m_bPaletted)
			{
				{}
			}
			else
			{
				if(filter_b==NULL)
					DOERR(XPNG_ERR_INVALID_DATA);		// 에러 상황이다.

				int a, b;
				BYTE* dest;

				// 한번에 3,4 픽셀 처리하기
				if(ColorMapTablePos==0)
				{
					// todo
					// 이거 테스트 해봐야 하는데.. 적당한 샘플이 없음.. average 는 별로 안쓰이는듯?
				}

				while(outBufLen)
				{
					// 참조할 데이타
					a = filter_a[ColorMapTablePos];								// 왼쪽
					b = *(filter_b + ColorMapTableCur[ColorMapTablePos]);		// 위

					// 현재 출력 지점
					dest = imageCurBuffer + ColorMapTableCur[ColorMapTablePos];

					*dest = *outBuf + (a+b)/2;

					// 좌측 업데이트
					filter_a[ColorMapTablePos] = *dest;
					// 상단 업데이트
					filter_b++;

					MOVETONEXTBUFFER
				}
			}
			break;

		case FILTER_PAETH :
			if(m_bPaletted)
			{
				{}
			}
			else
			{
				if(filter_b==NULL)
					DOERR(XPNG_ERR_INVALID_DATA);		// 에러 상황이다.

				int a, b,c,pa,pb,pc;
				int p;
				BYTE* dest;
				int cur;

/////////////////////////////////////////////////////////////////////////////
//
// PAETH 처리를 매크로화
//
#define DO_PAETH(src, dst)													\
			a = filter_a[src]; b = *(filter_b +dst); c = filter_c[src];		\
			p = a + b - c;													\
			pa = XPNG_ABS(p-a);												\
			pb = XPNG_ABS(p-b);												\
			pc = XPNG_ABS(p-c);												\
																			\
			/* 현재 출력 지점 */											\
			dest = imageCurBuffer + dst;									\
																			\
			/* 디코딩된 값 */												\
			cur = *(outBuf+src);											\
																			\
			/* 합치기 */													\
			if(pa<=pb && pa<=pc)	*dest = cur + a;						\
			else if(pb<=pc)			*dest = cur + b;						\
			else					*dest = cur + c;						\
																			\
			/* 필터 업데이트 */												\
			filter_a[src] = *dest;											\
			filter_c[src] = b;


				// 한번에 3,4 픽셀 처리하기
				if(ColorMapTablePos==0)
				{
					if(ColorMapTableCur == colorMapTable_RGB_2_BGR)
					{
						while(outBufLen>3 && imageCurWidthByte>3)
						{
							DO_PAETH(0, 2);		// R
							DO_PAETH(1, 1);		// G
							DO_PAETH(2, 0);		// B

							// 다음 픽셀로
							MOVETONEXTBUFFER_FAST(3);
							filter_b+= 3;
						}
					}
					else if(ColorMapTableCur == colorMapTable_RGBA_2_BGRA)
					{
						while(outBufLen>4 && imageCurWidthByte>4)
						{
							DO_PAETH(0, 2);		// R
							DO_PAETH(1, 1);		// G
							DO_PAETH(2, 0);		// B
							DO_PAETH(3, 3);		// A

							// 다음 픽셀로
							MOVETONEXTBUFFER_FAST(4);
							filter_b+= 4;
						}
					}
				}

				while(outBufLen)
				{
					// 참조할 데이타
					a = filter_a[ColorMapTablePos];								// 왼쪽
					b = *(filter_b + ColorMapTableCur[ColorMapTablePos]);		// 위
					c = filter_c[ColorMapTablePos];								// 왼쪽 위

					p = a + b - c;

					pa = XPNG_ABS(p-a);
					pb = XPNG_ABS(p-b);
					pc = XPNG_ABS(p-c);


					// 현재 출력 지점
					dest = imageCurBuffer + ColorMapTableCur[ColorMapTablePos];

					// 디코딩된 값
					cur = *outBuf;


					// 합치기
					if(pa<=pb && pa<=pc)
						*dest = cur + a;
					else if(pb<=pc)
						*dest = cur + b;
					else 
						*dest = cur + c;

					// 좌측 업데이트
					filter_a[ColorMapTablePos] = *dest;
					// 좌상단 업데이트
					filter_c[ColorMapTablePos] = b;
					// 상단 업데이트
					filter_b++;

					MOVETONEXTBUFFER
				}
			}
			break;
		default :
			{}
			return FALSE;
		}
	}

	// 로컬 변수를 멤버 변수로 되돌리기
	m_imageCurBuffer = imageCurBuffer;
	m_imageCurWidthByte = imageCurWidthByte;
	m_ColorMapTablePos = ColorMapTablePos ;

	m_filter = filter;
	m_filter_b = filter_b;
	*(UINT32*)m_filter_a = *(UINT32*)filter_a;
	*(UINT32*)m_filter_c = *(UINT32*)filter_c;
	
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         PLTE 를 처리한다.
/// @param  
/// @return 
/// @date   Friday, October 29, 2010  5:37:35 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XPng::ProcessPalette(int dataLen)
{
	if(dataLen!=LEN_PALETTE) 
		DOERR(XPNG_ERR_CANT_READ_HEADER);

	// 메모리 잡고
	if(m_palette==NULL)
		m_palette = (BYTE*)malloc(LEN_PALETTE);

	// 읽기
	if(m_pStream->Read(m_palette, LEN_PALETTE)==FALSE)
		DOERR(XPNG_ERR_CANT_READ);

#ifndef _XPNG_NO_CRC
	// crc 업데이트
	m_crc32 = CRC32(m_crc32, m_palette, LEN_PALETTE);
#endif

	// crc 체크
	if(CheckCRC()==FALSE)
		DOERR(XPNG_ERR_INVALID_CRC);

	return TRUE;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///         trans 처리
/// @param  
/// @return 
/// @date   Friday, October 29, 2010  5:54:16 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XPng::ProcessTrans(int dataLen)
{
	if(dataLen!=LEN_TRANS) 
		DOERR(XPNG_ERR_CANT_READ_HEADER);

	// 메모리 잡고
	if(m_trans==NULL)
		m_trans = (BYTE*)malloc(LEN_TRANS);

	// 읽기
	if(m_pStream->Read(m_trans, LEN_TRANS)==FALSE)
		DOERR(XPNG_ERR_CANT_READ);

#ifndef _XPNG_NO_CRC
	// crc 업데이트
	m_crc32 = CRC32(m_crc32, m_trans, LEN_TRANS);
#endif

	// crc 체크
	if(CheckCRC()==FALSE)
		DOERR(XPNG_ERR_INVALID_CRC);

	return TRUE;
}

