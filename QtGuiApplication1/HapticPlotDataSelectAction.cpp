#include "HapticPlotDataSelectAction.h"
//#include "qcustomplot.h"
//#include "HapticPlotData.h"
#include "HapticPlotGraph.h"
#include "HapticModifFunc.h"
#include "HapticPlotKeyAction.h"
#include "HapticEditorWindowHud.h"

#include "UndoCommands.h"
#include "UndoDataTypes.h"
#include "Device/HapticDeviceManager.h"
#include "HapticPlotGraphCursor.h"

#define ROUNDING(x, dig)	( floor((x) * pow(float(10), dig) + 0.5f) / pow(float(10), dig) )

static double Rounding(double x, int digit)
{
	return (floor((x)* pow(float(10), digit) + 0.5f) / pow(float(10), digit));
}

static int GetRoundingHzIndex(double Coord)
{
	Coord = Rounding(Coord, 6);
	int  index = (Coord * 1000.0f) / 3.125f;
	return index;
}

HapticPlotDataSelectAction::HapticPlotDataSelectAction(HapticEditorWindowHud* hud)//, UPDATE_SIGNAL UpdateFunc)
{
	//mPrevStateData = NULL;
	mParentHud = hud;

	m_Plot = hud->GetGraph();// HapticPlot;
	m_UserData = hud->GetPlotData();

	// 이거 이벤트가 먼저와야되서 먼저 초기화.
	m_Cursor = new HapticPlotGraphCursor(this);

	m_CurxRectItem = NULL;

	IsClickedRect = false;
	
	m_Plot->getCustomPlot()->setSelectionRectMode(QCP::srmNone);	
	m_Plot->getCustomPlot()->axisRect()->setRangeZoom(Qt::Orientation::Horizontal);

	m_Plot->getCustomPlot()->graph(0)->setSelectable((QCP::SelectionType::stDataRange));
	
	
	m_Plot->getCustomPlot()->setInteractions(QCP::iRangeZoom);// | QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iSelectItems);


	connect(m_Plot->getCustomPlot(), SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMoveSignal(QMouseEvent*)));
	connect(m_Plot->getCustomPlot(), SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePressSignal(QMouseEvent*)));
	connect(m_Plot->getCustomPlot(), SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(mouseReleaseSignal(QMouseEvent*)));
	connect(m_Plot->getCustomPlot(), SIGNAL(beforeReplot()), this, SLOT(beforeReplot()));
//	connect(m_Plot->getCustomPlot(), SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
}

HapticPlotDataSelectAction::~HapticPlotDataSelectAction()
{
	delete m_Cursor;
	m_Cursor = NULL;

	//if (mPrevStateData)
	//	delete mPrevStateData;

//	if (m_PrevPlotData != NULL)
//		delete m_PrevPlotData;
//
////	if (m_CurxRectItem != NULL)
//	//	delete m_CurxRectItem;
//
//	//m_CurxRectItem = NULL;
//	m_PrevPlotData = NULL;
	//mPrevStateData = NULL;
}


void HapticPlotDataSelectAction::SetActiveSelectionRectMode(bool bActive)
{
	if (m_Plot == NULL)
		return;

	QCP::SelectionRectMode curMode = m_Plot->getCustomPlot()->selectionRectMode();

	//qDebug() << "Selected Items = " << m_Plot->getCustomPlot()->selectedItems().length();

	if (bActive)
	{
		if (curMode == QCP::srmSelect)
			return;

		m_Plot->getCustomPlot()->setSelectionRectMode(QCP::srmSelect);
	}
	else
	{
		if (curMode == QCP::srmNone)
			return;

		m_Plot->getCustomPlot()->setSelectionRectMode(QCP::srmNone);
	}

	m_Plot->getCustomPlot()->replot();
}

void HapticPlotDataSelectAction::keyReleaseEvent(QKeyEvent *e)
{
	if (m_Plot == NULL || m_UserData == NULL)
		return;

	if (m_CurxRectItem == NULL)
		return;

	if (HT::HapticDeviceManager::instance()->IsRunning())
		return;

	QCustomPlot* plot = m_Plot->getCustomPlot();
	if (plot == NULL)
		return;

	if (m_CurxRectItem->topLeft->key() == 0, m_CurxRectItem->bottomRight->key() == 0)
		return;

	bool bRet = false;	
	QList<HzPlotData> oldData;

	HaticModifGraph::eModifGraphDataType modifType;
	switch ((Qt::Key) e->key())
	{
	case  Qt::Key::Key_Left:
	{
		modifType = HaticModifGraph::eModifGraphDataType::HZ_UP;
		bRet = mParentHud->UpdateModifGraphData(HapticEditorWindowHud::KEYBOARD, modifType);
		break;
	}
	case  Qt::Key::Key_Right:
	{
		modifType = HaticModifGraph::eModifGraphDataType::HZ_DOWN;
		bRet =  mParentHud->UpdateModifGraphData(HapticEditorWindowHud::KEYBOARD, modifType);
		break;
	}
	case  Qt::Key::Key_Up:
	{
		bRet = true;
		modifType = HaticModifGraph::eModifGraphDataType::AMP_UP;
	}
	break;
	case  Qt::Key::Key_Down:
	{
		bRet = true;
		modifType = HaticModifGraph::eModifGraphDataType::AMP_DOWN;
	}
	break;
	default:
		return;
	}

	mParentHud->EndModifGraphData(HapticEditorWindowHud::KEYBOARD, modifType, bRet);
}

void HapticPlotDataSelectAction::keyPressEvent(QKeyEvent *e)
{
//	if (e->isAutoRepeat())
//		return;

	if (m_Plot == NULL || m_UserData == NULL)
		return;

	if (m_CurxRectItem == NULL)
		return;

	if (HT::HapticDeviceManager::instance()->IsRunning())
		return;

	QCustomPlot* plot = m_Plot->getCustomPlot();
	if (plot == NULL)
		return;

	if (m_CurxRectItem->topLeft->key() == 0, m_CurxRectItem->bottomRight->key() == 0)
		return;

	bool bRet = false;
	try
	{
		Qt::Key PressKey = (Qt::Key) e->key();

		switch ((Qt::Key) e->key())
		{
		case  Qt::Key::Key_Up:
		{
			if (!e->isAutoRepeat())
				mParentHud->BeginModifGraphData(HapticEditorWindowHud::KEYBOARD, HaticModifGraph::eModifGraphDataType::AMP_UP);
	
			mParentHud->UpdateModifGraphData(HapticEditorWindowHud::KEYBOARD, HaticModifGraph::eModifGraphDataType::AMP_UP);
			break;
		}
		case  Qt::Key::Key_Down:
		{
			if (!e->isAutoRepeat())
				mParentHud->BeginModifGraphData(HapticEditorWindowHud::KEYBOARD, HaticModifGraph::eModifGraphDataType::AMP_DOWN);

			mParentHud->UpdateModifGraphData(HapticEditorWindowHud::KEYBOARD, HaticModifGraph::eModifGraphDataType::AMP_DOWN);
			break;
		}
		case  Qt::Key::Key_Left:
		{
			if (!e->isAutoRepeat())
				mParentHud->BeginModifGraphData(HapticEditorWindowHud::KEYBOARD, HaticModifGraph::eModifGraphDataType::HZ_UP);

			break;
		}
		case  Qt::Key::Key_Right:
		{
			if (!e->isAutoRepeat())
				mParentHud->BeginModifGraphData(HapticEditorWindowHud::KEYBOARD, HaticModifGraph::eModifGraphDataType::HZ_DOWN);

			break;
		}

		default:
			return;
		}
	}
	catch (EXCEPINFO)
	{
		//-- 복구.
	}	
}

void HapticPlotDataSelectAction::mouseMoveSignal(QMouseEvent *event)
{
	if (HT::HapticDeviceManager::instance()->IsRunning())
		return;
	HapticPlotKeyAction::instance()->mouseMove(event, mParentHud);
}

void HapticPlotDataSelectAction::mousePressSignal(QMouseEvent *event)
{
	if (HT::HapticDeviceManager::instance()->IsRunning())
		return;

	HapticPlotKeyAction::instance()->mousePress(event, mParentHud);
}

void HapticPlotDataSelectAction::mouseReleaseSignal(QMouseEvent *event)
{
	if (!HT::HapticDeviceManager::instance()->IsRunning())
	{
		HapticPlotKeyAction::instance()->mouseRelease(event, mParentHud);
	}

	Q_UNUSED(event)
}

void HapticPlotDataSelectAction::UpdateCursor(QCPItemRect* RectRangeItem)
{
	if (m_Cursor == NULL)
		return;

	m_Cursor->UpdateCursor(RectRangeItem);
}


QCPDataRange HapticPlotDataSelectAction::CoordToRange(double StartCoord, double EndCoord, HapticPlotData::eSerchDir StartCoordSerchDir,
	HapticPlotData::eSerchDir EndCoordSerchDir)
{
	int hzIndex1 = (StartCoord * 1000) / 3.125f;
	int hzIndex2 = (EndCoord * 1000) / 3.125f;

	QPoint hzGraphIndex =
		m_UserData->GetHzDataIndex(hzIndex1, hzIndex2, 1, StartCoordSerchDir, EndCoordSerchDir);
	
	if (hzGraphIndex.x() == -1 || hzGraphIndex.y() == -1)
		return QCPDataRange(0,0);

	const QList<HzPlotData> PloHztData = m_UserData->GetHzPlotData();
	
	int begiDataIndex = PloHztData[hzGraphIndex.x()].lowerIndex;
	int endDataIndex  = PloHztData[hzGraphIndex.y()].upperIndex;// +1;	

	QCPDataRange range((int)HapticPlotGraph::DefaultLerpData * begiDataIndex, (int)HapticPlotGraph::DefaultLerpData * endDataIndex);

	return range;
}

void HapticPlotDataSelectAction::ClearItems()
{
	if (m_CurxRectItem == NULL)
		return;

	if (m_Cursor)
		m_Cursor->Active(false);

	m_Plot->getCustomPlot()->removeItem(m_CurxRectItem);
	m_CurxRectItem = NULL;

}



bool HapticPlotDataSelectAction::CreateSelectItemRect(QCPDataRange range)
{
	//QCPDataSelection selection;
	//selection.addDataRange(range);		
	//m_Plot->getCustomPlot()->graph(0)->setSelection(selection);
	if (m_CurxRectItem)
	{
		m_Plot->getCustomPlot()->removeItem(m_CurxRectItem);
		m_CurxRectItem = NULL;
	}

	m_CurxRectItem = new QCPItemRect(m_Plot->getCustomPlot());		
	m_CurxRectItem->setLayer("grid");
	m_CurxRectItem->setVisible(true);
	m_CurxRectItem->setPen(QPen(Qt::transparent));		
	m_CurxRectItem->setBrush(QBrush(QColor(255, 0, 0, 128)));// Qt::lightGray));
	m_CurxRectItem->topLeft->setType(QCPItemPosition::ptPlotCoords);		
	
	int StartIndex = (float)range.begin() / HapticPlotGraph::DefaultLerpData;
	int EndIndex = (float)range.end() / HapticPlotGraph::DefaultLerpData;

	//float DefaultDeltaLerpKey = (3.125f / 1000.0f) / (HapticPlotGraph::DefaultLerpData);
	double StartCoord = 3.125f * (StartIndex) / 1000.0f;
	double EndCoord = Rounding( 3.125f * (EndIndex)  / 1000.0f , 6);
//
	m_CurxRectItem->topLeft->setCoords(StartCoord, AMP_LIMIT_VALUE);
	m_CurxRectItem->bottomRight->setType(QCPItemPosition::ptPlotCoords);
	m_CurxRectItem->bottomRight->setCoords(EndCoord, -AMP_LIMIT_VALUE);
//		
	m_CurxRectItem->setClipAxisRect(m_Plot->getCustomPlot()->axisRect());
	m_CurxRectItem->setClipToAxisRect(true);
	m_CurxRectItem->setSelectable(false);

	return true;
}


//bool HapticPlotDataSelectAction::CreateSelectItemRect(QPointF Coord)
//{
//	////QCPDataSelection selection;
//	////selection.addDataRange(range);		
//	////m_Plot->getCustomPlot()->graph(0)->setSelection(selection);
//	//if (m_CurxRectItem)
//	//{
//	//	m_Plot->getCustomPlot()->removeItem(m_CurxRectItem);
//	//	m_CurxRectItem = NULL;
//	//}
//
//	//m_CurxRectItem = new QCPItemRect(m_Plot->getCustomPlot());
//	//m_CurxRectItem->setLayer("grid");
//	//m_CurxRectItem->setVisible(true);
//	//m_CurxRectItem->setPen(QPen(Qt::transparent));
//	//m_CurxRectItem->setBrush(QBrush(QColor(255, 0, 0, 128)));// Qt::lightGray));
//	//m_CurxRectItem->topLeft->setType(QCPItemPosition::ptPlotCoords);
//
//	////float DefaultDeltaLerpKey = (3.125f / 1000.0f) / (HapticPlotGraph::DefaultLerpData);
//	//double StartCoord = (3.125f * (range.begin() / HapticPlotGraph::DefaultLerpData)) / 1000;
//	//double EndCoord = (3.125f * (range.end() / HapticPlotGraph::DefaultLerpData)) / 1000;
//	////
//	//m_CurxRectItem->topLeft->setCoords(StartCoord, AMP_LIMIT_VALUE);
//	//m_CurxRectItem->bottomRight->setType(QCPItemPosition::ptPlotCoords);
//	//m_CurxRectItem->bottomRight->setCoords(EndCoord, -AMP_LIMIT_VALUE);
//	////		
//	//m_CurxRectItem->setClipAxisRect(m_Plot->getCustomPlot()->axisRect());
//	//m_CurxRectItem->setClipToAxisRect(true);
//	//m_CurxRectItem->setSelectable(false);
//
//	return true;
//}
//
bool HapticPlotDataSelectAction::CreateSelectItemRectToCoord(QPointF leftCoord, QPointF RightCoord)
{
	if (m_CurxRectItem)
	{
		m_Plot->getCustomPlot()->removeItem(m_CurxRectItem);
		m_CurxRectItem = NULL;
	}

	m_CurxRectItem = new QCPItemRect(m_Plot->getCustomPlot());
	m_CurxRectItem->setLayer("grid");
	m_CurxRectItem->setVisible(true);
	m_CurxRectItem->setPen(QPen(Qt::transparent));
	m_CurxRectItem->setBrush(QBrush(QColor(255, 0, 0, 128)));// Qt::lightGray));
	m_CurxRectItem->topLeft->setType(QCPItemPosition::ptPlotCoords);
	
	m_CurxRectItem->topLeft->setCoords(leftCoord);// , 128);
	m_CurxRectItem->bottomRight->setType(QCPItemPosition::ptPlotCoords);
	m_CurxRectItem->bottomRight->setCoords(RightCoord);// , -128);
	
	m_CurxRectItem->setClipAxisRect(m_Plot->getCustomPlot()->axisRect());
	m_CurxRectItem->setClipToAxisRect(true);

	return true;
}

QCPDataRange HapticPlotDataSelectAction::MergeRange(QCPDataRange Item1, QCPDataRange Item2)
{
	QCPDataRange RetDataRange(0, 0);
	//if (Item2.intersects(Item1))
	//{
		int beginMin = qMin(Item1.begin(), Item2.begin());
		int endMax = qMax(Item1.end(), Item2.end());

		RetDataRange.setBegin(beginMin);
		RetDataRange.setEnd(endMax);
//	}
//	else
//	{
//		RetDataRange = Item1;	
//	}

	return RetDataRange;
}

QPoint HapticPlotDataSelectAction::GetCurretSelectHzGraphIndex()
{    
	return GetCurretSelectHzGraphIndex(m_CurxRectItem);
}




QPoint HapticPlotDataSelectAction::GetCurretSelectHzGraphIndex(QCPItemRect* RectItem)
{
	if (RectItem == NULL)
		return QPoint(-1, -1);

	QCustomPlot* plot = m_Plot->getCustomPlot();
	QCPRange Range = plot->xAxis->range();

	double EndCoord = Rounding( RectItem->bottomRight->coords().x(), 6);

	int hzIndex1 = GetRoundingHzIndex(RectItem->topLeft->coords().x());
	int hzIndex2 = GetRoundingHzIndex(RectItem->bottomRight->coords().x());

	QPoint hzGraphIndex = QPoint(-1, -1);
	hzGraphIndex = m_UserData->GetHzDataIndex(hzIndex1, hzIndex2, 1, HapticPlotData::eSerchDir::LOWER, HapticPlotData::eSerchDir::LOWER);

	if (EndCoord >= Range.upper)
		hzGraphIndex.setY(m_UserData->GetHzPlotData().size());

	return  hzGraphIndex;
}


void HapticPlotDataSelectAction::UpdateSelection(double StartCoord, double EndCoord, int PressKey, HapticPlotData::eSerchDir StartCoordSerchDir,
	HapticPlotData::eSerchDir EndCoordSerchDir, int SelectAct )
{
	if (m_Plot == NULL)
		return;

	QCustomPlot* plot = m_Plot->getCustomPlot();
	if (plot == NULL)
		return;

	QCPDataRange SelectRange = CoordToRange(qMin(StartCoord, EndCoord), qMax(StartCoord, EndCoord), StartCoordSerchDir, EndCoordSerchDir);

	if (PressKey == Qt::Key::Key_Shift)
	{
		// MergeSelect..
		SelectRange = MergeRange(SelectRange, m_CurSelectRange);
	}
	else
	{
		//-- Single Select
	}

	// 
	qDebug() << QString("SelectRange = %1, %2").arg(SelectRange.begin()).arg(SelectRange.end());

	if (SelectRange == m_CurSelectRange)
		return;

	/////////
	CreateSelectItemRect(SelectRange);
	plot->replot();
	
	UpdateCursor(m_CurxRectItem);

	double OldStartCoord = (3.125f * (m_CurSelectRange.begin() / HapticPlotGraph::DefaultLerpData)) / 1000;
	double OldEndCoord = (3.125f * (m_CurSelectRange.end() / HapticPlotGraph::DefaultLerpData)) / 1000;

	double NewStartCoord = (3.125f * (SelectRange.begin() / HapticPlotGraph::DefaultLerpData)) / 1000;
	double NewEndCoord = (3.125f * (SelectRange.end() / HapticPlotGraph::DefaultLerpData)) / 1000;

	QPointF OldCoord(OldStartCoord, OldEndCoord);
	QPointF NewCoord(NewStartCoord, NewEndCoord);

	QVariant var;
	var.setValue(QUD_DataCoord(OldCoord, NewCoord, SelectAct));
	UnDoCommand::CreateCommand(mParentHud, UnDoCommand::SELECT_GRAPH, 1, var);
	
	m_CurSelectRange = SelectRange;
}


void HapticPlotDataSelectAction::UpdateSelectionFromCursor(QPointF PrevCursorCoord, QPointF NowCursorCoord)
{
	if (m_CurxRectItem == NULL)
		return;

	if (m_Plot == NULL)
		return;

	QCustomPlot* plot = m_Plot->getCustomPlot();
	if (plot == NULL)
		return;

	if (m_CurxRectItem->topLeft->coords().x() == NowCursorCoord.x() &&
		m_CurxRectItem->bottomRight->coords().x() == NowCursorCoord.y())
	{
		plot->replot();
		return;
	}

	m_CurxRectItem->topLeft->setCoords(NowCursorCoord.x(), AMP_LIMIT_VALUE);
	m_CurxRectItem->bottomRight->setCoords(NowCursorCoord.y(), -AMP_LIMIT_VALUE);

	if (NowCursorCoord.x() == NowCursorCoord.y())
	{
		int hzIndex = (NowCursorCoord.x() * 1000) / 3.125f;
		m_CurSelectRange = QCPDataRange(hzIndex * HapticPlotGraph::DefaultLerpData, hzIndex *HapticPlotGraph::DefaultLerpData);
	}
	else
	{
		// 이것도 같이 갱신.
		m_CurSelectRange = CoordToRange(NowCursorCoord.x(), NowCursorCoord.y(), HapticPlotData::eSerchDir::LOWER, HapticPlotData::eSerchDir::UPPPER);
	}

	qDebug() << QString("SelectRangeFromCursor = %1, %2").arg(m_CurSelectRange.begin()).arg(m_CurSelectRange.end());

	plot->replot();

	QVariant var;
	var.setValue(QUD_DataCoord(PrevCursorCoord, NowCursorCoord,
		HapticEditorWindowHud::eSelectActType::_CURSOR_));

	UnDoCommand::CreateCommand(GetParentHud(), UnDoCommand::SELECT_GRAPH, 1, var);

	mParentHud->UpdateHelpDesc();
}

void HapticPlotDataSelectAction::UpdateSelection(QPoint ClickPos, QPoint ReleasePos, int PressKey)
{
	if (m_Plot == NULL)
		return;

	QCustomPlot* plot = m_Plot->getCustomPlot();
	if (plot == NULL)
		return;

	double StartCoord = plot->xAxis->pixelToCoord(ClickPos.x());
	double EndCoord = plot->xAxis->pixelToCoord(ReleasePos.x());

	UpdateSelection(StartCoord, EndCoord, PressKey, HapticPlotData::eSerchDir::LOWER, HapticPlotData::eSerchDir::UPPPER,
		HapticEditorWindowHud::eSelectActType::_MOUSE_);

}

void HapticPlotDataSelectAction::beforeReplot()
{
	// this is to prevent the legend from stretching if the plot is stretched.
	// Since we've set the inset placement to be ipFree, the width/height of the legend
	// is also defined in axisRect coordinates (0..1) and thus would stretch.
	// This is due to change in a future release (probably QCP 2.0) since it's basically a design mistake.
	//m_Plot->legend->setMaximumSize(plot->legend->minimumSizeHint());
}


