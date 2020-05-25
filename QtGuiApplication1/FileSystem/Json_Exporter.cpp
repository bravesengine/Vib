#include "Json_Exporter.h"
#include "qdebug.h"
#include "HapticPlotData.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include <QFile>
#include "HFS_Manager.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QByteArray>

#include <qbuffer.h>

using namespace HT;



Json_Exporter::Json_Exporter()
{

}

Json_Exporter::~Json_Exporter()
{

}

QJsonValue jsonValFromByteArray(const QByteArray &p) {

	QByteArray const encoded = p.toBase64();
	return { QLatin1String(encoded) };
}

bool Json_Exporter::SaveFile(QString File, HapticPlotData* PlotData)
{
	if (PlotData == NULL)
		return false;

	QFile saveFile(File);
	if (!saveFile.open(QIODevice::WriteOnly))
		return false;

	QList<HzPlotData> hzData = PlotData->GetHzPlotData();
	//QByteArray unzipData = data->Serialize();
	QJsonObject jsonObj;

	jsonObj.insert("HeaderDesc", HFS_Manager::Header_Desc);
	jsonObj.insert("Version", HFS_Manager::Header_Version);
	jsonObj.insert("Size", hzData.size());

	///////////////////////////////////////////////////////////////
	QByteArray RawData = PlotData->HzToRaw(hzData);
	QString DataString = "";
	for (int i = 0; i < RawData.count(); i++)
	{
		DataString += QString("%1,").arg((unsigned char)RawData[i]);
	}

	if (DataString.back() == ",")
		DataString = DataString.remove(DataString.size() - 1, 1);

	jsonObj.insert("RawData", DataString);
	//////////////////////////////////////////////////////////


	QByteArray unzipData = PlotData->Serialize();	
	QByteArray compressData = HFS_Manager::Compress(unzipData);
			
	QJsonValue val = jsonValFromByteArray(compressData);

	jsonObj.insert("RawBase64", val);

	QJsonDocument saveDoc(jsonObj);
	saveFile.write(saveDoc.toJson());

	return true;
}
