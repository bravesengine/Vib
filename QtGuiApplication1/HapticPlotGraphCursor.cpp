#include "HapticPlotGraphCursor.h"
#include "qcustomplot.h"
#include "qcustomplot_TracerExt.h"

#include "HapticPlotDataSelectAction.h"
#include "HapticPlotGraph.h"
#include "UndoDataTypes.h"
#include "HapticEditorWindowHud.h"


void HapticPlotGraphCursor::CursorGeomety::Initize(QString Name, QString LayerName, QCustomPlot *customPlot)
{
	//QCPItemTracerExt *CursorItem = NULL;
	//QCPItemLine *CursorLine = NULL;

	CursorLine = new QCPItemLine(customPlot);
	CursorLine->start->setCoords(-1 , 300);
	CursorLine->end->setCoords(-1, -300);
	CursorLine->setLayer(LayerName);
	CursorLine->setPen(QPen(QColor(0, 255, 0)));
	CursorLine->setObjectName(Name + "Line Item..");
	CursorLine->setSelectable(false);
	CursorLine->setVisible(false);

	////////////////////////////////////////////////
	CursorItem = new QCPItemTracerExt(customPlot);
	CursorItem->setGraph(customPlot->graph(0));

	CursorItem->setGraphKey(0);
	CursorItem->setInterpolating(true);
	//  None으로하면 Draw안함.
	CursorItem->setStyle(QCPItemTracer::tsCircle);

	CursorItem->setPen(QPen(Qt::yellow));
	CursorItem->setBrush(Qt::red);
	CursorItem->setSize(10);
	CursorItem->setLayer(LayerName);
	CursorItem->setObjectName(Name + "TracerExt Item..");
	CursorItem->setSelectable(true);
	CursorItem->setVisible(false);
}

void HapticPlotGraphCursor::CursorGeomety::Move(double Coord)
{
	CursorItem->setGraphKey(Coord);

	CursorLine->start->setCoords(Coord, 300);
	CursorLine->end->setCoords(Coord, -300);
}

void HapticPlotGraphCursor::CursorGeomety::Active(bool bActive)
{
	CursorItem->setVisible(bActive);
	CursorLine->setVisible(bActive);
}


void HapticPlotGraphCursor::CursorGeomety::CaptureCoord()
{
	mPrevCoord = CursorItem->graphKey();
}




HapticPlotGraphCursor::HapticPlotGraphCursor(HapticPlotDataSelectAction *SelectAction)
{
	//QDir dir(".");
	//QImage q(dir.absolutePath() + "/Resources/" + "Circle.png");// "Play.png");
	//QPixmap pixmap = QPixmap::fromImage(q);

	mSelectAction = SelectAction;
	m_Plot = mSelectAction->getGraph()->getCustomPlot();
	QCustomPlot *customPlot = m_Plot;	

	// mask  인가??? 
	//customPlot->addLayer("TopLayer", customPlot->layer("main"), QCustomPlot::limBelow);
	//customPlot->addLayer("TopLayer", customPlot->layer("main"), QCustomPlot::limAbove);

	// 마지막? Layer끝에 추가.
	customPlot->addLayer("CursorLayer");
	mLCursor = new CursorGeomety();
	mLCursor->Initize("LEFT", "CursorLayer", m_Plot);

	mRCursor = new CursorGeomety();
	mRCursor->Initize("RIGHT","CursorLayer", m_Plot);

//	connect(mCursor1, SIGNAL(itemClick(QCPAbstractItem*, QMouseEvent*)), this, SLOT(mousePressItem(QCPAbstractItem*, QMouseEvent*)));
	connect(customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePressSignal(QMouseEvent*)));
	connect(customPlot, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(mouseReleaseSignal(QMouseEvent*)));
	connect(customPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMoveSignal(QMouseEvent*)));


}

HapticPlotGraphCursor::~HapticPlotGraphCursor()
{
	delete mLCursor;
	delete mRCursor;

	mLCursor = NULL;
	mRCursor = NULL;
}

void HapticPlotGraphCursor::mousePressItem(QCPAbstractItem* item, QMouseEvent* event)
{
	//	//qDebug() << "Click Item = " << item->objectName();
}


// 그래프 단위로 선택 이동.
int HapticPlotGraphCursor::ResetPositionToSnap()
{
	////-- 영역을 벗어났는지 체크.
//if (!m_Plot->axisRect()->rect().contains(event->pos()))
//{	
//	qDebug() << "OUT!!";
//}


	if (mClickedCursor == NULL || m_Plot == NULL || mSelectAction == NULL)
		return -1;

	double Coord =  mClickedCursor->CursorItem->graphKey();
	
	//double Coord = m_Plot->xAxis->pixelToCoord(pt.x());
	QCPDataRange Range = mSelectAction->CoordToRange(Coord, Coord, HapticPlotData::eSerchDir::LOWER, HapticPlotData::eSerchDir::LOWER);

	double LCoord = (3.125f * (Range.begin() / HapticPlotGraph::DefaultLerpData)) / 1000;
	double RCoord = (3.125f * (Range.end() / HapticPlotGraph::DefaultLerpData)) / 1000;

	// 가까운 곳으로 이동.
	//LCoord - Coord;
	QVector2D Center = QVector2D(Coord, 0);
	QVector2D LVec = QVector2D(LCoord, 0);
	QVector2D RVec = QVector2D(RCoord, 0);

	float Llengh = (Center - LVec).length();
	float Rlength = (Center - RVec).length();

	int Snap = 0;

	if (Llengh < Rlength)
	{
		Snap = 0;
		UpdateCursor(mClickedCursor, LCoord);
	}
	else
	{
		Snap = 1;
		UpdateCursor(mClickedCursor, RCoord);
	}

	return Snap;
}

void HapticPlotGraphCursor::mouseReleaseSignal(QMouseEvent* event)
{
	if (mClickedCursor == NULL)
		return;

	double LOldCoord = mLCursor->GetPrevCoord();
	double ROldCoord = mRCursor->GetPrevCoord();

	int Snap = ResetPositionToSnap();// event->pos());

	double LNewCoord = mLCursor->CursorItem->graphKey();
	double RNewCoord = mRCursor->CursorItem->graphKey();
	
	//qDebug() << "LCoord = " << LNewCoord << "RCoord = " << RNewCoord;

	mSelectAction->UpdateSelectionFromCursor(QPointF(LOldCoord, ROldCoord), QPointF(LNewCoord, RNewCoord));
	mClickedCursor = NULL;
}

void HapticPlotGraphCursor::mouseMoveSignal(QMouseEvent* event)
{
	if (mClickedCursor == NULL)
		return;

	double Coord = m_Plot->xAxis->pixelToCoord(event->pos().x());	

	if (Coord > m_Plot->xAxis->range().upper)
		Coord = m_Plot->xAxis->range().upper;
	else if (Coord < m_Plot->xAxis->range().lower)
		Coord = m_Plot->xAxis->range().lower;

	qDebug() << "Move Coord = " << Coord;
	mClickedCursor->Move(Coord);

	m_Plot->replot();	
}

void  HapticPlotGraphCursor::mousePressSignal(QMouseEvent* event)
{
	qDebug() << "Cursor mousePressSignal!!";
	//double len = mCursor1->selectTest(event->pos(), true);		
	QCPAbstractItem* itm = m_Plot->itemAt(event->pos(), true);

	if (itm && itm == mLCursor->CursorItem)
	{
		mClickedCursor = mLCursor;
	}
	else if (itm && itm == mRCursor->CursorItem)
	{
		mClickedCursor = mRCursor;
	}
	else
		mClickedCursor = NULL;

	if (mClickedCursor)
	{
		// 이전 값저장..
		mLCursor->CaptureCoord();
		mRCursor->CaptureCoord();
	}
}

void HapticPlotGraphCursor::Active(bool bActive)
{
	if(mLCursor) mLCursor->Active(bActive);
	if (mRCursor) mRCursor->Active(bActive);
}


void HapticPlotGraphCursor::UpdateCursor(QCPItemRect* RectRangeItem)
{
	if (RectRangeItem == NULL)
		return;

	QPointF LCoord = RectRangeItem->topLeft->coords();
	QPointF RCoord = RectRangeItem->bottomRight->coords();

	mLCursor->Move(LCoord.x());
	mRCursor->Move(RCoord.x());

	Active(TRUE);

	m_Plot->replot();
}

void HapticPlotGraphCursor::UpdateCursor(CursorGeomety* Cursor, double CursorCoord)
{
	if (Cursor == NULL || m_Plot == NULL)
		return;
	
	Cursor->Move(CursorCoord);
	//	m_Plot->replot();
}


//void HapticPlotGraphCursor::UpdateLCursor(double CursorCoord)
//{
//	if (mLCursor == NULL)
//		return;
//
//	UpdateCursor(mLCursor, CursorCoord);
//}
//
//void HapticPlotGraphCursor::UpdateRCursor(double CursorCoord)
//{
//	if (mRCursor == NULL)
//		return;
//
//	UpdateCursor(mRCursor, CursorCoord);
//}
//
//void HapticPlotGraphCursor::UpdateCursor(double LCursorCoord, double RCursorCoord)
//{
//	UpdateLCursor(LCursorCoord);
//	UpdateRCursor(RCursorCoord);
//}
//


//
//void HapticPlotGraphCursor::UpdateCursor(QCPDataRange Range)
//{
//	double LCoord = (3.125f * (Range.begin() / HapticPlotGraph::DefaultLerpData)) / 1000;
//	double RCoord = (3.125f * (Range.end() / HapticPlotGraph::DefaultLerpData)) / 1000;
//
//	UpdateCursor(LCoord, RCoord);
//}
