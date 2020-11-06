#pragma once
#include "H_Importer.h"

class HapticPlotData;
class WaveToHaptic;

namespace HT
{
	class MP3_Importer : public H_Importer
	{
		Q_OBJECT
	public:
		MP3_Importer();
		~MP3_Importer();

	private:
		virtual bool LoadFile(QString File, HapticPlotData* ImportData);

		WaveToHaptic* mWaveToHaptic;
	};
};