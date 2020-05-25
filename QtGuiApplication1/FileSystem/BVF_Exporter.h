#pragma once

#include <QWidget>
#include "H_Exporter.h"

class HapticPlotData;

namespace HT
{
	class BVF_Exporter : public H_Exporter
	{
		Q_OBJECT
	public:
		BVF_Exporter();
		~BVF_Exporter();

		virtual bool SaveFile(QString File, HapticPlotData* PlotData);

	};
};

//QDataStream &operator << (QDataStream& in, HapticPlotData* data);


