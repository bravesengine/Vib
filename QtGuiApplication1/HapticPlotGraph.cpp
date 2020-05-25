#include "HapticPlotGraph.h"
#include "qcustomplot.h"
#include "HapticPlotGraphCursor.h"
static float StartZoom = 1.0f;

// 하나의 파장? 을 그릴때 30개식 Lerp해서 Draw..
const float HapticPlotGraph::DefaultLerpData=30;


HapticPlotGraph::HapticPlotGraph(QCustomPlot *Plot)
{

	m_TimeLength = m_OldTimeLength = 0;

	m_Plot = Plot;



	QCustomPlot *customPlot = m_Plot;
	
	
	QPen Pen2(QColor(40, 110, 255));
	Pen2.setWidth(1);


	customPlot->setBackground(QBrush(QColor(210, 210, 210, 255), Qt::SolidPattern));// painter->pen().color(), Qt::SolidPattern);)

	customPlot->addGraph(); // blue line
	customPlot->graph(0)->setPen(Pen2);

	QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
	timeTicker->setTimeFormat("%s:%z");
	customPlot->xAxis->setTicker(timeTicker);
	

	customPlot->graph()->setLineStyle(QCPGraph::lsLine);
	customPlot->xAxis->grid()->setSubGridVisible(false);
	customPlot->yAxis->grid()->setSubGridVisible(false);
	customPlot->xAxis->grid()->setVisible(false);
	customPlot->yAxis->grid()->setVisible(false);


	//  왼쪽 TickLne 감춤.
	//customPlot->yAxis->setTicks(false);// false);

	//  방향
	//customPlot->yAxis->setti
	//customPlot->yAxis->axisType
	
	//customPlot->yAxis->setTickLineHide(false);// false);
	//customPlot->yAxis->setSubTickLineHide(true);// false);

	QSharedPointer<QCPAxisTickerText> Ticker(new QCPAxisTickerText);

	Ticker->addTick(255, "255");
	Ticker->addTick(0, "0");
	Ticker->addTick(-255, "-255");
	
	//customPlot->xAxis->setTicker(textTicker);

	//Ticker->setTickStepStrategy()
	customPlot->yAxis->setTicker(Ticker);

	customPlot->yAxis->setTickLabelColor(Qt::red);
	
	customPlot->yAxis->setTickPen(QPen(Qt::yellow));

	//customPlot->yAxis->setTickLabelFont(QFont)
//	customPlot->yAxis->setTickLengthIn(-5);
//	customPlot->yAxis->setTickLengthOut(5);
	//customPlot->yAxis->setTickLengthOut(5);

	customPlot->yAxis->setTickLength(5, 3);

//	customPlot->yAxis->ticker->axisType = QCPAxis::AxisType::atLeft;

	customPlot->xAxis->setTickLabels(true);
	customPlot->yAxis->setTickLabels(true);


	// 아래쪽 TickLine 감춤.
	customPlot->xAxis->setTickLineHide(true);// false);
	customPlot->xAxis->setSubTickLineHide(true);// false);


	//-- 위쪽 TickLine 감추기
	customPlot->xAxis2->setTickLineHide(true);// false);
	customPlot->xAxis2->setSubTickLineHide(true);// false);

	customPlot->yAxis2->setTickLineHide(true);// false);
	customPlot->yAxis2->setSubTickLineHide(true);// false);


	customPlot->xAxis->setTickPen(QPen(QColor(255, 255, 255), 1));
	customPlot->xAxis->setSubTickPen(QPen(QColor(255, 255, 255), 1));

//	customPlot->xAxis->setSubTicks(false);	



	//-- 가운데 선 그리기.
	QCPItemLine *tickHLine = new QCPItemLine(customPlot);	
	tickHLine->start->setCoords(0, 0);
	tickHLine->end->setCoords(100, 0);	
	//tickHLine->setPen(QPen(QColor(137, 140, 140), 1));
	tickHLine->setPen(QPen(QColor(137, 140, 140), 1));
	tickHLine->setObjectName("center Line");
	tickHLine->setSelectable(false);


	//gridsetSubTicks(false);
	//customPlot->yAxis->setSubTicks(false);


	// 위아래 Margin.. 300 - AMP_LIMIT_VALUE..
	customPlot->yAxis->setRange(-300, 300);// AMP_LIMIT_VALUE, AMP_LIMIT_VALUE);


	customPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
	//	customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
	//	customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
		// Set BackGroud Color




	QLinearGradient axisRectGradient;
	axisRectGradient.setStart(0, 0);
	axisRectGradient.setFinalStop(0, 350);
	axisRectGradient.setColorAt(0, QColor(80, 80, 80));
	axisRectGradient.setColorAt(1, QColor(30, 30, 30));
	customPlot->axisRect()->setBackground(axisRectGradient);

	customPlot->rescaleAxes();
	customPlot->axisRect()->setupFullAxesBox(true);
	//this->ui.customPlot->replot();


	
	//////////////////
}

HapticPlotGraph::~HapticPlotGraph()
{
}

void HapticPlotGraph::SetGraph(const QList<HzPlotData> &PlotData, float TimeLength)
{

	QCustomPlot *customPlot = m_Plot;

	m_OldTimeLength = m_TimeLength;
	m_TimeLength = TimeLength;

	double oldlastKey = 0;
	QSharedPointer<QCPGraphDataContainer> dataContainer = customPlot->graph(0)->data();
	if (!dataContainer->isEmpty())
	{
		QCPGraphDataContainer::iterator it = dataContainer->end() - 1;
		oldlastKey = it->key;
	}

	float oldViewRangeLower = customPlot->xAxis->range().lower;
	float oldViewRangeUppper = customPlot->xAxis->range().upper;
	float ViewRangelength = customPlot->xAxis->range().upper - customPlot->xAxis->range().lower;

	//-- Clear..
	customPlot->graph(0)->data()->clear();

	if (PlotData.size() == 0)
	{
		customPlot->xAxis->setRange(0, 0, Qt::AlignLeft);
		return;
	}

	//-- 최대 햅틱 버퍼 256개, 3.125 ms  최대 (320hz)
	float DeltaKey = ((800 / 256.0f));
	//	int  TotalDataCount = (m_TimeLength / DeltaKey) * DefaultLerpData;
	int TotalDataArrayCount = m_TimeLength / DeltaKey;
	float DefaultDeltaLerpKey = (DeltaKey / 1000.0f) / (DefaultLerpData);

	double CutKey = 0;
	for (int i = 0; i < PlotData.size(); i++)
	{
		int LerpCout = (PlotData[i].upperIndex - PlotData[i].lowerIndex);// +1;
		int TotLerpCount = DefaultLerpData * LerpCout;

		float Degress = 360.0f / TotLerpCount;

		for (int j = 0; j < TotLerpCount; j++)
		{
			float yData = (qSin(qDegreesToRadians(Degress * j))) * PlotData[i].HapticRawData;

			customPlot->graph(0)->addData(CutKey, yData);

			CutKey += DefaultDeltaLerpKey;
		}
	}

	//-- last data..
	//CutKey += DefaultDeltaLerpKey;
	customPlot->graph(0)->addData(CutKey, 0);

	if(oldlastKey == 0)
	{
		customPlot->xAxis->setRange(0, (m_TimeLength / 1000.0f), Qt::AlignLeft);
	}
	else if (CutKey < oldViewRangeUppper)
	{		
		qDebug() << "HapticPlotGraph::SetGraph() Over View??";
		customPlot->xAxis->setRange(oldViewRangeLower, (m_TimeLength / 1000.0f), Qt::AlignLeft);
	}

	//	//QSharedPointer<QCPGraphDataContainer> dataContainer = customPlot->graph(0)->data();
	//	//QCPGraphDataContainer::iterator it = dataContainer->end() - 1;
	//	//double newlastKey = it->key;
	//	////customPlot->xAxis->setRange(oldViewRangeLower,   , Qt::AlignLeft);
	//	//qDebug() << "lower = " << customPlot->xAxis->range().lower << "uppper = " << customPlot->xAxis->range().upper << "last Key = " << lastKey;	
	//	//qDebug() << "ViewRatio = " << ViewRatio;


	// TEST..
	//Active(true);
}

void HapticPlotGraph::UpdateGraph(const QList<HzPlotData> &hzData)
{
	QCustomPlot* plot = m_Plot;
	if (plot == NULL)
		return;

	QSharedPointer <QCPGraphDataContainer> GraphData = plot->graph(0)->data();
	QCPGraphDataContainer* Container = GraphData.get();

	for (int i = 0; i < hzData.size(); i++)
	{
		int LerpCout = (hzData[i].upperIndex - hzData[i].lowerIndex);
		int TotLerpCount = 30 * LerpCout;

		float Degress = 360.0f / TotLerpCount;
		int ContaterIndex = hzData[i].lowerIndex * 30;

		for (int j = 0; j < TotLerpCount; j++)
		{
			float yData = (qSin(qDegreesToRadians(Degress * j))) * hzData[i].HapticRawData;

			QCPGraphData* data = (QCPGraphData*)Container->at(ContaterIndex + j);

			data->value = yData;
		}
	}
}