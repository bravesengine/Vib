#pragma once

#include <QWidget>
#include "singleton.h"

#include "SampleTableParser.h"

namespace HT
{
	class TableManager : public QObject
	{
		Q_OBJECT
	public:
		TableManager();
		~TableManager();

		//--use sample
		static TableManager* instance()
		{
			return Singleton<TableManager>::instance(TableManager::createInstance);
		}

		bool LoadTable();


		QSharedPointer<Samples> GetSampleTable()
		{
			return mSampleParserPtr->getTable();
		}

	private:
		static TableManager* createInstance();


		QSharedPointer<SampleTableParser> mSampleParserPtr;

	};
};

