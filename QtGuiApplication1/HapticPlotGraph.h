#pragma once

#include <QWidget>
#include "HapticPlotData.h"

class QCustomPlot;

class HapticPlotGraphCursor;
class HapticPlotGraph
{
public:	
	
	HapticPlotGraph(QCustomPlot *Plot);
	~HapticPlotGraph();


	void SetGraph(const QList<HzPlotData> &PlotData, float TimeLength);
	void UpdateGraph(const QList<HzPlotData> &hzData);


	float GetTimeLength() { return m_TimeLength;  }

	QCustomPlot*  getCustomPlot()
	{
		return m_Plot;
	}

	 static const float DefaultLerpData;


private:
	float m_TimeLength;
	float m_OldTimeLength;

	QCustomPlot *m_Plot;

	

};
