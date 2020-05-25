#include "TableManager.h"
#include "qdebug.h"

using namespace HT;
TableManager::TableManager()
{

}

TableManager::~TableManager()
{
	mSampleParserPtr = NULL;
}

TableManager* TableManager::createInstance()
{
	return new TableManager();
}

bool TableManager::LoadTable()
{
	QDir dir(".");	
	QString XmlPath = QString("%1/%2/%3").arg(dir.absolutePath(), "data", "Samples.xml");

	mSampleParserPtr.reset(new SampleTableParser());

	if (!mSampleParserPtr->ParseXml(XmlPath))
	{
		return false;
	}

	return true;
}


//
//
//QDir dir(".");
//
//SchemaPath = QString("%1/%2/%3").arg(dir.absolutePath(), "data", SchmeaName);// "ServerConfig.xsd");
//XmlPath = QString("%1/%2/%3").arg(dir.absolutePath(), "data", xmlName);// "DefaultConfig.xml");
//
//
//
//
//ParserPtr->SetSchema(SchemaPath);

//
//
