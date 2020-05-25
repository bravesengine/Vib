#include "Json_Importer.h"
#include "WaveToHaptic.h"

#include "qdebug.h"
#include "HapticPlotData.h"
#include "qfiledialog.h"
#include "qmessagebox.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QByteArray>
#include "HFS_Manager.h"


#include <QFile>
using namespace HT;


QByteArray JsonValToByte(const QJsonValue &val) {
	auto const encoded = val.toString().toLatin1();

	return QByteArray::fromBase64(encoded);
}

Json_Importer::Json_Importer() : H_Importer()
{
}


Json_Importer::~Json_Importer()
{

}

bool Json_Importer::LoadFile(QString File, HapticPlotData* ImportData)
{
	QFile file(File);

	if (!file.exists())
		return false;

	if (!file.open(QIODevice::ReadOnly))
		return false;

	QByteArray loadData = file.readAll();
	
	QJsonDocument loadDoc(QJsonDocument::fromJson(loadData));

	QJsonObject jsonObj = loadDoc.object();
	if (!jsonObj.contains("HeaderDesc") || !jsonObj.contains("Version") || !jsonObj.contains("Size"))
		return false;

	QString HeaderDesc  = jsonObj["HeaderDesc"].toString();
	double Version = jsonObj["Version"].toDouble();
	int Size = jsonObj["Size"].toInt();

	QByteArray CompressRawBase64 = JsonValToByte(jsonObj["RawBase64"]);

	QByteArray RawBase64 =  HFS_Manager::UnCompress(CompressRawBase64);

	QList<HzPlotData> hzData;
	bool bRet = HapticPlotData::ReadHzPlotData(RawBase64, Size, hzData);
	return ImportData->DataToReMakeTime(hzData);
}
