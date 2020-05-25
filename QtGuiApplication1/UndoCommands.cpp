#include "UndoCommands.h"
#include "UndoDataTypes.h"
#include "HapticEditorWindowHud.h"
#include "HapticPlotDataSelectAction.h"
#include "HapticPlotGraph.h"
#include "HapticModifFunc.h"


bool UnDoCommand::CreateCommand(HapticEditorWindowHud* hud, UnDoCommand::eCommandType t, int Count, ...)
{
	if (hud == NULL)
		return false;


	if (hud->GetUndoStack() == NULL)
		return false;

	va_list ap;
	va_start(ap, Count);

	QUndoCommand* Command = NULL;
	QVariantList vList;
	try
	{
		for (int i = 0; i < Count; i++)
		{
			QVariant v = va_arg(ap, QVariant);
			vList.append(v);
		}
		
		Command = CreateCommand(hud, t, vList);
		va_end(ap);
	}
	catch (...)
	{

		return false;
	}

	if (Command == NULL)
		return false;

	//if (!hud->GetUndoStack()->undoOrRedoRunning())
	//{

	//}


	hud->GetUndoStack()->push(Command);
	return true;
}

QUndoCommand* UnDoCommand::CreateCommand(HapticEditorWindowHud* hud, UnDoCommand::eCommandType t, const QVariantList &vList)
{
	QUndoCommand *Command = NULL;
	//-- 음?
	switch (t)
	{
	case SELECT_GRAPH:
	{
		Command = new SelectRectCommand(hud, vList);
	}
	break;
	case GRAPH_MODIF:
	{
		Command = new GraphModifCommand(hud, vList);
	}
	break;
	case GRAPH_COPY:
	{
		Command = new GraphCopyCommand(hud, vList);
	}
	break;
	//case GRAPH_CUT:
	//{
	//	Command = new GraphCutCommand(hud, vList);
	//}
	//break;	
	default:
		return NULL;
	}

	return Command;
}



/////////////////////////////////////////////////////////////////////////////////////////
//
// GraphCopyCommand
//
GraphCopyCommand::GraphCopyCommand(HapticEditorWindowHud* hud, const QVariantList &vList)
{
	mHud = hud;
	mbVariable = false;

	if (vList.size() != 1)
	{
		setText(QString("GraphCopyCommand: CopyGraph Invalid arg!!"));
		return;
	}

	QVariant v = vList.at(0);
	mCopyGraphData = new QUD_HzCopyPlotData(v.value<QUD_HzCopyPlotData>());

	mbVariable = true;

	setText(QString("CopyGraphCommand: SelectGraph"));
}

GraphCopyCommand::~GraphCopyCommand()
{

}

void GraphCopyCommand::redo()
{
	if (mHud == NULL || !mbVariable)
		return;

	if (RedoCallCount() != 0)
	{
		qDebug() << "GraphCopyCommand redo()";

		if (mCopyGraphData->new_Data().get() == NULL)
			return;

		QClipboard* _clipboard = QApplication::clipboard();

		QSharedPointer<QByteArray> v = mCopyGraphData->new_Data();

		qDebug() << "ClipBoard Redo CopySize =" << v.get()->length();

		QMimeData* hzMimeData = new QMimeData;
		hzMimeData->setData(MIME_COPY_GRAPH, *v.get());
		hzMimeData->setText("Copy HzPlotData");
		_clipboard->setMimeData(hzMimeData);
	}

	IncreaseCallRedoCount();
}

void GraphCopyCommand::undo()
{
	// 이전값두 필요하잖아..
	qDebug() << "GraphCopyCommand undo()";

	if (mCopyGraphData->old_Data().get() == NULL)
		return;

	QClipboard* _clipboard = QApplication::clipboard();
	QMimeData* hzMimeData = new QMimeData;

	QSharedPointer<QByteArray> v = mCopyGraphData->old_Data();
	qDebug() << "ClipBoard UnDo WriteSize =" << v.get()->length();

	hzMimeData->setData(MIME_COPY_GRAPH, *v.get());
	hzMimeData->setText("Copy HzPlotData");
	_clipboard->setMimeData(hzMimeData);

}




///////////////////////////////////////////////////////////////////////////////////////////
////
//// GraphCutCommand
////
//
//GraphCutCommand::GraphCutCommand(HapticEditorWindowHud* hud, const QVariantList &vList)
//{
//	mHud = hud;
//	mbVariable = false;
//
//	if (vList.size() != 2)
//	{
//		setText(QString("GraphCutCommand: Invalid arg!!"));
//		return;
//	}
//
//	//QVariant v = vList.at(0);
//	//mHzPlotData = new QUD_HzPlotData(v.value<QUD_HzPlotData>());
//	//HaticModifGraph::eModifGraphDataType ModifType = (HaticModifGraph::eModifGraphDataType) mHzPlotData->ModifType();
//	//HapticEditorWindowHud::eActType ActType = (HapticEditorWindowHud::eActType) mHzPlotData->ActType();
//	//QMetaEnum metaEnum1 = QMetaEnum::fromType<HaticModifGraph::eModifGraphDataType>();
//	//QMetaEnum metaEnum2 = QMetaEnum::fromType< HapticEditorWindowHud::eActType>();
//	//setText(QString("GraphModifCommand = %1 Act = %2").arg(metaEnum1.valueToKey(ModifType)).arg(metaEnum2.valueToKey(ActType)));
//
//	mbVariable = true;
//
//}
//
//GraphCutCommand::~GraphCutCommand()
//{
//
//
//}
//
//void GraphCutCommand::redo()
//{
//	qDebug() << "GraphCutCommand redo()";
//	if (mHud == NULL || !mbVariable)
//		return;
//
//	GraphModifCommand::redo();
//
//	// 클립보드 복구.
//
//}
//
//void GraphCutCommand::undo()
//{
//	qDebug() << "GraphCutCommand undo()";
//	if (mHud == NULL || !mbVariable)
//		return;
//
//	GraphModifCommand::undo();
//
//	// 클립보드 복구.
//}


/////////////////////////////////////////////////////////////////////////////////////////
//
// SelectRectCommand
//

SelectRectCommand::~SelectRectCommand()
{
	if (mSelectRange)
		delete mSelectRange;

	mSelectRange = NULL;

}

SelectRectCommand::SelectRectCommand(HapticEditorWindowHud* hud, const QVariantList &vList)
{
	mHud = hud;
	mbVariable = false;
	mSelectRange = NULL;

	if (vList.size() != 1)
	{
		setText(QString("Select Graph Invalid arg!!"));
		return;
	}

	QVariant v = vList.at(0);	
	mSelectRange = new QUD_DataCoord(v.value<QUD_DataCoord>());

	mbVariable = true;

	HapticEditorWindowHud::eSelectActType ActType = (HapticEditorWindowHud::eSelectActType) mSelectRange->ActType();

	QMetaEnum metaEnum = QMetaEnum::fromType< HapticEditorWindowHud::eSelectActType>();
	setText(QString("Selected Graph : %1").arg(metaEnum.valueToKey(ActType)));
}

void SelectRectCommand::redo()
{
	if (RedoCallCount() != 0)
		qDebug() << "SelectRectCommand redo()";
	
	UpdateSelectItem(mSelectRange->new_Coord());

	IncreaseCallRedoCount();
}

void SelectRectCommand::undo()
{
	qDebug() << "SelectRectCommand undo()";

	UpdateSelectItem(mSelectRange->old_Coord());
}

bool SelectRectCommand::UpdateSelectItem(QPointF SelectCoord)
{
	if (mHud == NULL || !mbVariable)
		return false;

	if (RedoCallCount() == 0)
		return false;

	HapticPlotDataSelectAction* SelectAction = mHud->SelectAction();
	if (SelectAction == NULL)
		return false;
	
	SelectAction->CreateSelectItemRectToCoord(QPointF(SelectCoord.x(), AMP_LIMIT_VALUE), QPointF(SelectCoord.y(), -AMP_LIMIT_VALUE));

	SelectAction->UpdateCursor(SelectAction->GetCurItemRect());

	////SelectAction->CreateSelectItemRect(SelectCoord);
	mHud->GetGraph()->getCustomPlot()->replot();

	return true;
}



/////////////////////////////////////////////////////////////////////////////////////////
//
// GraphModifCommand
//
GraphModifCommand::GraphModifCommand(HapticEditorWindowHud* hud, const QVariantList &vList)
{
	mHud = hud;
	mbVariable = false;

	if (vList.size() != 1)
	{
		setText(QString("GraphModifCommand: Invalid arg!!"));
		return;
	}

	QVariant v1 = vList.at(0);

	mHzPlotData = new QUD_HzPlotData(v1.value<QUD_HzPlotData>());


	//QVariant v2 = vList.at(1);
	//mClipBoardGraphData = new QUD_HzCopyPlotData(v2.value<QUD_HzCopyPlotData>());

	HaticModifGraph::eModifGraphDataType ModifType = (HaticModifGraph::eModifGraphDataType) mHzPlotData->ModifType();
	HapticEditorWindowHud::eActType ActType = (HapticEditorWindowHud::eActType) mHzPlotData->ActType();

	QMetaEnum metaEnum1 = QMetaEnum::fromType<HaticModifGraph::eModifGraphDataType>();
	QMetaEnum metaEnum2 = QMetaEnum::fromType< HapticEditorWindowHud::eActType>();

	setText(QString("GraphModif: %1 , %2").arg(metaEnum1.valueToKey(ModifType)).arg(metaEnum2.valueToKey(ActType)));



	mbVariable = true;
}

GraphModifCommand::~GraphModifCommand()
{

}

bool GraphModifCommand::RedoProcess()
{
	HapticPlotDataSelectAction* SelectAction = mHud->SelectAction();
	if (RedoCallCount() == 0)
		return false;

	QList<HzPlotData> hzData = mHzPlotData->new_Data().m_PlotData;

	////////////////
	if (mHzPlotData->new_Data().CopyClipBoardData.get() != NULL)
	{
		QClipboard* _clipboard = QApplication::clipboard();

		QMimeData* hzMimeData = new QMimeData;
		hzMimeData->setData(MIME_COPY_GRAPH, *mHzPlotData->new_Data().CopyClipBoardData.get());
		hzMimeData->setText("Copy HzPlotData");
		_clipboard->setMimeData(hzMimeData);

		qDebug() << "GraphModifCommand:" << "ClipBoard Redo WriteSize =" << mHzPlotData->new_Data().CopyClipBoardData.get()->length();
	}


	mHud->GetPlotData()->DataToReMakeTime(hzData);
	mHud->GetGraph()->SetGraph(hzData, mHud->GetPlotData()->GetTimeLength());

	SelectAction->CreateSelectItemRectToCoord(mHzPlotData->new_Data().m_SelectLeftCoord, mHzPlotData->new_Data().m_SelectRightCoord);
	SelectAction->UpdateCursor(SelectAction->GetCurItemRect());

	mHud->GetGraph()->getCustomPlot()->replot();
	mHud->UpdatePlotHud(false);


	return true;
}

bool GraphModifCommand::UndoProcess()
{
	HapticPlotDataSelectAction* SelectAction = mHud->SelectAction();
	if (SelectAction == NULL)
		return false;

	QList<HzPlotData> hzData = mHzPlotData->old_Data().m_PlotData;


	if (mHzPlotData->old_Data().PrevClipBoardData.get() != NULL)
	{
		QClipboard* _clipboard = QApplication::clipboard();
		QMimeData* hzMimeData = new QMimeData;
		hzMimeData->setData(MIME_COPY_GRAPH, *mHzPlotData->old_Data().PrevClipBoardData.get());
		hzMimeData->setText("Copy HzPlotData");
		_clipboard->setMimeData(hzMimeData);

		qDebug() <<  "GraphModifCommand:" << "ClipBoard Undo WriteSize =" << mHzPlotData->old_Data().PrevClipBoardData.get()->length();
	}


	mHud->GetPlotData()->DataToReMakeTime(hzData);
	mHud->GetGraph()->SetGraph(hzData, mHud->GetPlotData()->GetTimeLength());

	//-- select Rect..
	SelectAction->CreateSelectItemRectToCoord(mHzPlotData->old_Data().m_SelectLeftCoord, mHzPlotData->old_Data().m_SelectRightCoord);
	SelectAction->UpdateCursor(SelectAction->GetCurItemRect());

	mHud->GetGraph()->getCustomPlot()->replot();
	mHud->UpdatePlotHud(false);

	return true;
}


void GraphModifCommand::redo()
{
	if (RedoCallCount() != 0)
		qDebug() << "GraphModifCommand redo()";
	
	if (mHud == NULL || !mbVariable)
		return;

	RedoProcess();
	IncreaseCallRedoCount();
}

void GraphModifCommand::undo()
{
	if (mHzPlotData)
	{
		HaticModifGraph::eModifGraphDataType ModifType = (HaticModifGraph::eModifGraphDataType) mHzPlotData->ModifType();
		QMetaEnum metaEnum1 = QMetaEnum::fromType<HaticModifGraph::eModifGraphDataType>();
		qDebug() << QString("GraphModifCommand (%1} undo()").arg(metaEnum1.valueToKey(ModifType));
	}
	else
	{
		qDebug() << QString("GraphModifCommand undo()");
	}

	if (mHud == NULL || !mbVariable)
		return;

	UndoProcess();
}
