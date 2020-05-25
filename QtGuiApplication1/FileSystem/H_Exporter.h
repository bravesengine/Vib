#pragma once

#include <QWidget>
#include "singleton.h"

class HapticPlotData;

namespace HT
{


	class H_Exporter : public QObject
	{
		Q_OBJECT
	public:
		H_Exporter() {}
		~H_Exporter() {}

		virtual bool SaveFile(QString File, HapticPlotData* PlotData) { return false; }
	};
};

//QDataStream &operator << (QDataStream& in, HapticPlotData* data);


