#include "WAVE_Importer.h"
#include "WaveToHaptic.h"

#include "qdebug.h"
#include "HapticPlotData.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include <QSound>
#include <QAudioBuffer>
#include <QMediaPlayer>

#include <QFile>
using namespace HT;

WAVE_Importer::WAVE_Importer() : H_Importer()
{
	mWaveToHaptic = new WaveToHaptic();
}

WAVE_Importer::~WAVE_Importer()
{

}

bool WAVE_Importer::LoadFile(QString File, HapticPlotData* ImportData)
{
	QFile file(File);

	if (!file.exists())
		return false;

	if (!file.open(QIODevice::ReadOnly))
		return false;


	bool bRet = mWaveToHaptic->LoadWaveFile(File);
	if (bRet == false)
		return false;


	QList<quint8> RawData = mWaveToHaptic->BufferToHaptic();
	if (RawData.size() == 0)
		return false;
	
	ImportData->DataToReMakeTime(ImportData->MakePlotData(RawData));// , false));

	return bRet;
}
