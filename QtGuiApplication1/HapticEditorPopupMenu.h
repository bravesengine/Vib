#pragma once
#include <QObject>
#include <QWidget>
class QAction;
class QMenu;

class HapticEditorWindowHud;
class HapticEditorPopupMenu : public QWidget
{
	Q_OBJECT

public:
	HapticEditorPopupMenu(HapticEditorWindowHud *parent);
	~HapticEditorPopupMenu();

	void ShowPopUp(QPoint pt);

protected:

private slots :
	void onCutGraph();
	void onCopyGraph();
	void onPasteGraph();
	void onViewInfo();

private:
		
	QAction *mCutAct;
	QAction *mCopyAct;
	QAction *mPasteAct;
	QAction *mViewInfo;

	HapticEditorWindowHud *m_Parent;

	QMenu *mMenu;

private:

	void createActions();
	void createMenus(HapticEditorWindowHud *parent);
};