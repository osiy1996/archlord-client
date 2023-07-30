
#include <AgpmPathManager/AgpmPathManager.h>
#include <AuXmlParser/AuXmlParser.h>

AgpmPathManager::AgpmPathManager()
{
}

AgpmPathManager::~AgpmPathManager()
{
}

BOOL AgpmPathManager::Load()
{
	AuXmlDocument m_XmlData;
	m_XmlData.LoadFile( "ini\\PathData.xml" );

	return TRUE;
}
