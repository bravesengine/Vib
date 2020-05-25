#include "BVF_Importer.h"
#include "qdebug.h"
#include "HapticPlotData.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include "HFS_Manager.h"
#include <QFile>
using namespace HT;




//-- Load
QDataStream &operator >> (QDataStream& in, HapticPlotData* data)
{
	int count = 0;

	//out <<;
	//out << HFS_Manager::Header_Version;

	QString HeaderString;
	in >> HeaderString;
	if (HeaderString != HFS_Manager::Header_Desc)
		return in;

	double Version =0;
	in >> Version;
	in >> count;

	//-- ¸¶Áö¸· 
	QByteArray compressedData;
	in >> compressedData;
	QByteArray uncompressData = HFS_Manager::UnCompress(compressedData);

	QList<HzPlotData> hzData;
	bool bRet = HapticPlotData::ReadHzPlotData(uncompressData, count, hzData);
	data->DataToReMakeTime(hzData);

	return in;
}

BVF_Importer::BVF_Importer() : H_Importer()
{

}

BVF_Importer::~BVF_Importer()
{

}

bool BVF_Importer::LoadFile(QString File, HapticPlotData* ImportData)
{
	QFile file(File);

	if (!file.exists())
		return false;

	if (!file.open(QIODevice::ReadOnly))
		return false;

	QDataStream in(&file);
	in >> ImportData;
	file.close();

	if (ImportData->GetHzPlotData().size() == 0)
		return false;

	return true;
}
