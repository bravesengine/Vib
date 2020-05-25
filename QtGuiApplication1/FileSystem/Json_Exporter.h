#pragma once

#include <QWidget>
#include "H_Exporter.h"

class HapticPlotData;

namespace HT
{
	class Json_Exporter : public H_Exporter
	{
		Q_OBJECT
	public:
		Json_Exporter();
		~Json_Exporter();

		virtual bool SaveFile(QString File, HapticPlotData* PlotData);

	};
};




