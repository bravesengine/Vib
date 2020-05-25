#pragma once

#include <QWidget>
#include "singleton.h"

class HapticPlotData;

namespace HT
{
	class HFS_Manager : public QObject
	{
		Q_OBJECT
	public:
		HFS_Manager();
		~HFS_Manager();

		static const double Header_Version;		
		static const QString Header_Desc;

		static QByteArray Compress(QByteArray &uncompressed_data);
		static QByteArray UnCompress(QByteArray &compressed_data);

		static HFS_Manager* instance()
		{
			return Singleton<HFS_Manager>::instance(HFS_Manager::createInstance);
		}

		bool LoadDialog(HapticPlotData* ImportData, QString &outPath);
		bool SaveDialog(HapticPlotData* ExportData, QString &outPath);


	private:
		static HFS_Manager* createInstance();
	};

	//inline HFS_Manager::Header_Version = 1.0;
	//inline HFS_Manager::Header_Desc = "Braves Vibration File";
};


