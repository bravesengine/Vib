
#include <HapticEditorToolBar.h>
#include "HapticEditorWindowHud.h"
#include "qtoolbar.h"
#include "qdir.h"

#include "QToolButtonWacher.h"
#include "QTimer.h"

#include "QToolButton.h"
#include "HapticPlotDataSelectAction.h"
#include "HapticModifFunc.h"
#include "UndoDataTypes.h"
#include "HapticPlotGraph.h"
#include "Device/HapticDeviceManager.h"
#include "DeviceSelectHud.h"

using namespace HT;
HapticEditorToolBar::HapticEditorToolBar(HapticEditorWindowHud *parent, QToolBar* toolBar, QUndoStack* undoStack)//, QToolBar *toolBar)
{
	if (parent == NULL)
		return;
	
	mUndoStack = undoStack;

	m_Parent = parent;

	//mPrevGraphState = NULL;

    m_MainToolBar = toolBar;
	mAmpUpWacher = NULL;
	mAmpDownWacher = NULL;

	createActions(undoStack);
	createMenus(parent);
	createToolBars(parent);	

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(ProcessAction()));
	timer->start(100);
}

HapticEditorToolBar::~HapticEditorToolBar()
{
	//if (mPrevGraphState)
	//	delete mPrevGraphState;

	if (mAmpUpWacher)
		delete mAmpUpWacher;
	if (mAmpDownWacher)
		delete mAmpDownWacher;

	mAmpUpWacher = NULL;
	mAmpDownWacher = NULL;
	//mPrevGraphState = NULL;
}

void HapticEditorToolBar::ProcessAction()
{
	if (mAmpUpWacher == NULL || mAmpDownWacher == NULL)
		return;

	if (mAmpUpWacher->IsMouseOver() && mAmpUpWacher->IsPress())
	{
		m_Parent->BeginModifGraphData(HapticEditorWindowHud::TOOLBAR, HaticModifGraph::eModifGraphDataType::AMP_UP);
		m_Parent->UpdateModifGraphData(HapticEditorWindowHud::TOOLBAR, HaticModifGraph::eModifGraphDataType::AMP_UP);

	
	}
	else if (mAmpDownWacher->IsMouseOver() && mAmpDownWacher->IsPress())
	{
		m_Parent->BeginModifGraphData(HapticEditorWindowHud::TOOLBAR, HaticModifGraph::eModifGraphDataType::AMP_DOWN);
		m_Parent->UpdateModifGraphData(HapticEditorWindowHud::TOOLBAR, HaticModifGraph::eModifGraphDataType::AMP_DOWN);
	}
}

static QIcon CreateIcon(QString icoFile)
{
	QDir dir(".");
	QImage q(dir.absolutePath() + "/Resources/" + icoFile);// "Play.png");
	QIcon Icon = QIcon(QPixmap::fromImage(q));
	return Icon;
}

void HapticEditorToolBar::onPlayVibraion()
{
	//-- 현재 선택된 Device 가져오기.
	if (DeviceSelectHud::mCurSelectDeviceType == DeviceSelectHud::NONE)
		return;

	HapticDeviceManager::eDeviceType DeviceType = HapticDeviceManager::eDeviceType::NONE;
	int SubControllerID = 0;
	if (DeviceSelectHud::mCurSelectDeviceType == DeviceSelectHud::OVR_TOUCH_L)
	{
		DeviceType = HapticDeviceManager::eDeviceType::OCULUS_CONTROLLER;
		//SubControllerID = 0;
	}
	else if (DeviceSelectHud::mCurSelectDeviceType == DeviceSelectHud::OVR_TOUCH_R)
	{
		DeviceType = HapticDeviceManager::eDeviceType::OCULUS_CONTROLLER;
		SubControllerID = 1;
	}
	else if (DeviceSelectHud::mCurSelectDeviceType == DeviceSelectHud::X_INPUT)
	{
		DeviceType = HapticDeviceManager::eDeviceType::XBOXPAD_CONTROLLER;
	}

	if (DeviceType == HapticDeviceManager::eDeviceType::NONE)
		return;

	if (HapticDeviceManager::instance()->IsRunning(DeviceType, SubControllerID))
		return;

	if (!HapticDeviceManager::instance()->isConnected(DeviceType))
		return;

	const QList<HzPlotData> &hzData = m_Parent->GetPlotData()->GetHzPlotData();
	QByteArray RawData = m_Parent->GetPlotData()->HzToRaw(hzData);
	float timelength = m_Parent->GetPlotData()->GetTimeLength();

	HapticDeviceManager::instance()->StopVibraion(DeviceType);
	HapticDeviceManager::instance()->ClearVibraionClip(DeviceType);
	HapticDeviceManager::instance()->InsertVibraionClip(DeviceType, SubControllerID, RawData, timelength);
	HapticDeviceManager::instance()->RunVibraion(DeviceType);
}

void HapticEditorToolBar::onStopVibraion()
{
	if (DeviceSelectHud::mCurSelectDeviceType == DeviceSelectHud::NONE)
		return;
	HapticDeviceManager::eDeviceType DeviceType = HapticDeviceManager::eDeviceType::NONE;

	if (DeviceSelectHud::mCurSelectDeviceType == DeviceSelectHud::OVR_TOUCH_L)
	{
		DeviceType = HapticDeviceManager::eDeviceType::OCULUS_CONTROLLER;
	}
	else if (DeviceSelectHud::mCurSelectDeviceType == DeviceSelectHud::OVR_TOUCH_R)
	{
		DeviceType = HapticDeviceManager::eDeviceType::OCULUS_CONTROLLER;
	}
	else if (DeviceSelectHud::mCurSelectDeviceType == DeviceSelectHud::X_INPUT)
	{
		DeviceType = HapticDeviceManager::eDeviceType::XBOXPAD_CONTROLLER;
	}

	if (DeviceType == HapticDeviceManager::eDeviceType::NONE)
		return;

	if (!HapticDeviceManager::instance()->isConnected(DeviceType))
		return;

	HapticDeviceManager::instance()->StopVibraion(DeviceType);
	//HapticDeviceManager::instance()->ClearVibraionClip(DeviceType);
}


//-- Mouse Release Message..
void HapticEditorToolBar::onAmpUpVibraion()
{
	if (m_Parent == NULL)
		return;

	if (HapticDeviceManager::instance()->IsRunning())
		return;
	
	m_Parent->EndModifGraphData(HapticEditorWindowHud::TOOLBAR, HaticModifGraph::eModifGraphDataType::AMP_UP, true);
}

void HapticEditorToolBar::onAmpDownVibraion()
{
	if (m_Parent == NULL)
		return;

	if (HapticDeviceManager::instance()->IsRunning())
		return;

	m_Parent->EndModifGraphData(HapticEditorWindowHud::TOOLBAR, HaticModifGraph::eModifGraphDataType::AMP_DOWN, true);
}


void HapticEditorToolBar::onHzUpCommand()
{
	if (HapticDeviceManager::instance()->IsRunning())
		return;

	bool bRet = m_Parent->BeginModifGraphData(HapticEditorWindowHud::TOOLBAR, HaticModifGraph::eModifGraphDataType::HZ_UP);
	m_Parent->UpdateModifGraphData(HapticEditorWindowHud::TOOLBAR, HaticModifGraph::eModifGraphDataType::HZ_UP);

	m_Parent->EndModifGraphData(HapticEditorWindowHud::TOOLBAR, HaticModifGraph::eModifGraphDataType::HZ_UP, bRet);

}
void HapticEditorToolBar::onHzDownCommand()
{
	if (HapticDeviceManager::instance()->IsRunning())
		return;

	bool bRet = m_Parent->BeginModifGraphData(HapticEditorWindowHud::TOOLBAR, HaticModifGraph::eModifGraphDataType::HZ_DOWN);
	m_Parent->UpdateModifGraphData(HapticEditorWindowHud::TOOLBAR, HaticModifGraph::eModifGraphDataType::HZ_DOWN);
	m_Parent->EndModifGraphData(HapticEditorWindowHud::TOOLBAR, HaticModifGraph::eModifGraphDataType::HZ_DOWN, bRet);
}

void HapticEditorToolBar::onUnDoCommand()
{
	if (mUndoStack == NULL)
		return;

	if (HapticDeviceManager::instance()->IsRunning())
		return;

	mUndoStack->undo();
}
void HapticEditorToolBar::onReDoCommand()
{
	if (mUndoStack == NULL)
		return;

	if (HapticDeviceManager::instance()->IsRunning())
		return;

	mUndoStack->redo();
}

void HapticEditorToolBar::onRepalyCheck()
{
	//- Replay OnOff?
	bool bRePlay = HapticDeviceManager::instance()->IsReplay();
	bRePlay = !bRePlay;

	HapticDeviceManager::instance()->SetReplay(bRePlay);

	QIcon ico;
	if (bRePlay)
	{
		QDir dir(".");
		QPixmap pixmap = QPixmap(dir.absolutePath() + "/Resources/" + "rePlay_on.png");

		ico.addPixmap(pixmap, QIcon::Normal, QIcon::On);
	}
	else
	{
		QDir dir(".");
		QPixmap pixmap = QPixmap(dir.absolutePath() + "/Resources/" + "rePlay_off.png");
		ico.addPixmap(pixmap, QIcon::Normal, QIcon::On);
	}
	mRePlayAct->setIcon(ico);

}

void HapticEditorToolBar::onPasteGraph()
{
	if (HapticDeviceManager::instance()->IsRunning())
		return;

	qDebug() << "onPasteGraph";

	bool bRet = m_Parent->BeginModifGraphData(HapticEditorWindowHud::TOOLBAR, HaticModifGraph::eModifGraphDataType::PASTH_GRAHP);

	if(bRet)
		m_Parent->UpdateModifGraphData(HapticEditorWindowHud::TOOLBAR, HaticModifGraph::eModifGraphDataType::PASTH_GRAHP);
	
	m_Parent->EndModifGraphData(HapticEditorWindowHud::TOOLBAR, HaticModifGraph::eModifGraphDataType::PASTH_GRAHP, bRet);

}
void HapticEditorToolBar::onCopyGraph()
{
	qDebug() << "onCopyGraph";
	m_Parent->CopySelectedGraphtoClipBoard();
}

void HapticEditorToolBar::onCutGraph()
{
	if (HapticDeviceManager::instance()->IsRunning())
		return;

	qDebug() << "onCutGraph";	
	bool bRet = m_Parent->BeginModifGraphData(HapticEditorWindowHud::TOOLBAR, HaticModifGraph::eModifGraphDataType::CUT_GRAHP, true);

	if(bRet)
		m_Parent->UpdateModifGraphData(HapticEditorWindowHud::TOOLBAR, HaticModifGraph::eModifGraphDataType::CUT_GRAHP);
	
	m_Parent->EndModifGraphData(HapticEditorWindowHud::TOOLBAR, HaticModifGraph::eModifGraphDataType::CUT_GRAHP, bRet);
}


void HapticEditorToolBar::createActions(QUndoStack* undoStack)
{		
	mPlayAct = new QAction(CreateIcon("Play.png"), tr("&Run Vibraion"), this);
	mPlayAct->setShortcut(tr("Run"));
	connect(mPlayAct, SIGNAL(triggered()), this, SLOT(onPlayVibraion()));
	
	mStopAct = new QAction(CreateIcon("Stop.png"), tr("&Stop Vibraion"), this);
	mStopAct->setShortcut(tr("Stop"));
	connect(mStopAct, SIGNAL(triggered()), this, SLOT(onStopVibraion()));

	mRePlayAct = new QAction(CreateIcon("rePlay_off.png"), tr("&Replay Check"), this);
	mRePlayAct->setShortcut(tr("RePlay"));
	connect(mRePlayAct, SIGNAL(triggered()), this, SLOT(onRepalyCheck()));

	//---

	mAmpUpAct = new QAction(CreateIcon("Amp_Up.png"), tr("&AmpUp"), this);
	connect(mAmpUpAct, SIGNAL(triggered()), this, SLOT(onAmpUpVibraion()));
//	connect(mAmpUpAct, SIGNAL(hovered()), this, SLOT(onAmpUpVibraionHovered()));
	mAmpDownAct = new QAction(CreateIcon("Amp_Dw.png"), tr("&AmpDown"), this);
	connect(mAmpDownAct, SIGNAL(triggered()), this, SLOT(onAmpDownVibraion()));

	mHzUpAct = new QAction(CreateIcon("Hz_Up.png"), tr("&Hz Up"), this);
	connect(mHzUpAct, SIGNAL(triggered()), this, SLOT(onHzUpCommand()));

	mHzDownAct = new QAction(CreateIcon("Hz_Dw.png"), tr("&Hz Down"), this);
	connect(mHzDownAct, SIGNAL(triggered()), this, SLOT(onHzDownCommand()));

	mUndoAct = new QAction(CreateIcon("UnDo.png"), tr("&Undo"), this);
	mUndoAct->setShortcut(tr("Ctrl+Z"));
	mUndoAct->setEnabled(false);	
	connect(mUndoAct, SIGNAL(triggered()), this, SLOT(onUnDoCommand()));


	mCutAct = new QAction(CreateIcon("Cut.png"), tr("&Cut Graph"), this);
	mCutAct->setShortcut(tr("Ctrl+X"));

	connect(mCutAct, SIGNAL(triggered()), this, SLOT(onCutGraph()));

	mCopyAct = new QAction(CreateIcon("Copy.png"), tr("&Copy Graph"), this);
	mCopyAct->setShortcut(tr("Ctrl+C"));
	connect(mCopyAct, SIGNAL(triggered()), this, SLOT(onCopyGraph()));

	mPasteAct = new QAction(CreateIcon("Paste.png"), tr("&Paste Graph"), this);
	mPasteAct->setShortcut(tr("Ctrl+V"));
	connect(mPasteAct, SIGNAL(triggered()), this, SLOT(onPasteGraph()));


	
	mRedoAct = new QAction(CreateIcon("ReDo.png"), tr("&Redo"), this);
	QList<QKeySequence> redoShortcuts;	
	redoShortcuts.append(QKeySequence(tr("Ctrl+Y")));
	redoShortcuts.append(QKeySequence(tr("Shift+Ctrl+Z")));
	mRedoAct->setShortcuts(redoShortcuts);
	mRedoAct->setEnabled(false);

	connect(mRedoAct, SIGNAL(triggered()), this, SLOT(onReDoCommand()));

	connect(undoStack, SIGNAL(canRedoChanged(bool)), mRedoAct, SLOT(setEnabled(bool)));
	connect(undoStack, SIGNAL(canUndoChanged(bool)), mUndoAct, SLOT(setEnabled(bool)));

}


//void DeviceSelectHud::UpdateDeviceBtn(int Index, bool bChekable)
//{
//	for each (DEVICE_BUTTON* var in mDeviceBtns)
//	{
//		QIcon ico;
//		if (var->Index != Index)
//		{
//			if (!var->toolButton->isCheckable())
//				continue;
//
//			var->toolButton->setCheckable(false);
//		}
//		else
//		{
//			var->toolButton->setCheckable(bChekable);
//		}
//
//		if (var->toolButton->isCheckable())
//		{
//			ico.addPixmap(var->SelectImg, QIcon::Normal, QIcon::On);
//			var->toolButton->setIcon(ico);
//			var->toolButton->setIconSize(var->SelectImg.rect().size());
//		}
//		else
//		{
//			ico.addPixmap(var->DeSelectImg, QIcon::Normal, QIcon::On);
//			var->toolButton->setIcon(ico);
//			var->toolButton->setIconSize(var->DeSelectImg.rect().size());
//		}
//	}
//}

void HapticEditorToolBar::createMenus(HapticEditorWindowHud *parent)
{
//	//fileMenu = parent->menuBar()->addMenu(tr("&File"));
//	//fileMenu->addAction(ConnectAct);
//
////	fileMenu->addAction(openAct);
//	//editMenu = menuBar()->addMenu(tr("&Edit"));
//	//editMenu->addAction(cutAct);
//	//editMenu->addAction(copyAct);
//	//editMenu->addAction(pasteAct);
//	//menuBar()->addSeparator();

//QMenu *menu = new QMenu();
	//QAction *testAction = new QAction("menu item", this);
	//menu->addAction(testAction);

	//QToolButton* toolButton = new QToolButton();
	//toolButton->setMenu(menu);
	//toolButton->setPopupMode(QToolButton::InstantPopup);

	//QWidgetAction* toolButtonAction = new QWidgetAction(this);
	//toolButtonAction->setDefaultWidget(toolButton);

	////-- 툴바 Action Connect 해서 이벤트 Get
	//QToolBar* toolBar = new QToolBar(this);
	////toolBar->addAction(toolButtonAction);
	//QWidget* placeholder = new QWidget(this);
	//toolBar->insertWidget(new QAction(this), placeholder);
	//placeholder->setWindowSwidth(100);

	//toolBar->addAction("Play");
	//toolBar->addAction("Stop");

	//QWidget* spinWidget = new QWidget;
	//QHBoxLayout* spinLay = new QHBoxLayout(spinWidget);
	//spinLay->addWidget(new QSpinBox(spinWidget));
	//spinLay->setContentsMargins(4, 0, 50, 0); // Here you set the spacing

	//QToolBar* toolbarDocument = new QToolBar(this);
	////toolbarDocument->addWidget(spinWidget);
	//toolbarDocument->addWidget(new QComboBox);

	//QToolBar* toolBar = new QToolBar(this);
	//toolBar->
	//addToolBar(toolbarDocument);
	//QToolBar* toolBar = new QToolBar(this);
	//toolBar->addAction("Play");
	//toolBar->addAction("Stop");
	////toolBar->addWidget(new QLabel("|", this));// addSeparator();
	//toolBar->addAction("UOPOO");
}

bool HapticEditorToolBar::eventFilter(QObject *watched, QEvent *event)
{
	return QObject::eventFilter(watched, event);
}

void HapticEditorToolBar::resizeEvent(QResizeEvent* event)
{
	//event->size();

	//if(m_MainToolBar)
	//	m_MainToolBar->setSizesetSizeIncrement
}

void HapticEditorToolBar::createToolBars(HapticEditorWindowHud *parent)
{
	if (m_MainToolBar == NULL)
	{
		m_MainToolBar = new QToolBar(parent);// parent->getUi().widget_2);
		m_MainToolBar->setToolTip(tr("Editor ToolBar"));
		m_MainToolBar->setObjectName(QStringLiteral("toolBar"));

		m_MainToolBar->setIconSize(QSize(64, 64));
		m_MainToolBar->setMinimumSize(QSize(15 * 64, 64));
		m_MainToolBar->setMaximumSize(QSize(16777215, 64));

		QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
		//sizePolicy1.setHorizontalStretch(0);
		//sizePolicy1.setVerticalStretch(0);
		//sizePolicy1.setHeightForWidth(parent->sizePolicy().hasHeightForWidth());

		//frame_2->setSizePolicy(sizePolicy1);
		m_MainToolBar->setSizePolicy(sizePolicy1);
	}



	m_MainToolBar->addAction(mPlayAct);	
	m_MainToolBar->addAction(mStopAct);

	m_MainToolBar->addAction(mRePlayAct);

	m_MainToolBar->addSeparator();

	m_MainToolBar->addAction(mAmpUpAct);
	m_MainToolBar->addAction(mAmpDownAct);
	m_MainToolBar->addAction(mHzUpAct);
	m_MainToolBar->addAction(mHzDownAct);

	m_MainToolBar->addSeparator();

	m_MainToolBar->addAction(mCutAct);
	m_MainToolBar->addAction(mCopyAct);
	m_MainToolBar->addAction(mPasteAct);

	m_MainToolBar->addAction(mUndoAct);
	m_MainToolBar->addAction(mRedoAct);




	mAmpUpWacher = new QToolButtonWacher(dynamic_cast<QToolButton*>(m_MainToolBar->widgetForAction(mAmpUpAct)));
	mAmpDownWacher = new  QToolButtonWacher(dynamic_cast<QToolButton*>(m_MainToolBar->widgetForAction(mAmpDownAct)));
	//m_MainToolBar->installEventFilter(this);

}
