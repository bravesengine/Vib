
#include <QScopeGuard>
#include <HapticEditorMainToolBar.h>
#include "QtHapticMain.h"
#include "qtoolbar.h"
#include "qdir.h"
#include "HapticEditorWindowHud.h"
#include "HapticPlotGraph.h"
#include "Parser/TableManager.h"
#include "Samples.h"
//#include "x.h"
#include <QtCore>
#include "GenDocument.h"

//#include "BVF_Exporter.h"
//#include "BVF_Importer.h"

#include "WaveToHaptic.h"
#include "HFS_Manager.h"

//#include "xlsxdocument.h"
bool HapticEditorMainToolBar::mbShowHistory = false;

using namespace HT;
static QIcon CreateIcon(QString icoFile)
{
	QDir dir(".");
	QImage q(dir.absolutePath() + "/Resources/" + icoFile);// "Play.png");
	QIcon Icon = QIcon(QPixmap::fromImage(q));
	return Icon;
}

HapticEditorMainToolBar::HapticEditorMainToolBar( QToolBar *toolBar)
{	
	m_MainToolBar = toolBar;
	//QHBoxLayout* verticalLayout_5 = new QHBoxLayout(this);
	//verticalLayout_5->setSpacing(0);
	//verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
	//verticalLayout_5->setContentsMargins(0, 0, 0, 0);
	//verticalLayout_5->addWidget(new QLabel("AAAA"));
	createActions();
	createMenus();
	createToolBars();	
}

HapticEditorMainToolBar::~HapticEditorMainToolBar()
{
	
	
}

int HapticEditorMainToolBar::ShowWarningBox()
{
	QMessageBox msgBox;
	msgBox.setWindowTitle(QString::fromLocal8Bit("경고"));
	msgBox.setIcon(QMessageBox::Warning);
	msgBox.setText(QString::fromLocal8Bit("저장하지 않으면 기존 작업내용은 사라집니다\n저장하시겠습니까?"));
	msgBox.setStandardButtons(QMessageBox::Yes);
	msgBox.addButton(QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);

	return msgBox.exec();

	//if (msgBox.exec() == QMessageBox::Yes) {
	//	// do something
	//	onSave();
	//}

}

void HapticEditorMainToolBar::onNewDoc()
{
	QtHapticMain* MainWindow = QtHapticMain::Instance();
	if (MainWindow == NULL)
		return;


	GenDocument doc;

	doc.setModal(true);
	doc.show();
	int Ret = doc.exec();

	if (!doc.IsOk())
		return;

	//-- 뭔가 작업중이라면.. 경고 메세지..
	HapticPlotData* PlotData = MainWindow->getEditorHud()->GetPlotData();
	if (PlotData->GetHzPlotData().size() != 0)
	{
		if (ShowWarningBox() == QMessageBox::Yes)
			onSave();
	}

	int tableIndex = doc.GetSelectionIndex();
	int RepeatCount = doc.GetRepeatCount();

	QSharedPointer<Samples> data = TableManager::instance()->GetSampleTable();
	if (data.get() == NULL || tableIndex < 0)
		return;

	if (tableIndex+1 > data.get()->countOfSamples())
		return;

	Sample sampleData = data.get()->getSampleAt(tableIndex);

	QStringList RawDataStrings = sampleData.getData().split(",");

	QList<quint8> RawDataEntity;

	bool ok = false;
	for (int i = 0; i < RawDataStrings.count(); i++)
	{
		ok = false;
		quint8 RawInt8Data =  (quint8) RawDataStrings[i].toInt(&ok);
		if(ok)	
			RawDataEntity.append(RawInt8Data);
	}

	QList<quint8> RawData;

	for (int i = 0; i < RepeatCount; i++)
	{
		RawData += RawDataEntity;
	}

	PlotData->DataToReMakeTime(PlotData->MakePlotData(RawData));// , true));
	MainWindow->getEditorHud()->InitPlotData();
	MainWindow->getEditorHud()->GetGraph()->getCustomPlot()->replot();
	
	MainWindow->getEditorHud()->UpdateWorkDesc(QString("Sample : %1 , Repeat=%2").arg(sampleData.getTitle()).arg(RepeatCount));


}

void  HapticEditorMainToolBar::onLoad()
{	
	QtHapticMain* MainWindow = QtHapticMain::Instance();
	if (MainWindow == NULL)
		return;

	//-- 뭔가 작업중이라면.. 경고 메세지..
	HapticPlotData* PlotData = MainWindow->getEditorHud()->GetPlotData();
	if (PlotData->GetHzPlotData().size() != 0)
	{
		if (ShowWarningBox() == QMessageBox::Yes)
			onSave();
	}

	HapticPlotData* ImportPlotData = new HapticPlotData();

	QString filePath;
	bool bSuccessLod = HFS_Manager::instance()->LoadDialog(ImportPlotData, filePath);

	if (bSuccessLod)
	{
		MainWindow->getEditorHud()->InitPlotData();
		MainWindow->getEditorHud()->GetGraph()->getCustomPlot()->replot();
		MainWindow->getEditorHud()->UpdateWorkDesc(filePath);
	}
	
}

void  HapticEditorMainToolBar::onSave()
{
	QtHapticMain* MainWindow = QtHapticMain::Instance();
	if (MainWindow == NULL)
		return;

	HapticPlotData* PlotData = MainWindow->getEditorHud()->GetPlotData();
	QString SavePath;
	HFS_Manager::instance()->SaveDialog(PlotData, SavePath);
}

void  HapticEditorMainToolBar::onShowHistory()
{
	QtHapticMain* MainWindow = QtHapticMain::Instance();
	if (MainWindow == NULL)
		return;

	mbShowHistory = !mbShowHistory;
	MainWindow->ToggleHistoryHud();// (mbShowHistory);
}




void HapticEditorMainToolBar::createActions()
{		
	//QAction *mNewDocAct;
	//QAction *mLoadAct;
	//QAction *mSaveAct;
	
	mNewDocAct = new QAction(CreateIcon("document.png"), tr("&New Document"), this);
	mNewDocAct->setShortcut(tr("New"));
	connect(mNewDocAct, SIGNAL(triggered()), this, SLOT(onNewDoc()));

	//mWavetoDocAct = new QAction(CreateIcon("wavetohap.png"), tr("&Wave to Haptic"), this);
	//mWavetoDocAct->setShortcut(tr("Wave To NewDoc"));
	//connect(mWavetoDocAct, SIGNAL(triggered()), this, SLOT(onWaveToDoc()));

	mLoadAct = new QAction(CreateIcon("load.png"), tr("&Load Haptic Data"), this);
	mLoadAct->setShortcut(tr("Load"));
	connect(mLoadAct, SIGNAL(triggered()), this, SLOT(onLoad()));

	mSaveAct = new QAction(CreateIcon("save.png"), tr("&Save Haptic Data"), this);
	mSaveAct->setShortcut(tr("Save"));
	connect(mSaveAct, SIGNAL(triggered()), this, SLOT(onSave()));

	mShowHistoryAct = new QAction(CreateIcon("history.png"), tr("&Show History"), this);
	mShowHistoryAct->setShortcut(tr("Show History"));
	connect(mShowHistoryAct, SIGNAL(triggered()), this, SLOT(onShowHistory()));


}

void HapticEditorMainToolBar::createMenus()
{
}

void HapticEditorMainToolBar::createToolBars()
{
	/* if (m_MainToolBar == NULL)
	 {
		 m_MainToolBar = new QToolBar((QWidget*)mParent);
		 m_MainToolBar->setToolTip(tr("Editor ToolBar"));
		 m_MainToolBar->setObjectName(QStringLiteral("toolBar"));

		 m_MainToolBar->setMinimumSize(QSize(500, 40));
		 m_MainToolBar->setMaximumSize(QSize(16777215, 40));
		 m_MainToolBar->setIconSize(QSize(40, 40));
	 }*/

	m_MainToolBar->setIconSize(QSize(42, 42));
	//m_MainToolBar->setMinimumSize(QSize(500, 42));

	m_MainToolBar->addAction(mNewDocAct);

	//m_MainToolBar->addSeparator();

	m_MainToolBar->addAction(mLoadAct);
	m_MainToolBar->addAction(mSaveAct);
	m_MainToolBar->addAction(mShowHistoryAct);


	//m_MainToolBar->addAction(mWavetoDocAct);




	//QWidget* empty = new QWidget();
	//empty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);	
	//m_MainToolBar->addWidget(empty);
	// this will be right aligned

	//QLabel *cp = new QLabel("BRAVES ");
	//cp->setStyleSheet("font: bold; font-size: 20px; color : rgb(170,170,170);");

//	m_MainToolBar->insertWidget(NULL, cp);// 5)

	//QAction *test = new QAction(this);
	//test->setText("RIGHT");
	//m_MainToolBar->addAction(test);

	// m_MainToolBar->addSeparator();
}
