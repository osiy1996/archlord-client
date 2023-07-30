// -----------------------------------------------------------------------------
//              _____                   _    _____                                               _     
//             |_   _|                 | |  / ____|                                             | |    
//   __ _ _   _  | |  _ __  _ __  _   _| |_| |      ___  _ __ ___  _ __   ___  ___  ___ _ __    | |__  
//  / _` | | | | | | | '_ \| '_ \| | | | __| |     / _ \| '_ ` _ \| '_ \ / _ \/ __|/ _ \ '__|   | '_ \ 
// | (_| | |_| |_| |_| | | | |_) | |_| | |_| |____| (_) | | | | | | |_) | (_) \__ \  __/ |    _ | | | |
//  \__,_|\__,_|_____|_| |_| .__/ \__,_|\__|\_____|\___/|_| |_| |_| .__/ \___/|___/\___|_|   (_)|_| |_|
//                         | |                                    | |                                  
//                         |_|                                    |_|                                  
//
// 
//
// -----------------------------------------------------------------------------
// Originally created on 07/27/2005 by daeyoung Cha
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_AUINPUTCOMPOSER
#define _H_AUINPUTCOMPOSER

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>

// Local Header Files
#include <string>
#include <vector>


// -----------------------------------------------------------------------------

class	AuInputComposer
{
public:
	// Destruction
	virtual		~AuInputComposer();

	// Implementation
	static			AuInputComposer& GetInstance();		// inline�Լ��� �ۼ��� �ΰ��� �ν��Ͻ��� ����� �����Ƿ�..
	void			InitInputComposer();				// �ʱ�ȭ

	// Window IME Message Function implementation
	void			OnInputLanguageChange(HWND hWnd, WPARAM wParam, LPARAM lParam);		// WM_INPUTLANGCHANGE
	bool			OnComposition(HWND hWnd, WPARAM wParam, LPARAM lParam);				// WM_IME_COMPOSITION
	bool			OnEndComposition(HWND hWnd, WPARAM wParam, LPARAM lParam);			// WM_IME_ENDCOMPOSITION
	bool			OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);					// WM_IME_NOTIFY
	bool			OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam);					// WM_CHAR
	void			AdjustCompositionWindowPos(HWND hWnd);
	
	// Accessors
	int				GetInput(char* text, int len);
	int				GetComp(char* text, int len);
	int				GetReading(char* text, int len);					// Reading window�� �� ���ں�ȯ & ��ȯ
	int				GetCandidate(DWORD index, char* text, int len);		// candidate window�� ���ڿ��� ���� ��ȯ
	int				GetCandidateCount();								// candidate window�� ���� ��ȯ
	int				GetCandidateSelection();							// ������ candidate window�� ���� ��ȯ
	int				GetCandidatePageSize();								// candidate window�� page size ��ȯ							
	int				GetCandidatePageStart();							// candidate window�� ����page�� page��ȣ ��ȯ
	char*			GetNextChar(const char* pTemp);						// ���� ���ڿ� ���
	WORD			GetCodePage()			{ return m_codePage; }		// LangID�� ���� codepage
	LANGID			GetLangId()				{ return m_langId; }		// ���� ������ Language ID
	int				GetCharSet()			{ return GetCharsetFromLang(m_langId); }
	int				GetImeState()			{ return m_ImeState; }
	char*			GetIndicator()			{ return m_wszCurrIndicator; }			// ���� ������ Indicator(char) ��ȯ
	WORD			GetPrimaryLangID()		{ return PRIMARYLANGID(m_langId); }		// ���� ������ LangID�� PRIMARYLANGID ��ȯ
	WORD			GetSubLangID()			{ return SUBLANGID(m_langId); }

	// Property
	bool			IsVerticalReading()		{ return m_bVerticalReading; }	//reading window�� vertical���� �ƴ���
	bool			IsVerticalCandidate()	{ return m_bVerticalCandidate; }//candidate window�� vertical���� �ƴ���

	void			SetEditPosition( int nX , int nY )
	{
		m_ptEditPosition.x	= nX;
		m_ptEditPosition.y	= nY;
	}

	void			SetCandidatePos( int x, int y )
	{
		candidatePos_.x = x;
		candidatePos_.y = y;
	}

	POINT &			GetCandidatePos() { return candidatePos_; }


	void			SetRenderedIMEWindow( BOOL bUse = TRUE )
	{
		m_bUseRenderedIMEWindow = bUse;
	}

	void ToAlphaNumeric( HWND hWnd );
	void ToNative( HWND hWnd );
private:

	// Construction
	AuInputComposer();

	// Data members
	// ����� ������ �ʿ䰡 ����
	std::wstring		m_input;		// ���տϷ�� ���ڿ�
	std::wstring		m_comp;			// �������� ���ڿ�
	std::wstring		m_reading;		// reading window

	std::vector<BYTE>	m_candidate;

	int					m_ulStart;
	int					m_ulEnd;

	// Language identifier.
	// typedef	WORD LANGID
	// +-------------------------+-------------------------+
	// |  SubLanguage ID  |   Primary Language ID   |
	// +-------------------------+-------------------------+
	// 15                    10  9                         0   bit
	// LANG_SYSTEM_DEFAULT, which identifies the system default language. 
	// LANG_USER_DEFAULT, which identifies the language of the current user. 
	LANGID				m_langId;
	WORD				m_codePage;

	bool				m_bUnicodeIME;
	bool				m_bVerticalReading;
	bool				m_bVerticalCandidate;
	int					m_ImeState;

	char*				m_wszCurrIndicator;

	DWORD				m_dwId[2];

	//Pimpl idiom, only use private data..
	class WidgetImpl*	_pImpl;

	POINT				m_ptEditPosition;
	POINT				candidatePos_;

	BOOL				m_bUseRenderedIMEWindow;

	DWORD				m_sentence;
	DWORD				m_conversion;
private:

	//private implementation
	void			GetImeId();
	bool			GetReadingWindowOrientation();
	void			GetPrivateReadingString(HWND hWnd);
	void			CheckToggleState(HWND hWnd);

	int				GetCharsetFromLang(LANGID);
	int				GetCodePageFromCharset( int charset );
	int				GetCodePageFromLang( LANGID langid );

};

#endif // _H_AUINPUTCOMPOSER
// -----------------------------------------------------------------------------
// auInputComposer.h - End of file
// -----------------------------------------------------------------------------

