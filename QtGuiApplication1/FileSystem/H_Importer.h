#pragma once

#include <QWidget>
#include "singleton.h"

class HapticPlotData;
namespace HT
{
	class H_Importer : public QObject
	{
		Q_OBJECT
	public:
		H_Importer() {}
		~H_Importer() {}
		virtual bool LoadFile(QString File, HapticPlotData* ImportData)
		{
			return false;
		}
	private:		
	};
};


