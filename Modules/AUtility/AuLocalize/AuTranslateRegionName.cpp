#include "AuTranslateRegionName.h"

using std::string;

// korean - English
CTranslate::CTranslate()
{
	m_strMap["Anchorville"]	= "��Ŀ��";
	m_strMap["Delfaras"]	= "���Ķ�";
	m_strMap["Tullan"]		= "����";
	m_strMap["Ellore"]		= "���θ�";

	m_strMap["Golundo"]		= "��鵵";
	m_strMap["Kuhn"]		= "��";
	m_strMap["Trilgard"]	= "Ʈ������";
	m_strMap["Zylok"]		= "���Ϸ�";

	m_strMap["Norine"]		= "�븰";
	m_strMap["Laflesia"]	= "���÷��þ�";
	m_strMap["Deribelle"]	= "������";
	m_strMap["Tor Fortress"]= "�丣 ���";

	m_strMap["Sienne"]		= "�ÿ�";
	m_strMap["Sariend"]		= "�縮����";
	m_strMap["Stull"]		= "����";
	m_strMap["Halien"]		= "�Ҹ���";

	m_strMap["Hideback"]	= "���̵�� ����";
	m_strMap["Terranoa"]	= "�׶���";
}

// English - Korean
CTranslate::CTranslate(int i)
{
	m_strMap["��Ŀ��"]		= "Anchorville";
	m_strMap["���Ķ�"]	= "Delfaras";
	m_strMap["����"]		= "Tullan";
	m_strMap["���θ�"]		= "Ellore";

	m_strMap["��鵵"]		= "Golundo";
	m_strMap["��"]			= "Kuhn";
	m_strMap["Ʈ������"]	= "Trilgard";
	m_strMap["���Ϸ�"]		= "Zylok";

	m_strMap["�븰"]		= "Norine";
	m_strMap["���÷��þ�"]	= "Laflesia";
	m_strMap["������"]		= "Deribelle";
	m_strMap["�丣 ���"]	= "Tor Fortress";

	m_strMap["�ÿ�"]		= "Sienne";
	m_strMap["�縮����"]	= "Sariend";
	m_strMap["����"]		= "Stull";
	m_strMap["�Ҹ���"]		= "Halien";

	m_strMap["���̵�� ����"]	= "Hideback";
	m_strMap["�׶���"]	= "Terranoa";


}

CTranslate::~CTranslate()
{
}

const char* CTranslate::GetName(char* key)
{
	StrMap::iterator iter = m_strMap.find(key);
	return (iter != m_strMap.end()) ? iter->second.c_str() : "";
}

const char* GetKorName(char* engName)
{
	static CTranslate ek;
	return ek.GetName(engName);
}

const char* GetEngName(char* korName)
{
	static CTranslate ke(1);
	return ke.GetName(korName);
}
