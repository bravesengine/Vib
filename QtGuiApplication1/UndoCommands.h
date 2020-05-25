#pragma once

#include <QWidget>
#include "qundostack.h"
#include <QMetaType>

//#include "UndoDataTypes.h"
#include "HapticPlotData.h"
#include "qcustomplot.h"

static const QString MIME_COPY_GRAPH ("Copy Graph");
//static const QString MIME_CUT_GRAPH("Cut Graph");


class HapticEditorWindowHud;
class QUD_HzPlotData;
class QUD_DataCoord;
class QUD_HzCopyPlotData;

class UnDo_GraphStateData
{
public:
	QList<HzPlotData> m_PlotData;
	QPointF m_SelectLeftCoord;
	QPointF m_SelectRightCoord;
	float  m_TotTime;
	int m_ActType;

	QSharedPointer<QByteArray> CopyClipBoardData;
	QSharedPointer<QByteArray> PrevClipBoardData;
		
	int CopyRangeBegin = 0;
	int CopyRangeEnd = 0;

};

class UnDoCommand
{
public:
	enum eCommandType
	{
		SELECT_GRAPH,
		GRAPH_MODIF,
		GRAPH_COPY,
		//GRAPH_CUT,
	};

	static bool CreateCommand(HapticEditorWindowHud* hud, eCommandType t, int Count, ...);
private:
	static QUndoCommand* CreateCommand(HapticEditorWindowHud* hud, eCommandType t, const QVariantList &vList);
};

class UndoCommandBase : public QUndoCommand
{
public:
	UndoCommandBase()
	{

	}
	~UndoCommandBase()
	{

	}
private:	
	int mRedoCallCount = 0;
protected:
	void IncreaseCallRedoCount() { mRedoCallCount++; }
	int  RedoCallCount() {return mRedoCallCount ; }
};

/////////////////////
class GraphCopyCommand : public UndoCommandBase
{
public:
	GraphCopyCommand(HapticEditorWindowHud* hud, const QVariantList &vList);
	~GraphCopyCommand();

public:
	virtual void redo();
	virtual	void undo();

private:
	HapticEditorWindowHud* mHud;
	bool mbVariable;

	QUD_HzCopyPlotData* mCopyGraphData;
};

//
//class GraphCutCommand : public UndoCommandBase
//{
//public:
//	GraphCutCommand(HapticEditorWindowHud* hud, const QVariantList &vList);
//	~GraphCutCommand();
//
//public:
//	virtual void redo();
//	virtual	void undo();
//
//private:
//	HapticEditorWindowHud* mHud;
//	bool mbVariable;
//
//	QUD_HzPlotData* mCutGraphData;
//};
//
//////////////////////////////////////

////////////////////////////////////
class SelectRectCommand : public UndoCommandBase
{
public:
	SelectRectCommand(HapticEditorWindowHud* hud, const QVariantList &vList);
	~SelectRectCommand();

public:
	virtual void redo();
	virtual	void undo();

private:
	HapticEditorWindowHud* mHud;
	QUD_DataCoord *mSelectRange;

	bool mbVariable;

	bool UpdateSelectItem(QPointF SelectCoord);
};

class GraphModifCommand : public UndoCommandBase
{

public:
	GraphModifCommand() {}
	GraphModifCommand(HapticEditorWindowHud* hud, const QVariantList &vList);
	~GraphModifCommand();

public:
	virtual void redo();
	virtual	void undo();

protected:
	HapticEditorWindowHud* mHud;

	QUD_HzPlotData *mHzPlotData;
	//QUD_HzCopyPlotData* mClipBoardGraphData;

	bool mbVariable;

	bool RedoProcess();
	bool UndoProcess();

};




