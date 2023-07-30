#include "AuStringConv.h"
#include <ApBase/ApDefine.h>

#include <vector>

// ������ ���� ��� �ڵ带 �����´�.
unsigned GetLangID( void )
{
	UINT code = 949;

	switch( g_eServiceArea )
	{
	case AP_SERVICE_AREA_CHINA:
		code = 936;
		break;
	case AP_SERVICE_AREA_TAIWAN:
		code = 950;
		break;
	case AP_SERVICE_AREA_GLOBAL:
		code = 1252;
		break;
	case AP_SERVICE_AREA_JAPAN:
		code = 932;
		break;
	default:
		code = 949;	// �⺻�� �ѱ�
	}

	return code;
}

// src�� �����ڰ� �ִٸ� �ҹ��ڷ� ��ȯ�Ѵ�.
// �Ϻ���� _mbclwr�� �̻��ϰ� �۵��ؼ� ���ڿ��� �����ڵ�� ��ȯ����
// ���� �빮�ڸ� �ҹ��ڷ� ����� �ٽ� mbcs�� ��ȯ�Ѵ�.
void ConvertToLower( char* src, char* lower, int lowerSize )
{
	if ( src != NULL && strlen( src ) != 0 )
	{
		const int wideSize = 64;

		// to Unicode
		wchar_t wide[wideSize] = {0, };
		ToWide( src, wide, wideSize );

		// lower characters
		_wcslwr_s( wide, wideSize );

		// to mbcs
		ToMBCS( wide, lower, lowerSize );
	}
}

int ToWide( char* mbcs, wchar_t* wide, int wideCharCount )
{
	return MultiByteToWideChar( GetLangID(), 0, mbcs, -1, wide, wideCharCount );
}

int ToMBCS( wchar_t* wide, char* mbcs, int mbcsCharCount )
{
	int needBufferSize = WideCharToMultiByte( GetLangID(), 0, wide, -1, 0, 0, 0, 0 );
	
	if ( needBufferSize <= mbcsCharCount )
	{
		WideCharToMultiByte( GetLangID(), 0, wide, -1, mbcs, mbcsCharCount, 0, 0 );
		mbcs[needBufferSize] = 0;
		return true;
	}

	return false;
}

int GetMbcsLength( char* mbcs )
{
	return MultiByteToWideChar( GetLangID(), 0, mbcs, -1, 0, 0 );
}

void ToLowerExceptFirst( char* str, int const length )
{
	if ( 0 != length )
	{
		// Unicode�� �ٲ���
		std::vector<wchar_t> wide( length );
		ToWide( str, &wide[0], length );

		// ù���ڰ� �����ڶ�� �ҹ��ڷ�
		if ( wide[0] >= L'a' && wide[0] <= L'z' )
			wide[0] = L'A' + wide[0] - L'a';

		// ������ �����ڸ� �ҹ��ڷ� �ٲ���
		unsigned strSize = static_cast<unsigned>( wcslen( &wide[0] ) );

		for ( unsigned i = 1; i < strSize; ++i )
		{
			if ( wide[i] >= L'A' && wide[i] <= L'Z' )
				wide[i] = L'a' + wide[i] - 'A';
		}

		// �ٽ� MBCS��
		ToMBCS( &wide[0], str, length );
	}
}

// UIMessage�� ������ �ٸ� ���ڸ� �ڸ�(,)�� ����ǥ(")�� �ٲ���.
// �Ϻ��� MBCS�� ��쿡 trail byte�� ,�� ���� �� �ִ�.
void ChangeCommaAndQutationMark( char* src, char comma, char qutation )
{
	std::vector<wchar_t> wideString(2048);
	const int charCount = ToWide( src, &wideString[0], 2048 );

	for ( int i = 0; i < charCount; ++i )
	{
		if ( wideString[i] == comma )
			wideString[i] = L',';

		if ( wideString[i] == qutation )
			wideString[i] = L'"';
	}

	// AGCMUIMANAGER2_MAX_UI_MESSAGE_DATA�� ���ؼ� AgcmUIManager2Stream.h��
	// include�ϱ⿡�� ������ �־�, 384�� �ϵ��ڵ��Ѵ�.
	ToMBCS( &wideString[0], src, /*AGCMUIMANAGER2_MAX_UI_MESSAGE_DATA*/384 );
}
