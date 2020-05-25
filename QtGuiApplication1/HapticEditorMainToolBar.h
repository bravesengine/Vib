#pragma once

#include <QObject>
#include <qwidget.h>
class QAction;
class QMenu;
class QToolBar;
class QtHapticMain;

class HapticEditorMainToolBar : public QWidget
{
	Q_OBJECT

public:
	HapticEditorMainToolBar(QToolBar *toolBar);
	~HapticEditorMainToolBar();

	static bool mbShowHistory;
protected:

private slots :
	void onNewDoc();
	void onLoad();
	void onSave();
	void onShowHistory();

	//void onWaveToDoc();
private:

	QAction *mNewDocAct;
	QAction *mLoadAct;
	QAction *mSaveAct;
	QAction *mShowHistoryAct;

	QAction *mWavetoDocAct;

	QToolBar* m_MainToolBar;



private:
	void createActions();
	void createToolBars();
	void createMenus();

	int ShowWarningBox();
};