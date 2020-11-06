#include "MP3_Importer.h"
#include "WaveToHaptic.h"

#include "qdebug.h"
#include "HapticPlotData.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include <QSound>
#include <QAudioBuffer>
#include <QMediaPlayer>
#include <lame.h>
#include <stdio.h>

#include <QFile>

using namespace HT;


MP3_Importer::MP3_Importer() : H_Importer()
{
}


MP3_Importer::~MP3_Importer()
{

}

bool MP3_Importer::LoadFile(QString File, HapticPlotData* ImportData)
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
