#ifndef __TRANSLATE_REGION_NAME_H_
#define __TRANSLATE_REGION_NAME_H_

#include <map>
#include <string>

typedef std::map<std::string, std::string> StrMap;

class CTranslate
{
public:
	CTranslate();
	CTranslate(int i);	// ���� �ٸ� �ΰ��� �ν��Ͻ��� ������ �ؼ� ������
	~CTranslate();

	const char* GetName(char* key);

private:
	StrMap m_strMap;
};

const char* GetKorName(char* engName);
const char* GetEngName(char* korName);


#endif __TRANSLATE_REGION_NAME_H_