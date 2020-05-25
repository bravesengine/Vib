#pragma once

#include <QWidget>
#include "HapticPlotData.h"

class QCustomPlot;
class QCPItemPixmap;
class QCPItemLine;
class QCPAbstractItem;
class QMouseEvent;
class HapticPlotDataSelectAction;
class QCPItemEllipse;
class QCPItemTracerExt;
class QCPDataRange;
class QCPItemRect;

class HapticPlotGraphCursor : public QObject
{
	Q_OBJECT

public:	
	
	class CursorGeomety
	{
	public:

		void Initize(QString Name, QString LayerName, QCustomPlot *customPlot);
		void Move(double Coord);
		void Active(bool bActive);

		QCPItemTracerExt *CursorItem = NULL;
		QCPItemLine *CursorLine  = NULL;

		void CaptureCoord();

		double GetPrevCoord() {
			return mPrevCoord;
		}

	private:
		double mPrevCoord = 0;
	};

	HapticPlotGraphCursor(HapticPlotDataSelectAction *SelectAction);
	~HapticPlotGraphCursor();

	CursorGeomety* GetItemClickedItem()
	{
		return mClickedCursor;
	}

	int ResetPositionToSnap();


	//void UpdateLCursor(double CursorCoord);
	//void UpdateRCursor(double CursorCoord);
	//void UpdateCursor(double LCursorCoord, double RCursorCoord);	
	//void UpdateCursor(QCPDataRange Range);

	void Active(bool bActive);

	void UpdateCursor(QCPItemRect* RectRangeItem);
	
private:

	QCustomPlot *m_Plot;
	HapticPlotDataSelectAction* mSelectAction;

	CursorGeomety *mClickedCursor = NULL;

	CursorGeomety *mLCursor = NULL;
	CursorGeomety *mRCursor = NULL;

	void UpdateCursor(CursorGeomety* Cursor, double CursorCoord);

private slots:
	void mousePressItem(QCPAbstractItem* item, QMouseEvent* event);
	void mouseReleaseSignal(QMouseEvent* event);
	void mouseMoveSignal(QMouseEvent* event);
	void mousePressSignal(QMouseEvent* event);



};
