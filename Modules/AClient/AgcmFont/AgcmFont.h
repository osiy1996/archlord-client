#ifndef __AGCMFONT_H__
#define __AGCMFONT_H__

#include <AgcModule/AgcModule.h>
#include <rwcore.h>
#include <rpworld.h>
#include <AgcSkeleton/AcDefine.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define		AGCM_FONT_MAX_TYPE_NUM			10		// Buffer�� ����� ���� (������ �޸� �� �ȸԴ´�.)
#define		AGCM_IMGFONT_MAX_TYPE_NUM		1
#define		AGCM_FONT_HASH_SIZE				19

#define		AGCM_FONT_NAME_LENGTH			64

#define		AGCM_FONT_WHITE_COLOR					0xffffffff
#define		AGCM_FONT_TRANSPARENT_WHITE_COLOR		0x50808080
#define		AGCM_FONT_TRANSPARENT_WHITE_COLOR2		0x50505050
#define		AGCM_FONT_TRANSPARENT_WHITE_COLOR3		0x28808080
#define		AGCM_FONT_TRANSPARENT_WHITE_COLOR4		0x28505050
#define		AGCM_FONT_WHITE_CLEAR_COLOR				0x00ffffff

#define		AGCM_FONT_SHADOW_ALPHA					184
#define		AGCM_FONT_SHAODW_COLOR					0xB8000000

#define		AGCM_FONT_GASIB_SIZE_15					4

enum AGCM_FONT_FLAG
{
	FF_LEFT = 1,
	FF_CENTER = 2,
	FF_RIGHT = 4,
	FF_VCENTER = 128					// vertical center align
};

typedef struct tag_TextRaster
{
	UINT16			ccode;					// ���� �ڵ�
	UINT16			ftype;					// ���� Ÿ��
	DWORD			color;					// ���� ����

	RwRaster*		Ras;
	INT32			count;					// alive count (50 ���� 1�� ����,0�Ǹ� ����)

	FLOAT			offset;					// freetype�� �׻� õ�忡 �پ� ��������
	
	tag_TextRaster*	next;
}TextRaster;

typedef struct tag_TextRasterHash
{
	TextRaster*		hash[AGCM_FONT_HASH_SIZE];
}TextRasterHash;

typedef struct tag_FontType
{
	INT32	lID;
	CHAR	szType[AGCM_FONT_NAME_LENGTH];
	CHAR	szFileName[AGCM_FONT_NAME_LENGTH];
	INT32	lSize;
}FontType;

typedef struct tag_AgcdFontClippingBox
{
	FLOAT	x,y,w,h;
}AgcdFontClippingBox;

//��block�� 5000���� ������(6*1000):: ���� cameraz��(index���� Ȱ��) 
#define			AGCM_FONT_ASCII_VERTS1_VERT			4000
#define			AGCM_FONT_ASCII_VERTS1_INDEX		6000
//��block�� 200���� ������(6*500):: worldZ ��
#define			AGCM_FONT_ASCII_VERTS2		3000

// DX�� IM2D ��ü�Ͽ� �׸�(2004.4.14)
class AgcmFont : public AgcModule
{
public:
	AgcmFont();
	~AgcmFont();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnIdle(UINT32 ulClockCount);						

	void	OnCameraStateChange(CAMERASTATECHANGETYPE	ctype);

	// Immediate Mode(2D in World)
	void	DrawTextIMMode2DWorld(float x,float y,float screen_z,float recip_z,char* str,UINT32 fonttype,UINT8 alpha = 255,DWORD color = 0xffffffff, bool bDrawShadow = true,bool bDrawBack = false,DWORD backcolor = 0x7fff0000);
	void	DrawTextIMMode2DWorldScale(float x,float y,float size,float screen_z,float recip_z,char* str,UINT32 fonttype,UINT8 alpha = 255,DWORD color = 0xffffffff, bool bDrawShadow = true);

	// ClippingSet �Լ�(NULL�̸� clipping����)
	void	SetClippingArea(AgcdFontClippingBox*	area);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Immediate Mode
	// Clipping ���� �Լ���
	void	DrawTextIM2D	 (float x,float y,char* str,UINT32 fonttype,UINT8 alpha = 255,DWORD color = 0xffffffff,bool bDrawShadow = true,bool bBold = false,bool bDrawBack = false,DWORD backcolor = 0x7fff0000, INT32 length = -1);
	void	DrawTextIM2DEx	 (float x,float y,char* str, UINT sizeofchar, UINT32 fonttype,UINT8 alpha = 255,DWORD color = 0xffffffff,bool bDrawShadow = true,bool bBold = false,bool bDrawBack = false,DWORD backcolor = 0x7fff0000);
	RwRect	DrawTextRectIM2D (RwRect *pRect  ,char* str,UINT32 fonttype,UINT32 Flag = FF_LEFT,UINT8 alpha = 255,DWORD color = 0xffffffff,bool	bDrawShadow = true, INT32 length = -1);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void	DrawCaret		 (float x,float y,UINT32 fonttype, DWORD color = 0xffffffff, int width = 1);	// white & �ѱ�
	void	DrawCaretE		 (float x,float y,UINT32 fonttype, DWORD color = 0xffffffff);					// white & English
	void	DrawTextIM2DScale(float x,float y,float size,char* str,UINT32 fonttype,UINT8 alpha = 255,DWORD color = 0xffffffff,bool bDrawShadow = true, INT32 length = -1);

	// MapTool�� 
	void	DrawTextMapTool(float x,float y,char* str,UINT32 fonttype,UINT8 alpha = 255,DWORD color = 0xffffffff, bool bDrawShadow = true);
	
	// ImageFont��
	void	DrawTextImgFont  (float x,float y,char* str,UINT32 imgtype,UINT8 Red = 255,UINT8 Green = 255,UINT8 Blue = 255,UINT8 Alpha = 255);

	// string�� pixel���̸� �����Ѵ�
	INT32	GetTextExtent(UINT32 ftype, char* str, INT32 strlen) { return (this->*GetTextExtentPtr)(ftype, str, strlen); }
	int		GetCharacterWidth( UINT32 ftype, char* str ) { return (this->*GetCharacterWidthPtr)(ftype, str); }
	bool	IsLeadByte( unsigned char ch ) { return (this->*IsLeadBytePtr)(ch); }

	void	SetWorldCamera(RwCamera* pCam);

	// Parn���� �߰��Ͻ� Method��
	FontType*			GetSequenceFont(INT32 *plIndex);
	INT32				GetSequenceImageFont(INT32 *plIndex);

	// Block�� ���̴� fonttype ID (�ٸ� fonttype�� �����Ϸ��� endȣ���� �ٸ� type���� �缳��)
	// FontDrawStart�ÿ� clipping������ �����Ҽ� �ֵ��� �Ͽ���. �����ϸ� ������ ������ clipping ó��..
	void				FontDrawStart(int FontType);
	void				FontDrawEnd();
	
private:	
	// ���ο�
	// ������ text�� ������ ��Ȱ�� ������ ���� ^^v
	void	PrepareIMMode2D(float x,float y,char* str,INT32 len,UINT32 fonttype,UINT8 alpha,bool bShadow);
	
	void	RenderAsciiFont(float x,float y,float w,float h,UINT16	ccode,UINT32	fonttype,UINT8	alpha,bool	bShadow);
	void	RenderAsciiFont_CLIP(float x,float y,float w,float h,UINT16	ccode,UINT32	fonttype,UINT8	alpha,bool	bShadow);

	void	RenderFont(float x,float y,float size,UINT16	ccode,UINT32	fonttype,UINT8	alpha,bool	bShadow);
	void	RenderFont_CLIP(float x,float y,float size,UINT16	ccode,UINT32	fonttype,UINT8	alpha,bool	bShadow);

	void	RenderIMMode2DSCALE(float x,float y,char* str,INT32 len,UINT32 fonttype,UINT8 alpha,bool bShadow,float size=1.0f);
	void	RenderIMMode2DWORLD(float x,float y,float screen_z,float recip_z,char* str,INT32 len,UINT32 fonttype,UINT8 alpha,bool bShadow,float size=1.0f);

	void	RenderMapTool(float x,float y,char* str,INT32 len,UINT32 fonttype,UINT8 alpha,bool bShadow);
	
	INT32	GetHashKey(WORD	ccode);

	RwTexture*			CreateShareTexture(int w,int h,int type);			// ��� texture����
	int					PaintAlphabet( DWORD* pBitMapBit, int TexW, int TexH, int ftype,BOOL bMeasureOnly );

	RwRaster*			MakeFontRasterFromBitMapBits( DWORD* pBitMapBit, int TexW, int TexH );

	RwRaster*			MakeFontRasterFromFTBits(FT_GlyphSlot  slot, int width, int height );

	void				CameraStatusChange();								// Camera���� ���� vertexdata �ٽ� set
	RwRaster*			CreateFontRaster(int fonttype,char*	str,int len,FLOAT&	offset);	//	���ڰ� ������� ���� raster�� �����.

	// �� �߰� �Լ�
	BOOL	InitFuncPtr();
	
	INT32	(AgcmFont::*GetTextExtentPtr)(UINT32, char*, INT32);
	INT32	GetTextExtentKr( UINT32 ftype, char* str, INT32 strlen );
	INT32	GetTextExtentEn( UINT32 ftype, char* str, INT32 strlen );
	INT32	GetTextExtentJp( UINT32 ftype, char* str, INT32 strlen );
	INT32	GetTextExtentCn( UINT32 ftype, char* str, INT32 strlen );

	int (AgcmFont::*GetCharacterWidthPtr)(UINT32, char*);
	int GetCharacterWidthKr( UINT32 ftype, char* str );
	int GetCharacterWidthEn( UINT32 ftype, char* str );
	int GetCharacterWidthJp( UINT32 ftype, char* str );
	int GetCharacterWidthCn( UINT32 ftype, char* str );

	bool (AgcmFont::*IsLeadBytePtr)( unsigned char );
	bool IsLeadByteKr( unsigned char ch );
	bool IsLeadByteEn( unsigned char ch );
	bool IsLeadByteJp( unsigned char ch );
	bool IsLeadByteCn( unsigned char ch );

public:
	FontType			m_astFonts[AGCM_FONT_MAX_TYPE_NUM];					// Parn���� �߰��Ͻ� FontType Array
	UINT8				m_iXLH[AGCM_FONT_MAX_TYPE_NUM];						// x width(�ѱ�)
	UINT8				m_iXLE[AGCM_FONT_MAX_TYPE_NUM][256];				// x width(Endglish)
	UINT8				m_iYL[AGCM_FONT_MAX_TYPE_NUM];						// ���� ����
	
	vector<UINT8>		m_widthJpn[AGCM_FONT_MAX_TYPE_NUM];
	
	bool				m_bUpdateThisFrame;									// Tuner���� ����

private:
// data
	RwCamera*			m_pCamera;
	RwReal				m_fScreenZ;											//Im2D�� 
	RwReal				m_fRecipZ;											//Im2D��
	
	INT32				m_iTexSize[AGCM_FONT_MAX_TYPE_NUM];					// Texture Size
	INT32				m_iTexXLE[AGCM_FONT_MAX_TYPE_NUM][256];				// x width(English) - 2�� ���ũ��
	INT32				m_iTexYL[AGCM_FONT_MAX_TYPE_NUM];					// ���� ���� - 2�� ���ũ��

	TextRasterHash		m_listTextRaster;
	//HFONT				m_font[AGCM_FONT_MAX_TYPE_NUM];
	RwTexture*			m_pColonTexture;								// �� 

	RwIm2DVertex		m_vTriFan2World[4];								// UV��ȭ(WORLD)
	My2DVertex			m_vTriFan3[4];									// UV��ȭ
	
	INT32				m_iCameraRasterWidth;
	INT32				m_iCameraRasterHeight;
	
	//��ʿ�
	FLOAT				m_fTexCoords[AGCM_FONT_MAX_TYPE_NUM][95][4];
	bool				m_bUseShare[AGCM_FONT_MAX_TYPE_NUM];			// ��� ��� ���� (256 size�ʰ��� ��� ���x)
	RwTexture*			m_pEnglishTextTexture[AGCM_FONT_MAX_TYPE_NUM];	// ������ ���(32~126���� ��������)
	FLOAT				m_iShareSize[AGCM_FONT_MAX_TYPE_NUM];

	RwRGBA				m_stColor;										// String������ �� set(���� ���Ҹ� ����)
	DWORD				m_iColor2;
		
	//	Ascii�� POOL(FontDrawStart �� FontDrawEnd ���̿� �ִ� �۵��� ��� �׸���.)
	INT16				m_pIndexAscii1[AGCM_FONT_ASCII_VERTS1_INDEX];
	My2DVertex			m_vBufferAscii1[AGCM_FONT_ASCII_VERTS1_VERT];	

	RwIm2DVertex		m_vTriListAscii2[AGCM_FONT_ASCII_VERTS2];		// cameraz ��ȭ(WorldZ)

	INT32				m_iBeforeFilter;

	INT32				m_iAsciiNextIndex1;								// indexbuffer index�� ����
	INT32				m_iAsciiNextBufferIndex1;						// vertexbuffer index�� ����
	INT32				m_iAsciiNextIndex2;								// vert num�� ����
	INT32				m_iCurFontType;
	INT32				m_iFontBlockStatus;

	BOOL				m_bUseClipping;
	AgcdFontClippingBox	m_stClipArea;
	BOOL				m_bClippingRoughTestResult;

	//���� image font��
	FLOAT				m_fImgFontUV[AGCM_IMGFONT_MAX_TYPE_NUM][13][4];
	INT32				m_iImgFontW[AGCM_IMGFONT_MAX_TYPE_NUM][13];				// x width(Endglish)
	INT32				m_iImgFontTexW[AGCM_IMGFONT_MAX_TYPE_NUM];
	INT32				m_iImgFontH[AGCM_IMGFONT_MAX_TYPE_NUM];					// ���� ����
	INT32				m_iImgFontTexH[AGCM_IMGFONT_MAX_TYPE_NUM];
	RwTexture*			m_pImgFontTexture[AGCM_IMGFONT_MAX_TYPE_NUM];

	// FreeType!!
	FT_Library			m_pFTLibrary;
	FT_Face				m_pFTFaces[AGCM_FONT_MAX_TYPE_NUM];

	// ������ �� ���ؼ�, 0x80�� �Ѿ�� ���ڵ��� �ִ�
	int					m_mbcsLetters;
	std::vector<WORD>	m_sjis2uniTable;

private:
	inline int GetJapaneseCharWidth( int fontIndex, char* str );
	
	void SetJapaneseTable();
	int  SJIS_TO_UNICODE( int charIndex );
	INT32 GetMaxTexSize( int max );

public :
	AgcdFontClippingBox		GetCurrentClippingArea( void )		{ return m_stClipArea; }
};

// inline
int AgcmFont::GetJapaneseCharWidth( int fontIndex, char* str )
{
	int  width;
	WORD dc = MAKEWORD( str[1], str[0] );

	if ( dc >= 0x8140 && dc <= 0x8396 && str[1] != 0 )
		width = m_widthJpn[fontIndex][dc - 0x8140];
	else
		width = m_iXLH[fontIndex];

	return width;
}


#endif