#pragma once

#include <QWidget>
#include "qcustomplot.h"
//class QCustomPlot;
#include "HapticPlotData.h"
//#include "UndoCommands.h"

//class HapticPlotData;
class HapticPlotGraph;
class HapticEditorWindowHud;
class HapticPlotGraphCursor;

class HapticPlotDataSelectAction : public QObject
{
	Q_OBJECT;

public:

	HapticPlotDataSelectAction(HapticEditorWindowHud* hud);//, UPDATE_SIGNAL UpdateFunc);

	~HapticPlotDataSelectAction();

	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);


	void SetActiveSelectionRectMode(bool bActive);

	void  UpdateSelection(QPoint ClickPos, QPoint ReleasePos, int PressKey);
	void  UpdateSelection(double StartCoord, double EndCoord, int PressKey, HapticPlotData::eSerchDir StartCoordSerchDir,
		HapticPlotData::eSerchDir EndCoordSerchDir, int SelectAct);

	void  UpdateSelectionFromCursor(QPointF PrevCursorCoord, QPointF NowCursorCoord);


	QCPItemRect*  GetCurItemRect()
	{
		return m_CurxRectItem;
	}

	QPoint GetCurretSelectHzGraphIndex();
	QPoint GetCurretSelectHzGraphIndex(QCPItemRect* RectItem);

	void ClearItems();

	void SetPlotData(HapticPlotData* PlotData)
	{
		m_UserData = PlotData;
	}

	HapticPlotGraph* getGraph() {
		return m_Plot;
	}

	HapticPlotGraphCursor* getCursor()
	{
		return m_Cursor;
	}

	QCPDataRange CoordToRange(double StartCoord, double EndCoord, HapticPlotData::eSerchDir StartCoordSerchDir, HapticPlotData::eSerchDir EndCoordSerchDir);

	void UpdateCursor(QCPItemRect* RectRangeItem);

	HapticEditorWindowHud* GetParentHud() {
		return mParentHud;
	}
private:
	HapticPlotGraph *m_Plot;
	HapticPlotData* m_UserData;

private slots:
	void mouseMoveSignal(QMouseEvent *event);
	void mousePressSignal(QMouseEvent *event);
	void mouseReleaseSignal(QMouseEvent *event);
	void beforeReplot();
	//void selectionChanged();
	//void ItemMouseEvent(QCPAbstractItem *item, QMouseEvent *event);
	
private:
	bool IsClickedRect;

	QPointF dragLegendOrigin;

	QCPDataSelection m_CurSeletion;
	QCPItemRect*   m_CurxRectItem;


	QCPDataRange MergeRange(QCPDataRange Item1, QCPDataRange Item2);
	bool CreateSelectItemRect(QCPDataRange range);
	bool CreateSelectItemRectToCoord(QPointF leftCoord, QPointF RightCoord);

	QCPDataRange m_CurSelectRange;
	HapticEditorWindowHud* mParentHud;

	HapticPlotGraphCursor *m_Cursor;

	friend class SelectRectCommand;
	friend class GraphModifCommand;
};
