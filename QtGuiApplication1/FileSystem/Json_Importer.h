#pragma once

#include <QWidget>
#include "H_Importer.h"

class HapticPlotData;
class WaveToHaptic;

namespace HT
{
	class Json_Importer : public H_Importer
	{
		Q_OBJECT
	public:
		Json_Importer();
		~Json_Importer();

	private:
		virtual bool LoadFile(QString File, HapticPlotData* ImportData);

	};
};

