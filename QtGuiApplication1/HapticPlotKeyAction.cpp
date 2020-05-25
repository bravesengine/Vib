#include "HapticPlotKeyAction.h"
#include "qevent.h"
#include "HapticEditorWindowHud.h"
#include "HapticPlotDataSelectAction.h"
#include "HapticPlotGraphCursor.h"

HapticPlotKeyAction::HapticPlotKeyAction()
{


}

HapticPlotKeyAction::~HapticPlotKeyAction()
{
	ClearBuffer();
}

void HapticPlotKeyAction::ClearBuffer()
{
	for (int i = 0; i < mKeyBuffer.size(); i++)
	{
		delete mKeyBuffer[i];
	}

	mKeyBuffer.clear();
}
HapticPlotKeyAction* HapticPlotKeyAction::createInstance()
{
	return new HapticPlotKeyAction();
}

bool HapticPlotKeyAction::isKeyDown(Qt::Key key)
{
	if (mKeyBuffer.contains(key))
		return true;

	return false;
}

void HapticPlotKeyAction::KeyPress(QKeyEvent *e, HapticEditorWindowHud* hud)
{
	if (hud == NULL)
		return;

	if (hud->SelectAction() == NULL)
		return;

	hud->SelectAction()->keyPressEvent(e);

	if (e->isAutoRepeat())
		return;

	if (mKeyBuffer.contains(e->key()))
		return;

	switch ((Qt::Key) e->key())
	{
		case  Qt::Key::Key_Shift:
		case  Qt::Key::Key_Control:
		{			
			mKeyBuffer.insert(e->key(), new QKeyEvent(*e));
			break;
		}
	default:
		break;
	}
}

void HapticPlotKeyAction::KeyRelease(QKeyEvent *e, HapticEditorWindowHud* hud)
{
	if (e->isAutoRepeat() || hud == NULL)
		return;

	if (hud->SelectAction())
		hud->SelectAction()->keyReleaseEvent(e);

	switch ((Qt::Key) e->key())
	{
	case  Qt::Key::Key_Shift:
	case  Qt::Key::Key_Control:
	{
		RemoveBuffer(e->key());
		break;
	}
	default:
		break;
	}
}

void HapticPlotKeyAction::RemoveBuffer(int key)
{
	if (!mKeyBuffer.contains(key))
		return;

	QKeyEvent* Event = mKeyBuffer[key];
	mKeyBuffer.remove(key);
	delete Event;
	Event = NULL;
}

void HapticPlotKeyAction::mouseMove(QMouseEvent *event, HapticEditorWindowHud* ParentHud)
{
	if (event == NULL || ParentHud == NULL)
		return;

	if (m_bMouseLeftClick)
	{
		m_bMoseDrag = true;




	}
}

void HapticPlotKeyAction::mousePress(QMouseEvent *event, HapticEditorWindowHud* ParentHud)
{
	if (event == NULL || ParentHud == NULL)
		return;

	if (event->button() == Qt::MouseButton::LeftButton)
	{
		m_MouseClickPos = event->pos();

		//  acstroctItem 클릭 하지 않았을경우만?
		if (ParentHud->SelectAction()->getCursor()->GetItemClickedItem() == NULL)
		{
			qDebug() << "SetActiveSelectionRectMode";

			m_bMouseLeftClick = true;
			ParentHud->SelectAction()->SetActiveSelectionRectMode(true);
		}

	}

} 

void HapticPlotKeyAction::mouseRelease(QMouseEvent *event, HapticEditorWindowHud* ParentHud)//UPDATE_SELECT_FUNC SelectedFucn)
{
	if (event == NULL || ParentHud == NULL)
		return;

	if (event->button() == Qt::MouseButton::LeftButton && m_bMouseLeftClick)
	{
		m_bMouseLeftClick = false;
		ParentHud->SelectAction()->SetActiveSelectionRectMode(false);
	
	
		//if (m_bMoseDrag && SelectedFucn != NULL)
		//{
		if (isKeyDown(Qt::Key::Key_Shift))
			ParentHud->SelectAction()->UpdateSelection(m_MouseClickPos, event->pos(), Qt::Key::Key_Shift);
		else
			ParentHud->SelectAction()->UpdateSelection(m_MouseClickPos, event->pos(), 0);

		//ParentHud->getUi().DescAmpValueLabel
		//const Ui::HapticEditorHud& getUi()

		ParentHud->UpdateHelpDesc();

	}
	else if (event->button() == Qt::MouseButton::RightButton)
	{

		ParentHud->ShowMenu(event->pos());

	}

	m_bMoseDrag = false;
}
