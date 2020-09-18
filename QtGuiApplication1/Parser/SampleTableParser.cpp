#include "SampleTableParser.h"
#include <QtXml/qdom.h>
#include <QtXml/qxml.h>
#include <QtCore/QtCore>
#include <QtXmlPatterns/QXmlSchema>
#include <QtXmlPatterns/QXmlSchemaValidator> 

#include "Samples.h"
#include "Parser.h"
#include "handler.h"

#include <qdatetime.h>
#include <cstdlib>


using namespace HT;
using namespace qXsd2Code;

SampleTableParser::SampleTableParser()
{



}

SampleTableParser::~SampleTableParser()
{
	m_SamplesDataPtr = NULL;

	m_XmlReaderPtr = NULL;
	m_XmlHandlerPtr = NULL;
}

bool SampleTableParser::ParseXml(QString xmlPath)
{
	HT::Parser Parser;
	//-- 어잉?? Why Connet Comnnect??
	connect(&Parser, SIGNAL(signalSamples(const HT::Samples&)), this, SLOT(slotSampleTable(const HT::Samples&)));
	connect(&Parser, SIGNAL(signalError(QString)), this, SLOT(slotPrintError(QString)));
	connect(&Parser, SIGNAL(signalWarning(QString)), this, SLOT(slotPrintError(QString)));
	connect(&Parser, SIGNAL(signalValidationError(QString)), this, SLOT(slotPrintError(QString)));

	QFile file(xmlPath);
	if (!file.open(QIODevice::ReadOnly))
		return false;

	bool bRet = false;
	try
	{
		while (!file.atEnd())
		{
			QString line = file.readLine();
			Parser.parseXMLString(line, true);
		}
		bRet = true;
	}
	catch(...)
	{

	}
	return bRet;
}



bool SampleTableParser::ParseXmlFromData(QString xmlData)
{
	HT::Parser Parser;
	connect(&Parser, SIGNAL(signalSamples(const HT::Samples&)), this, SLOT(slotSampleTable(const HT::Samples&)));
	connect(&Parser, SIGNAL(signalError(QString)), this, SLOT(slotPrintError(QString)));
	connect(&Parser, SIGNAL(signalWarning(QString)), this, SLOT(slotPrintError(QString)));
	connect(&Parser, SIGNAL(signalValidationError(QString)), this, SLOT(slotPrintError(QString)));

	bool bRet = false;
	try
	{
		bRet = Parser.parseXMLString(xmlData, true);
	}
	catch (...)
	{
		return false;
	}

	return true;
}


void SampleTableParser::slotPrintError(const QString &errorStr)
{
	//-- blabla..
}


void SampleTableParser::slotSampleTable(const HT::Samples &obj)
{
	m_SamplesDataPtr = QSharedPointer < Samples > (new Samples(obj));
	//QString xml = m_ServerConfigPtr->toXML(true);
	//Write("D:\\test.xml", xml);
}

bool SampleTableParser::LoadXsdObject(QString xsdFile)
{
	QFile *file = new QFile(xsdFile);
	if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}

	m_XmlHandlerPtr = QSharedPointer<qXsd2Code::Handler>(new qXsd2Code::Handler());
	m_XmlReaderPtr = QSharedPointer<QXmlSimpleReader>(new QXmlSimpleReader());

	//?? 이렇게 쓰면 돼는건가?
	qXsd2Code::Handler* pHandler =	m_XmlHandlerPtr.data();
	//// setup the parser
	m_XmlReaderPtr->setContentHandler(pHandler);
	m_XmlReaderPtr->setErrorHandler(pHandler);

	QXmlInputSource inputXML(file);	
	bool bRet = m_XmlReaderPtr->parse(inputXML);



	/*QVector<XSDObject*> Objects = m_XmlHandlerPtr->objects();	
	for (int i = 0; i < Objects.size(); i++)
	{
		XSDObject *obj = Objects.at(i);
	}*/

	return bRet;
}

bool SampleTableParser::InvalidCheck(QString SchemaPath, QString xmlPath)
{
	QFile *in = new QFile(SchemaPath);
	in->open(QIODevice::ReadOnly);

	QXmlSchema schema;
	schema.load(in, QUrl::fromLocalFile(in->fileName()));

	if (schema.isValid()) {
		QFile file(xmlPath);
		file.open(QIODevice::ReadOnly);

		QXmlSchemaValidator validator(schema);
		
		if (!validator.validate(&file, QUrl::fromLocalFile(file.fileName())))
			return false;
		//	qDebug() << "instance document is valid";
		//else
		//	qDebug() << "instance document is invalid";
	}
	return true;
}


//QString ServerConfigParser::GetGameID()
//{
//	if (m_ServerConfigPtr.isNull())
//		return "";
//
//	//QSharedPointer<Braves::Servers> Config = GetParserData();
//	if (m_ServerConfigPtr->countOfGameClusters() != 1)
//		return "";
//
//	return m_ServerConfigPtr->getGameClusterAt(0).getGameID();
//}

//const DBInstance* ServerConfigParser::getDbInstance(QString instanceName)
//{
//	if(m_ServerConfigPtr.isNull())
//		return NULL;
//
//	for (int i = 0; i < m_ServerConfigPtr->countOfDBInstancess(); i++)
//	{
//		if (m_ServerConfigPtr->getDBInstancesAt(i).getInstanceName() == instanceName)
//		{
//			return &m_ServerConfigPtr->getDBInstancesAt(i);
//		}
//	}
//
//	return NULL;
//}
