#pragma once

#include <QWidget>
#include "H_Importer.h"

class HapticPlotData;
class WaveToHaptic;

namespace HT
{
	class WAVE_Importer : public H_Importer
	{
		Q_OBJECT
	public:
		WAVE_Importer();
		~WAVE_Importer();

	private:
		virtual bool LoadFile(QString File, HapticPlotData* ImportData);

		WaveToHaptic* mWaveToHaptic;
	};
};

