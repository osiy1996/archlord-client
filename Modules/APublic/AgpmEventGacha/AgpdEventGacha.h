#pragma once

#include <AgpmCharacter/AgpdCharacter.h>
#include <map>

using namespace std;

struct	AgpdGachaDropTable
{
    struct    DropInfo
    {
        INT32    nItemTID;        // �׺� ������

        INT32    nItemNeeded;    // ������ ����. . 0�̸� üũ ����

        INT32    nCost;    // �ʿ��� �� �׼�
        INT32	nNeedCharisma; // �ʿ� ī������ ����Ʈ - arycoat

        //FLOAT    fRank1;	// ������ �̴ϱ� ������ �ʿ䰡 ����
        FLOAT    fRank2;
        FLOAT    fRank3;
        FLOAT    fRank4;    // ��ũ�� �� ���� ,
							// �ε带 �Ҷ� �̸� ����Ͽ� Ȯ������ ������.

		DropInfo(): nItemTID( 0 ) , nItemNeeded( 0 ) , nCost( 0 ), nNeedCharisma(0),
			fRank2( 0.0f ) ,fRank3( 0.0f ) ,fRank4( 0.0f ) {}
    };

    map<INT32,DropInfo>    mapGacha;

    BOOL    StreamRead( const char * pFilename , BOOL bDecryption );
};

struct AgpdGachaItemTable
{
	typedef map< INT32 , vector< INT32 > > Table; // Rank , TID

	// Level , Item vector
    map< INT32 , Table >    mapItemTable;
    // ������ ������ ����.

    bool ValidationCheck(); // ������ Ȯ��... ����ִ� �����밡 ������ Ȯ��.
};

struct AgpdGachaType
{
	INT32		nID;
	std::string	strName;
	std::string	strComment;
	bool		bRaceCheck;
	bool        bClassCheck;
	bool        bLevelCheck;    // ������ ��� ���� üũ
	INT32       nLevelLimit;    // ��í �÷��� ���� üũ 
								// �� �������� ���ų� Ŭ��� ��� ������.
	AgpdGachaDropTable	stDropTable;	// ���� ����Ÿ ����..

	map< AuRace , AgpdGachaItemTable	>	mapGachaTable	;
	AgpdGachaItemTable	*	GetGachItemTable( AgpdCharacter * pcsCharacter	);
	AgpdGachaItemTable	*	GetGachItemTable( AuRace stRace	);

	AgpdGachaType() : nID( -1 ) , bRaceCheck( false ), bClassCheck( false ),
		bLevelCheck( false ) , nLevelLimit( 0 ) {}
};