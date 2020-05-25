#pragma once

#include <QObject>

//#include "UndoCommands.h"

class QResizeEvent;
class QAction;
class QMenu;
class QToolBar;

class HapticEditorWindowHud;
class QUndoStack;

class QToolButtonWacher;
class HapticEditorToolBar : public QObject
{
	Q_OBJECT

public:
	HapticEditorToolBar(HapticEditorWindowHud *parent, QToolBar* toolBar, QUndoStack* undoStack);
	~HapticEditorToolBar();

	void resizeEvent(QResizeEvent* event);


protected:

private slots :
	void onPlayVibraion();
	void onStopVibraion();
	void onRepalyCheck();

	void onAmpUpVibraion();
	//void onAmpUpVibraionHovered();

	void onAmpDownVibraion();
	void onHzUpCommand();
	void onHzDownCommand();
	void onUnDoCommand();
	void onReDoCommand();

	void ProcessAction();
	//void setEnabledUndo(bool bActive);
	void onPasteGraph();
	void onCopyGraph();
	void onCutGraph();


private:
		
//	QMenu *fileMenu;//

	QAction *mPlayAct;
	QAction *mStopAct;
	QAction *mRePlayAct;

	

	QAction *mAmpUpAct;
	QAction *mAmpDownAct;
	QAction *mHzUpAct;
	QAction *mHzDownAct;

	QAction *mUndoAct;
	QAction *mRedoAct;

	QAction	*mCutAct;
	QAction	*mCopyAct;
	QAction *mPasteAct;


	//QToolBar *fileToolBar;
	//QToolBar *editToolBar;

	QToolBar* m_MainToolBar;

	QToolButtonWacher* mAmpUpWacher;
	QToolButtonWacher* mAmpDownWacher;

	

	HapticEditorWindowHud *m_Parent;

private:
	void createActions(QUndoStack* undoStack);
	void createToolBars(HapticEditorWindowHud *parent);
	void createMenus(HapticEditorWindowHud *parent);

	bool eventFilter(QObject *watched, QEvent *event);


	QUndoStack* mUndoStack;
};