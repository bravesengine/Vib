#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtHapticMain.h"

class HapticEditorWindowHud;
class QSplitter;
class QUndoStack;
class QUndoView;
class HapticEditorMainToolBar;
class DeviceSelectHud;

class QListWidget;
class QtHapticMain : public QMainWindow
{
	Q_OBJECT

public:
	QtHapticMain();
	~QtHapticMain();


	HapticEditorWindowHud* getEditorHud() {
		return m_EditorHud;
	}




	static QtHapticMain* Instance();

private:
	Ui::QtHapticMainClass ui;
	//void on_actionRestore_layout_triggered();

	static QtHapticMain* mInstance;

	
	QMainWindow* m_SplitMainWindow;
	//QWidget* m_SplitMainWindow;

	QDockWidget* m_BottomDockWidget;
	//QDockWidget* m_dockWidget2;
	//QDockWidget* m_dockWidget3;

	HapticEditorWindowHud* m_EditorHud;

	DeviceSelectHud* m_DeiviceSelectHud;

	QSplitter *m_Splitter;

	QDockWidget *m_HistoryDock;

	HapticEditorMainToolBar* m_ToolBar;


protected:
	void resizeEvent(QResizeEvent *event) override;

	//void mousePressEvent(QMouseEvent *event) override;
	//void mouseMoveEvent(QMouseEvent *event) override;
	//void mouseReleaseEvent(QMouseEvent *event) override;

//private slots:


private :
	QUndoView *undoView;

	void createUndoView();

	//QTimer *mTimer;

public:
	void ToggleHistoryHud();
	bool WaveToRaw_LoadDialog();
};
