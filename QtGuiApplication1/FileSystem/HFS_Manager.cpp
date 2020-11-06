#include "HFS_Manager.h"
#include "qdebug.h"
#include "HapticPlotData.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include <QFile>
#include "QtHapticMain.h"
#include "HapticEditorWindowHud.h"

#include "H_Exporter.h"
#include "BVF_Exporter.h"
#include "Json_Exporter.h"

#include "H_Importer.h"
#include "BVF_Importer.h"
#include "WAVE_Importer.h"
#include "Json_Importer.h"
#include "lameHelper.h"

using namespace HT;

const double HFS_Manager::Header_Version = 1.0;
const QString HFS_Manager::Header_Desc = "Buffered HapticValue";

QByteArray HFS_Manager::Compress(QByteArray &uncompressed_data)
{
	return qCompress(uncompressed_data, 9);
}

QByteArray HFS_Manager::UnCompress(QByteArray &compressed_data)
{
	return qUncompress(compressed_data);
}

HFS_Manager::HFS_Manager()
{

}

HFS_Manager::~HFS_Manager()
{

}

HFS_Manager* HFS_Manager::createInstance()
{
	return new HFS_Manager();
}


//
//bool BVF_Exporter::SaveDialog( HapticPlotData* PlotData)
//{
//	if (PlotData->GetHzPlotData().size() == 0)
//		return false;
//
//	QString fileName = QFileDialog::getSaveFileName(NULL, tr("Save File"),
//		"/data/*.bvf",
//		tr("bvf file (*.bvf)"));
//
//	if (fileName.isEmpty())
//		return false;
//
//	bool bRet =  SaveFile(fileName, PlotData);	
//
//	if (bRet == false)
//	{
//		QMessageBox msgBox;
//		msgBox.setWindowTitle(QString::fromLocal8Bit("경고"));
//		msgBox.setIcon(QMessageBox::Warning);
//		msgBox.setText(QString::fromLocal8Bit("저장 실패"));
//		msgBox.setStandardButtons(QMessageBox::Yes);
//		msgBox.exec();
//	}
//	return bRet;
//
//}
bool HFS_Manager::SaveDialog(HapticPlotData* ExportData, QString &outPath)
{
	if (ExportData == NULL)
		return false;

	if (ExportData->GetHzPlotData().size() == 0)
		return false;

	QString selfilter = tr("Json(*.json);;Bvf(*.bvf)");

	QString filePath = QFileDialog::getSaveFileName(NULL, tr("Save File"),
		"/data/",
		tr("Json (*.json);;Bvf file (*.bvf)"));// &selfilter);

	if (filePath.isEmpty())
		return false;

	QFileInfo info(filePath);
	QString suffix = info.suffix();

	H_Exporter *Exporter = NULL;

	if (suffix == "bvf")  
		Exporter = new BVF_Exporter();
	else  if (suffix == "json") 
		Exporter = new Json_Exporter();

	if (Exporter == NULL)
		return false;

	bool bRet = Exporter->SaveFile(filePath, ExportData);

	QMessageBox msgBox;
	if (bRet == false)
	{
		msgBox.setWindowTitle(QString::fromLocal8Bit("정보"));
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setText(QString::fromLocal8Bit("저장 실패"));
	}
	else
	{
		msgBox.setWindowTitle(QString::fromLocal8Bit("정보"));
		msgBox.setIcon(QMessageBox::Information);
		msgBox.setText(QString::fromLocal8Bit("저장 완료"));
	}

	msgBox.setStandardButtons(QMessageBox::Yes);
	msgBox.exec();

	return bRet;
}

bool HFS_Manager::LoadDialog(HapticPlotData* ImportData, QString &outPath)
{
	QString selfilter = tr("vibrations Files (*.json *.bvf *.wav *.mp3)");
//	QString selected_filter = tr("vibrations files(*.json *.bvf *.wav");
	QString filePath = QFileDialog::getOpenFileName(NULL, tr("Open File"),
		"/data/", selfilter);// , &selfilter);

	QFileInfo info(filePath);
	QString suffix = info.suffix();

	H_Importer *Importer = NULL;
	
	if (suffix == "bvf")  Importer = new BVF_Importer();
	else if (suffix == "wav") Importer = new WAVE_Importer();
	else if (suffix == "json") Importer = new Json_Importer();
	else if (suffix == "mp3") 
	{
		QDir _dir("./wav");
		if (_dir.exists())
			_dir.removeRecursively();
		if (!_dir.exists())
			_dir.mkpath(".");
		lameHelper lhHandle;
		QByteArray ba1 = filePath.toLocal8Bit();
		char* c_str1 = ba1.data();
		QString fileName = "./wav/"+info.baseName()+".wav" ;
		QByteArray ba2 = fileName.toLocal8Bit();
		char* c_str2 = ba2.data();
		lhHandle.decode(c_str1,c_str2);
		filePath = "./wav/" + info.baseName() + ".wav";
		Importer = new WAVE_Importer();
	}

	if (Importer == NULL)
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle(QString::fromLocal8Bit("경고"));
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setText(QString::fromLocal8Bit("지원하지 않는 파일 형식 입니다."));
		msgBox.setStandardButtons(QMessageBox::Yes);
		msgBox.exec();
		return false;
	}

	bool bRet = Importer->LoadFile(filePath, ImportData);

	if (bRet == true)
	{
		outPath = filePath;
		QtHapticMain::Instance()->getEditorHud()->SetPlotData(ImportData);
	}
	else
	{
		delete ImportData;
		ImportData = NULL;
		QMessageBox msgBox;
		msgBox.setWindowTitle(QString::fromLocal8Bit("경고"));
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setText(QString::fromLocal8Bit("로딩 실패"));
		msgBox.setStandardButtons(QMessageBox::Yes);
		msgBox.exec();
	}

	return bRet;
}

