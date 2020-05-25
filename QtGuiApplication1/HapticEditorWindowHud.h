#pragma once

#include <QWidget>
#include <QDialog>
#include <ui_HapticEditorHud.h>
#include <ui_HapticEditorSplit.h>
#include "HapticPlotData.h"
#include "HapticModifFunc.h"

#include "Device/HT_GlobalTypes.h"
class HapticPlotDataSelectAction;
class HapticPlotGraph;
class HapticEditorToolBar;
class UnDo_GraphStateData;

class HapticEditorPopupMenu;
class HaticModifGraph;

class HapticEditorWindowHud : public QWidget
{
	Q_OBJECT

public:
	
	enum eActType
	{
		KEYBOARD,
		TOOLBAR,
		POPMENU,
	};
	
	Q_ENUM(eActType)

	enum eSelectActType
	{
		_MOUSE_,
		_CURSOR_,
	};

	Q_ENUM(eSelectActType)


	HapticEditorWindowHud(QWidget *parent = Q_NULLPTR);
	~HapticEditorWindowHud();

	void Put(QString logText);

	void resizeEvent(QResizeEvent* event);


	void UpdatePlotHud(bool bUpdateRange);
	void UpdateHelpDesc();


	HapticPlotDataSelectAction* SelectAction() {
		return m_SelectAction;
	}
	//void Set
	HapticPlotGraph* GetGraph()
	{
		return m_HapticGraph;
	}

	HapticPlotData* GetPlotData()
	{
		return m_HapticData;
	}

	QUndoStack* GetUndoStack() {
		return undoStack;
	}

	bool BeginModifGraphData(eActType act, HaticModifGraph::eModifGraphDataType t, bool bWriteClipBoardData = false);
	bool EndModifGraphData(eActType act, HaticModifGraph::eModifGraphDataType t, bool bInsertCommad);

	bool UpdateModifGraphData(eActType act, HaticModifGraph::eModifGraphDataType t);

	void UpdateWorkDesc(QString desc);



	bool CopySelectedGraphtoClipBoard();
	bool CutSelectedGraphtoClipBoard();

	const Ui::HapticEditorHud& getUi()
	{
		return ui;
	};

	HT::HPLAYER_EVENT_SIGNAL getEventSignal() {
		return m_EventSignal;
	}

	void ShowMenu(QPoint pt);

	void InitPlotData();

	bool SetPlotData(HapticPlotData* PlotData);

private:
	Ui::HapticEditorHud ui;
	float m_TimeLength;

	HapticPlotDataSelectAction *m_SelectAction;
	HapticPlotData *m_HapticData;
	HapticPlotGraph *m_HapticGraph;
	
	HapticEditorToolBar *mToolBar;

	HapticEditorPopupMenu* mPopUpMenu;


	UnDo_GraphStateData* mPrevStateData;

	void OnHapticDeviceEvent(int EventType, double Value);

	QSharedPointer<QByteArray> CopyCurrectSelectedToByteArray(int &RangeBein, int& RangeEnd, int& GraphCount);

	QMap<int, HaticModifGraph*>  mGraphModifFunc;

private slots:
	void Test();

	void keyPressEvent(QKeyEvent*);
	void keyReleaseEvent(QKeyEvent*);

	void horzScrollBarChanged(int value);
	//void vertScrollBarChanged(int value);
	void xAxisChanged(const QCPRange &newRange, const QCPRange &oldRange);
	//void xAxisScaleChanged(const QCPRange &newRange);
	//void yAxisChanged(QCPRange range);
	void yAxisChanged(const QCPRange &newRange, const QCPRange &oldRange);

private:
	void updateScrollBarStep();	

	QUndoStack *undoStack;

	HT::HPLAYER_EVENT_SIGNAL m_EventSignal;

//	bool  SelectedGraphtoClipBoard(QString MimeName);

	bool  SelectedGraphtoClipBoard(QString MimeName, QByteArray &PrevitemData,
		QByteArray &CopyData, int &RangeBegin, int &RangeEnd);

};

//Q_DECLARE_METATYPE(HapticEditorWindowHud)
//Q_DECLARE_METATYPE(HapticEditorWindowHud::eActType)
//Q_DECLARE_METATYPE(HapticEditorWindowHud::eSelectActType)
