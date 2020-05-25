#include "BVF_Exporter.h"
#include "qdebug.h"
#include "HapticPlotData.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include "HFS_Manager.h"
#include <QFile>
using namespace HT;


//-- Save.
QDataStream &operator << (QDataStream& out, HapticPlotData* data)
{
	QList<HzPlotData> hzData = data->GetHzPlotData();

	out << HFS_Manager::Header_Desc;
	out << HFS_Manager::Header_Version;

	out << hzData.size();

	QByteArray unzipData = data->Serialize();
	QByteArray  compressedData = HFS_Manager::Compress(unzipData);
	out << compressedData;

	return out;
}

BVF_Exporter::BVF_Exporter()
{

}

BVF_Exporter::~BVF_Exporter()
{

}

bool BVF_Exporter::SaveFile(QString File, HapticPlotData* PlotData)
{
	QFile file(File);

	if (!file.open(QIODevice::WriteOnly))
		return false;

	QDataStream out(&file);
	out << PlotData;
	file.close();
	return true;
}
