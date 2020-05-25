#pragma once
//
#include <QWidget>
#include "singleton.h"
#include "qmap.h"

class QKeyEvent;
class HapticEditorWindowHud;
class HapticPlotKeyAction 
{
public:
	
	typedef  std::function<void(QPoint, QPoint,int)> UPDATE_SELECT_FUNC;

	HapticPlotKeyAction();
	~HapticPlotKeyAction();


	//--use sample
	static HapticPlotKeyAction* instance()
	{
		return Singleton<HapticPlotKeyAction>::instance(HapticPlotKeyAction::createInstance);
	}

	void KeyPress(QKeyEvent *e, HapticEditorWindowHud*);
	void KeyRelease(QKeyEvent *e, HapticEditorWindowHud*);

	void ClearBuffer();
	void RemoveBuffer(int key);


	void mouseMove(QMouseEvent *event, HapticEditorWindowHud* ParentHud);
	void mousePress(QMouseEvent *event, HapticEditorWindowHud* ParentHud);
	void mouseRelease(QMouseEvent *event, HapticEditorWindowHud* ParentHud);// UPDATE_SELECT_FUNC SelectedFucn);


	bool isKeyDown(Qt::Key key);

private:
	static HapticPlotKeyAction* createInstance();

	
	QMap<int, QKeyEvent*> mKeyBuffer;

	bool	m_bMouseLeftClick = false;
	bool    m_bMoseDrag = false;

	QPoint  m_MouseClickPos;

};
