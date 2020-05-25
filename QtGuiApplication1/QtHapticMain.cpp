#include "QtHapticMain.h"
#include <QtWidgets>
#include "qlistwidget.h"
#include "qcustomplot.h"
#include "HapticEditorWindowHud.h"
#include "HapticEditorMainToolBar.h"
#include "DeviceSelectHud.h"

#include "Device/HapticDeviceManager.h"


using namespace HT;

QtHapticMain* QtHapticMain::mInstance = NULL;
QtHapticMain* QtHapticMain::Instance()
{
	return mInstance;
}

QtHapticMain::QtHapticMain()
{
	undoView = NULL;
	m_Splitter = NULL;
	m_HistoryDock = NULL;
	m_ToolBar = NULL;

	mInstance = this;

	ui.setupUi(this);
	


	m_ToolBar = new HapticEditorMainToolBar(ui.toolBar);

	m_Splitter = new QSplitter(this);
	m_Splitter->setOrientation(Qt::Vertical);


	//////////////////////////////////////////
	m_SplitMainWindow = new QMainWindow(m_Splitter);
	
	m_BottomDockWidget = new QDockWidget("Dock", m_SplitMainWindow);		
	m_BottomDockWidget->setTitleBarWidget(new QWidget()); // remove title bar
	m_BottomDockWidget->setAllowedAreas(Qt::NoDockWidgetArea); // do not allow to dock

	m_EditorHud = new HapticEditorWindowHud(m_SplitMainWindow);

	//m_BottomDockWidget->setSizePolicy(QSizePolicy())	
	m_SplitMainWindow->addDockWidget(Qt::BottomDockWidgetArea, m_BottomDockWidget);

	m_Splitter->addWidget(new DeviceSelectHud(this));
	m_Splitter->addWidget(m_EditorHud);	
	m_Splitter->setSizes(QList<int>({ 250,400 }));

	////////m_Splitter->setBackgroundRole
	////////QColor col = QColor(0, 0, 0, 255);
	////////QString qss1 = QString("QSplitter::handle{background: %1;}").arg(col.name());	
	////////m_Splitter->setStyleSheet(qss1);// "QSplitter::handle{background: red;}");
	////////QColor col2 = QColor(51, 51, 51, 255);
	////////QString qss2 = QString("background-color: %1").arg(col2.name());
	////////m_DeiviceSelectHud->setStyleSheet(qss2);
	////////m_MainWindow->setStyleSheet(qss2);	
	////////this->setStyleSheet(qss2);
	////////m_Splitter->setStyleSheet();

	if (m_Splitter)
	{
		setCentralWidget(m_Splitter);
		createUndoView();
	}
}

QtHapticMain::~QtHapticMain()
{
	if (undoView)
		delete undoView;

	//if (m_HistoryDlg)
	//	delete m_HistoryDlg;

	if (m_ToolBar)
		delete m_ToolBar;

	m_ToolBar = NULL;
	undoView = NULL;
	m_HistoryDock = NULL;
}

//static int frameCount = 0;
//void QtHapticMain::UpdateDevice()
//{	
//	int inteval = mTimer->interval();
//
//	HapticDeviceManager::instance()->Update(inteval);
//	frameCount++;
//}

void QtHapticMain::createUndoView()
{
	if (m_EditorHud == NULL)
		return;

	QUndoStack* undoStack = m_EditorHud->GetUndoStack();

	undoView = new QUndoView(undoStack, this);
	
	undoView->setWindowTitle(tr("Command List"));
	undoView->setAttribute(Qt::WidgetAttribute::WA_QuitOnClose, false);

	undoView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	
	m_HistoryDock = new QDockWidget(tr("History"), this);
	m_HistoryDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	m_HistoryDock->setWidget(undoView);

	m_HistoryDock->resize(QSize(300, 300));

	//this->removeDockWidget(m_HistoryDock);
	

	addDockWidget(Qt::RightDockWidgetArea, m_HistoryDock);
	
	m_HistoryDock->hide();

	//QVBoxLayout *layout = new QVBoxLayout(m_HistoryDlg);
	//m_HistoryDlg->RemoveT
	//layout->setContentsMargins(0, 0, 0, 0);
	//layout->addWidget(undoView);
	//if(HapticEditorMainToolBar::mbShowHistory)
	//	m_HistoryDlg->show();
	//else
	//	m_HistoryDlg->hide();

}

void QtHapticMain::ToggleHistoryHud()
{
	if (m_HistoryDock == NULL)
		return;
	
	if(m_HistoryDock->isHidden())
		m_HistoryDock->show();
	else
		m_HistoryDock->hide();
}

//void QtHapticMain::on_actionRestore_layout_triggered()
//{
//	//QList<QDockWidget*> list = findChildren<QDockWidget*>();
//	//foreach(QDockWidget* dock, list)
//	//{
//	//	if (dock->isFloating())
//	//		dock->setFloating(false);
//
//	//	m_MainWindow->removeDockWidget(dock);
//	//	if (dock == m_BottomDockWidget)
//	//		m_MainWindow->addDockWidget(Qt::TopDockWidgetArea, m_BottomDockWidget);
//	////else
//	//	//	m_TopSideWindow->addDockWidget(Qt::BottomDockWidgetArea, dock);
//	//	
//	//	dock->setVisible(true);
//	//}
//
//	//m_MainWindow->splitDockWidget(m_dockWidget2, m_dockWidget3, Qt::Horizontal);
//}

void QtHapticMain::resizeEvent(QResizeEvent* event)
{
	QMainWindow::resizeEvent(event);

	//if (m_Splitter)
	//	m_Splitter->updateGeometry();

}
