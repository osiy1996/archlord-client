#if !defined _AGPMTRANSFORM_H_
#define _AGPMTRANSFORM_H_

#include "ApBase.h"
#include "ApModule.h"

#include <map>
using namespace std;

//�������� �ۼ��� ���� ����
//������ �巡��ÿ¸� ��������� �η�η� ����Ҽ��ֵ���... ����...

//enum AuRaceType
//{
//	AURACE_TYPE_NONE		= 0,
//	AURACE_TYPE_HUMAN,			//�޸� 
//	AURACE_TYPE_ORC,			//��ũ
//	AURACE_TYPE_MOONELF,		//������
//	AURACE_TYPE_DRAGONSCION,	//�巡��ÿ�
//	AURACE_TYPE_MAX
//};

//enum AuCharClassType
//{
//	AUCHARCLASS_TYPE_NONE		= 0,
//	AUCHARCLASS_TYPE_KNIGHT,
//	AUCHARCLASS_TYPE_RANGER,
//	AUCHARCLASS_TYPE_SCION,
//	AUCHARCLASS_TYPE_MAGE,
//	AUCHARCLASS_TYPE_MAX
//};

struct TransformInfo
{
	TransformInfo( INT32 nKnight, INT32 nRanger, INT32 nMage ) : m_nKnight(nKnight), m_nRanger(nRanger), m_nMage(nMage)	{	}

	INT32	m_nKnight;
	INT32	m_nRanger;
	INT32	m_nMage;
};
typedef map< INT32, TransformInfo* >	TransformInfoMap;
typedef TransformInfoMap::iterator		TransformInfoMapItr;

class AgpmTransform : public ApModule
{
public:
	AgpmTransform();
	~AgpmTransform();

	BOOL	Initialize( char* szCharacter, char* szItem, BOOL bDecryption );
	void	Release();

	INT32	GetCharacterID( INT32 nID, AuCharClassType eClassType );
	INT32	GetItemID( INT32 nID, AuCharClassType eClassType );

private:
	TransformInfoMap	m_mapCharacterInfo;
	TransformInfoMap	m_mapItemInfo;
};

#endif