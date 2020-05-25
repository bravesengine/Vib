#include "HapticEditorWindowHud.h"

#include <QtWidgets>
#include "qcustomplot.h"

#include "HapticPlotData.h"
#include "HapticPlotDataSelectAction.h"
#include "HapticPlotGraph.h"
#include "HapticPlotKeyAction.h"
#include "HapticEditorToolBar.h"
#include "HapticEditorPopupMenu.h"

#include "UndoCommands.h"
#include "UndoDataTypes.h"

#include "Device/HapticDeviceManager.h"

#include "HapticModifFunc.h"

////undoAction = new QAction(tr("&Undo"), this);
////undoAction->setShortcut(tr("Ctrl+Z"));
////undoAction->setEnabled(false);

//connect(undoAction, SIGNAL(triggerd()), undoAction, "undo()");// SLOT(undo));
////undoAction->triggered.connect(undoStack, "undo()");



//redoAction = new QAction(tr("&Redo"), this);
////
////QList<QKeySequence*> *redoShortcuts = new QList<QKeySequence*>();
////
////redoShortcuts->append(new QKeySequence(tr("Ctrl+Y")));
////redoShortcuts->append(new QKeySequence(tr("Shift+Ctrl+Z")));
//
////redoAction->setShortcuts(redoShortcuts);

////redoAction.setEnabled(false);
////redoAction.triggered.connect(undoStack, "redo()");

using namespace HT;
HapticEditorWindowHud::HapticEditorWindowHud(QWidget *parent) : QWidget(parent)
{
	mPrevStateData = NULL;
	mToolBar = NULL;

	undoStack = new QUndoStack();

	ui.setupUi(this);

	ui.Plot->setMinimumHeight(150);
	//ui.Plot->setMaximumHeight(200);

	m_EventSignal = 
		std::bind(&HapticEditorWindowHud::OnHapticDeviceEvent, this, std::placeholders::_1, std::placeholders::_2);

	QMainWindow* ParentWin = qobject_cast<QMainWindow*>(parent);

//	if(ParentWin == NULL)
		mToolBar = new HapticEditorToolBar(this,  NULL, undoStack);
//	else
//		mToolBar = new HapticEditorToolBar(this, ParentWin->addToolBar("TEST"), undoStack);

	mPopUpMenu = new HapticEditorPopupMenu(this);

	//undoStack.canRedoChanged.connect(redoAction, "setEnabled(boolean)");
	//undoStack.canUndoChanged.connect(undoAction, "setEnabled(boolean)");


	m_HapticData = new HapticPlotData();
	//m_HapticData->MakeRandom(0);

	m_HapticGraph = new HapticPlotGraph(ui.Plot);

	m_TimeLength = 0.0f;
	
	m_SelectAction = new HapticPlotDataSelectAction(this);// std::bind(&HapticEditorWindowHud::UpdatePlotHud, this));


	mGraphModifFunc.insert(HaticModifGraph::eModifGraphDataType::AMP_UP, new HapticModifFuncAmpUp());
	mGraphModifFunc.insert(HaticModifGraph::eModifGraphDataType::AMP_DOWN, new HapticModifFuncAmpDown());

	mGraphModifFunc.insert(HaticModifGraph::eModifGraphDataType::HZ_DOWN, new HapticModifFuncHzDown());
	mGraphModifFunc.insert(HaticModifGraph::eModifGraphDataType::HZ_UP, new HapticModifFuncHzUp());

	mGraphModifFunc.insert(HaticModifGraph::eModifGraphDataType::PASTH_GRAHP, new HapticModifFuncPasth());

	mGraphModifFunc.insert(HaticModifGraph::eModifGraphDataType::CUT_GRAHP, new HapticModifFuncCut());



	InitPlotData();
}

void HapticEditorWindowHud::Test()
{
//	int xxx = 0;
}

HapticEditorWindowHud::~HapticEditorWindowHud()
{
	if (mToolBar)
		delete mToolBar;

	if (m_SelectAction)
		delete m_SelectAction;

	if (m_HapticData)
		delete m_HapticData;

	if (m_HapticGraph)
		delete m_HapticGraph;

	if (undoStack)
		delete undoStack;

	if (mPrevStateData)
		delete mPrevStateData;
	
	mPrevStateData = NULL;

	m_SelectAction = NULL;
	m_HapticData = NULL;
	m_HapticGraph = NULL;
	//mToolBar = NULL;
	undoStack = NULL;
}

void HapticEditorWindowHud::Put(QString logText)
{
	//plain_text_edit->clear();
	//ui.plainTextEdit->appendPlainText(logText);
}

void HapticEditorWindowHud::resizeEvent(QResizeEvent* event)
{
	if (mToolBar)
		mToolBar->resizeEvent(event);
	//ui.verticalLayoutWidget->setGeometry(QRect(0, 40, event->size().width(), event->size().height()-50));
	//ui.verticalLayout->setmin
//	if ((player != 0) && ((player->isPlaying()) || player->isLoaded() || player > isLoaded())) {
//		renderer->resize(ui->mainVideoWidget->width(), ui->mainVideoWidget->height());
//		resizeFilter();
}

void HapticEditorWindowHud::keyPressEvent(QKeyEvent *e)
{
	if (e == NULL)
		return;

	HapticPlotKeyAction::instance()->KeyPress(e, this);
	//if (m_SelectAction != NULL)
	//	m_SelectAction->keyPressEvent(e);

	QWidget::keyPressEvent(e);
}

void HapticEditorWindowHud::keyReleaseEvent(QKeyEvent * e)
{
	if (e == NULL)
		return;

	HapticPlotKeyAction::instance()->KeyRelease(e, this);
	//if (e->isAutoRepeat())
	//	return;
	////ui.DebugDesc->setText(QString("Release + %1").arg(e->key()));
	//if (m_SelectAction != NULL)
	//	m_SelectAction->keyReleaseEvent(e);

	QWidget::keyReleaseEvent(e);
}

bool HapticEditorWindowHud::SetPlotData(HapticPlotData* PlotData)
{
	if (m_HapticData != NULL)
		delete m_HapticData;

	if (m_SelectAction)
		m_SelectAction->SetPlotData(PlotData);


	m_HapticData = PlotData;
	return true;
}

void HapticEditorWindowHud::InitPlotData()
{
	if (m_HapticGraph == NULL)
		return;

	undoStack->clear();

	m_TimeLength = m_HapticData->GetTimeLength();
	m_HapticGraph->SetGraph(m_HapticData->GetHzPlotData(), m_HapticData->GetTimeLength());

	m_SelectAction->ClearItems();

	UpdatePlotHud(true);
	
	// create connection between axes and scroll bars:	
	connect(ui.horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horzScrollBarChanged(int)));
	connect(ui.Plot->xAxis, SIGNAL(rangeChanged(QCPRange, QCPRange)), this, SLOT(xAxisChanged(QCPRange, QCPRange)));
	connect(ui.Plot->yAxis, SIGNAL(rangeChanged(QCPRange, QCPRange)), this, SLOT(yAxisChanged(QCPRange, QCPRange)));

	updateScrollBarStep();
}


void HapticEditorWindowHud::UpdateWorkDesc(QString desc)
{	
	ui.WorkStateLabel->setText(desc);
}


void HapticEditorWindowHud::UpdateHelpDesc()
{
	getUi().DescAmpValueLabel->setText(QString("None Selected"));

	if (m_HapticGraph == NULL || m_SelectAction == NULL || m_HapticData == NULL)
		return;

	QPoint hzGraphIndex = m_SelectAction->GetCurretSelectHzGraphIndex();

	QString TotPlayTime = m_HapticData->GetTimeString(m_TimeLength);

	if (hzGraphIndex.x() == -1 || hzGraphIndex.y() == -1)
	{
		//  전체 플레이 시간.
		QString PlayTime = m_HapticData->GetTimeString(m_TimeLength);
		ui.HapticLengthLabel->setText(TotPlayTime);
		return;
	}

	// last  1개선택시
	if (hzGraphIndex.x()+1 == hzGraphIndex.y() || hzGraphIndex.x() == hzGraphIndex.y())
	{
		quint8 ampValue = m_HapticData->GetHzPlotData()[hzGraphIndex.x()].HapticRawData;
		getUi().DescAmpValueLabel->setText(QString("Amplitude Value : %1").arg(ampValue));

		QString PlayTime = m_HapticData->GetPlayTimeLengthRangeToString(hzGraphIndex);		
	
		//getUi().HapticLengthLabel->setText(PlayTime + "\n" + TotPlayTime);
		getUi().HapticLengthLabel->setText(PlayTime);

	}
	else  // 여러개 선택시.
	{
		// 선택영역의 시간, 파장갯수, 파장 최대갓.
		float TimeLength = 0;
		int SelectedGraphCount = 0;
		quint8 HigherAmpValue = 0;
		if (!m_HapticData->GetSelectedDeatialDesc(hzGraphIndex, TimeLength, SelectedGraphCount, HigherAmpValue))
		{
			getUi().DescAmpValueLabel->setText(QString("Invalid Selected!!"));
			return;
		}

		QString PlayTime = m_HapticData->GetPlayTimeLengthRangeToString(hzGraphIndex);
		//getUi().HapticLengthLabel->setText(PlayTime + "\n" + TotPlayTime);
		getUi().HapticLengthLabel->setText(PlayTime);

		QString TimeStr = m_HapticData->GetTimeString(TimeLength * 1000);

		getUi().DescAmpValueLabel->setText(QString("Time :%1, Selected Graph = %2\n Higher Amplitude = %3").arg(TimeStr).
			arg(SelectedGraphCount).arg(HigherAmpValue));
	}

}

void HapticEditorWindowHud::UpdatePlotHud(bool bUpdateRange)
{
	if (m_HapticGraph == NULL)
		return;

	m_TimeLength = m_HapticData->GetTimeLength();
	//QDateTime
	UpdateHelpDesc();

	//qDebug() << "HapticEditorWindowHud::UpdatePlotHud()";
	if(bUpdateRange)
		ui.horizontalScrollBar->setRange(0, (int)m_TimeLength);

	updateScrollBarStep();	
}

bool HapticEditorWindowHud::UpdateModifGraphData(eActType act, HaticModifGraph::eModifGraphDataType t)
{
	if (m_SelectAction == NULL)
		return false;

	QCPItemRect *CurRectItem = m_SelectAction->GetCurItemRect();
	if (CurRectItem == NULL)
		return false;
 
	if (!CurRectItem->visible()) return false;

	//if (CurRectItem->topLeft->key() == 0, CurRectItem->bottomRight->key() == 0)
	//	return false;

	bool bRet = mGraphModifFunc[(int)t]->RunModif(m_HapticGraph, m_HapticData, m_SelectAction,
		CurRectItem->topLeft->key(), CurRectItem->bottomRight->key(), CurRectItem);
	
	//HapticPlotDataSelectAction
	if (bRet)
	{
		m_HapticGraph->getCustomPlot()->replot();
		UpdateHelpDesc();
	}

	return bRet;
}

bool HapticEditorWindowHud::EndModifGraphData(eActType act, HaticModifGraph::eModifGraphDataType t, bool bInsertCommad)
{
	if (mPrevStateData == NULL)
		return false;

	QCPItemRect *CurRectItem = m_SelectAction->GetCurItemRect();
	if (CurRectItem == NULL)
		return false;

	if (bInsertCommad)
	{
		UnDo_GraphStateData CurStateData;
		CurStateData.m_PlotData = m_HapticData->GetHzPlotData();
		CurStateData.m_SelectLeftCoord = CurRectItem->topLeft->coords();
		CurStateData.m_SelectRightCoord = CurRectItem->bottomRight->coords();
		CurStateData.m_TotTime = m_HapticData->GetTimeLength();
		CurStateData.m_ActType = (int)act;

		CurStateData.CopyClipBoardData = mPrevStateData->CopyClipBoardData;
		CurStateData.PrevClipBoardData = mPrevStateData->PrevClipBoardData;
		CurStateData.CopyRangeBegin = mPrevStateData->CopyRangeBegin;
		CurStateData.CopyRangeEnd = mPrevStateData->CopyRangeEnd;

		QVariant var;
		var.setValue(QUD_HzPlotData(*mPrevStateData, CurStateData, t, act));

		UnDoCommand::CreateCommand(this, UnDoCommand::GRAPH_MODIF, 1, var);
	}

	m_HapticGraph->getCustomPlot()->replot();
	UpdatePlotHud(false);

	if(mPrevStateData)
		delete mPrevStateData;	

	mPrevStateData = NULL;
	return true;
}


bool  HapticEditorWindowHud::SelectedGraphtoClipBoard(QString MimeName, QByteArray &PrevitemData,
	 QByteArray &CopyData, int &RangeBegin, int &RangeEnd)
{
	if (HapticDeviceManager::instance()->IsRunning())
		return false;

	int CopyGrahpCount = 0;
	QSharedPointer<QByteArray> curCopyData = CopyCurrectSelectedToByteArray(RangeBegin, RangeEnd, CopyGrahpCount);
	if (curCopyData.get() == NULL)
		return false;

	QClipboard* _clipboard = QApplication::clipboard();
	const QMimeData* _mimeData = _clipboard->mimeData();
	QStringList _formats = _mimeData->formats();

	// 이전값 가져오기.
	PrevitemData = _mimeData->data(MimeName);
	
	qDebug() <<  QString("ClipBoard PrevSize = %1  CopySize = %2 GrahpCount = %3").arg(PrevitemData.length()).
		arg(curCopyData.get()->length()).
		arg(CopyGrahpCount);

	CopyData = *curCopyData.get();

	// Clip보드에 Set...
	QMimeData* hzMimeData = new QMimeData;
	hzMimeData->setData(MimeName, *curCopyData.get());
	hzMimeData->setText("Modif HzPlotData");

	_clipboard->setMimeData(hzMimeData);


	return true;
}

bool  HapticEditorWindowHud::CopySelectedGraphtoClipBoard()
{
	QByteArray PrevitemData;
	QByteArray CopyData;
	int RangeBegin;
	int RangeEnd;;

	bool bRet = SelectedGraphtoClipBoard(MIME_COPY_GRAPH, PrevitemData, CopyData, RangeBegin, RangeEnd);
	if (bRet == false)
		return false;

	// 이미 같은 값을 가지고 있다면?
	if (PrevitemData.toStdString() == CopyData.toStdString())
		return false;

	QVariant var;
	var.setValue(QUD_HzCopyPlotData(QSharedPointer<QByteArray>(new QByteArray(CopyData)),
		QSharedPointer<QByteArray>(new QByteArray(PrevitemData)), RangeBegin, RangeEnd));

	UnDoCommand::CreateCommand(this, UnDoCommand::GRAPH_COPY, 1, var);

	return true;
}

QSharedPointer<QByteArray> HapticEditorWindowHud::CopyCurrectSelectedToByteArray(int &RangeBegin, int& RangeEnd, int& GraphCount)
{
	QCPItemRect *CurRectItem = m_SelectAction->GetCurItemRect();
	if (CurRectItem == NULL)
		return NULL;

	double  StartKey = CurRectItem->topLeft->key();
	double  EndKey = CurRectItem->bottomRight->key();
	
	RangeBegin = qRound((StartKey * 1000) / 3.125f);
	RangeEnd = qRound((EndKey * 1000) / 3.125f);

	QList<HzPlotData> hzCopyData = m_HapticData->GetPlotDataRange(RangeBegin, RangeEnd);
	if (hzCopyData.count() == 0)
		return NULL;

	GraphCount = hzCopyData.length();

	//m_SamplesDataPtr = QSharedPointer < Samples >(new Samples(obj));
	QSharedPointer<QByteArray> outByteData = QSharedPointer<QByteArray>(new QByteArray);
	QDataStream outData(outByteData.get(), QIODevice::WriteOnly);
	outData << hzCopyData.size() << RangeBegin << RangeEnd;

	for (int i = 0; i < hzCopyData.size(); i++)
	{
		outData << hzCopyData[i];
	}

	return outByteData;
}

bool HapticEditorWindowHud::BeginModifGraphData(eActType act, HaticModifGraph::eModifGraphDataType t, bool bWriteClipBoardData)
{
	if (mPrevStateData != NULL)
		return false;

	QCPItemRect *CurRectItem = m_SelectAction->GetCurItemRect();
	if (CurRectItem == NULL)
		return false;

	if (!CurRectItem->visible())
		return false;

	//if (CurRectItem->topLeft->key() == 0, CurRectItem->bottomRight->key() == 0)
	//	return false;

	mPrevStateData = new UnDo_GraphStateData();

	if (bWriteClipBoardData)
	{
		QByteArray PrevitemData;
		QByteArray CopyData;
		int RangeBegin;
		int RangeEnd;;

		bool bRet = SelectedGraphtoClipBoard(MIME_COPY_GRAPH, PrevitemData, CopyData, RangeBegin, RangeEnd);
		if (bRet == false)
		{
			delete mPrevStateData;
			mPrevStateData = NULL;
			return false;
		}

		mPrevStateData->CopyClipBoardData = QSharedPointer< QByteArray>(new QByteArray(CopyData));
		mPrevStateData->PrevClipBoardData = QSharedPointer< QByteArray>(new QByteArray(PrevitemData));
		mPrevStateData->CopyRangeBegin = RangeBegin;
		mPrevStateData->CopyRangeEnd = RangeEnd;
	}

	mPrevStateData->m_PlotData = m_HapticData->Copy();
	mPrevStateData->m_SelectLeftCoord = CurRectItem->topLeft->coords();
	mPrevStateData->m_SelectRightCoord = CurRectItem->bottomRight->coords();
	mPrevStateData->m_TotTime = m_HapticData->GetTimeLength();
	mPrevStateData->m_ActType = (int)act;

	return true;
}

void HapticEditorWindowHud::horzScrollBarChanged(int value)
{
	double oldLower = ui.Plot->xAxis->range().lower;
	double oldUpper = ui.Plot->xAxis->range().upper;

	double oldRangeSize = ui.Plot->xAxis->range().size();

	QCPRange range = ui.Plot->xAxis->range();

	double newLower = (value) / 1000.0f;
	double newUpper = oldUpper;
	if (newLower + oldRangeSize < m_TimeLength / 1000)
	{
		ui.Plot->xAxis->setRange(newLower, oldRangeSize, Qt::AlignLeft);
		ui.Plot->replot();
	}

	//double newLower =;
	//ui.DebugDesc->setText(QString("horzScroll Value: %1 prevLower= %2 prevUpper= %3 / newLower = %4 ViewSize = %5").arg(value).arg(oldLower).arg(oldUpper).arg(newLower).arg(oldRangeSize));// customPlot->graph(0)->data()->size()));

}

void HapticEditorWindowHud::xAxisChanged(const QCPRange &newRange, const QCPRange &oldRange)
{
	if (newRange.lower < 0)
	{
		ui.Plot->xAxis->setRangeLower(0);
		//	ui.Plot->xAxis->setRangeUpper(oldRange.upper);
	}

	m_TimeLength = m_HapticData->GetTimeLength();

	if (newRange.upper > m_TimeLength / 1000.0f)
	{
		ui.Plot->xAxis->setRangeUpper(m_TimeLength / 1000.0f);
		//ui.Plot->xAxis->setRangeLower(oldRange.lower);
	}

	//else
	//{
	//	ui.Plot->xAxis->setRange(newRange);// setRangeUpper(newRange.upper);
	//}

	if (newRange.size() != oldRange.size())
	{
		updateScrollBarStep();
	}
}

//static float currentScrollBarValue = 0.0f;

void HapticEditorWindowHud::updateScrollBarStep()
{
	// 응?
	const QCPRange range = ui.Plot->xAxis->range();

	if (range.lower <= 0 && range.upper >= m_TimeLength / 1000.0f)
	{
		ui.horizontalScrollBar->setEnabled(false);
		//ui.horizontalScrollBar->setValue(0);
	}
	else
	{
		//ui.horizontalScrollBar->setValue(range.lower * 1000);
		ui.horizontalScrollBar->setEnabled(true);
	}

	double RangeSize = range.size();
	float n = (RangeSize * 1000) / m_TimeLength;

	//int CurValue = ui.horizontalScrollBar->value();
	ui.horizontalScrollBar->setPageStep(((m_TimeLength * n)) * 2);

	//-- 스크롤 할수 있는 나머지값.
	float s = m_TimeLength * (1 - n);

	ui.horizontalScrollBar->setRange(0, s);

	//-- tumb Size.
	ui.horizontalScrollBar->setValue((range.lower * 1000));// BarValue);



}



void HapticEditorWindowHud::yAxisChanged(const QCPRange &newRange, const QCPRange &oldRange)
{
	if (newRange.lower < -AMP_LIMIT_VALUE) {
		ui.Plot->yAxis->setRangeLower(-AMP_LIMIT_VALUE);
		ui.Plot->yAxis->setRangeUpper(oldRange.upper);
	}
	else if (newRange.upper > AMP_LIMIT_VALUE)
	{
		ui.Plot->yAxis->setRangeUpper(AMP_LIMIT_VALUE);
		ui.Plot->yAxis->setRangeLower(oldRange.lower);
	}
	else
	{
		ui.Plot->yAxis->setRange(newRange);// setRangeUpper(newRange.upper);

	}
	//	ui.Plot->yAxis->setRangeLower(0);
	//	ui->verticalScrollBar->setValue(qRound(-range.center()*100.0)); // adjust position of scroll bar slider
	//	ui->verticalScrollBar->setPageStep(qRound(range.size()*100.0)); // adjust size of scroll bar slider
}


void HapticEditorWindowHud::OnHapticDeviceEvent(int EventType, double Value)
{

}

void HapticEditorWindowHud::ShowMenu(QPoint pt)
{
	if (mPopUpMenu == NULL)
		return;

	mPopUpMenu->ShowPopUp(pt);
}
