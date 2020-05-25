#pragma once

#include <QtCore>
#include <QObject>
#include <qstring.h>
#include <qfile.h>

#include <QSharedPointer>

class QXmlSimpleReader;

namespace qXsd2Code
{
	class Handler;
	class XSDObject;
}

namespace HT
{
	class Samples;

	class SampleTableParser : public QObject
	{
		Q_OBJECT;

	public:
		SampleTableParser();
		~SampleTableParser();

		void  SetSchema(QString schemaPath)
		{
			m_scheamPath = schemaPath;
		}

		bool ParseXml(QString xmlPath);
		bool ParseXmlFromData(QString xmlData);

	public:

		bool InvalidCheck(QString SchemaPath, QString xmlPath);

		QSharedPointer<qXsd2Code::Handler> getHandler() {
			return m_XmlHandlerPtr;
		}
		
		QSharedPointer<Samples> getTable() { return m_SamplesDataPtr; }	
	
	private:
		QString m_scheamPath;
		QSharedPointer<HT::Samples> m_SamplesDataPtr;

	private slots:

		void slotSampleTable(const HT::Samples &obj);
		void slotPrintError(const QString &errorStr);
	private:

		QSharedPointer<QXmlSimpleReader> m_XmlReaderPtr;
		QSharedPointer<qXsd2Code::Handler> m_XmlHandlerPtr;
		bool LoadXsdObject(QString xsdFile);
	};
};
