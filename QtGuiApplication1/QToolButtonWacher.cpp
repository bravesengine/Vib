
#include <QToolButtonWacher.h>
#include "HapticEditorWindowHud.h"
#include "qtoolbar.h"
#include "QToolButton.h"

QToolButtonWacher::QToolButtonWacher(QToolButton *toolBtn)
{
	mToolBtn = toolBtn;
	mToolBtn->installEventFilter(this);
}

QToolButtonWacher::~QToolButtonWacher()
{

}

bool QToolButtonWacher::eventFilter(QObject *watched, QEvent *event)
{
	//if (event->type() == QEvent::Enter)
	//{
	//	qDebug() << event->type();
	//}
	
	if (event->type() == QEvent::Leave)
	{
		mbOnMouseOver = false;
	}

	if (event->type() == QEvent::MouseButtonPress)
	{
		mbPress = true;
	
		if(mbHover)
			mbOnMouseOver = true;
	}

	if (event->type() == QEvent::HoverEnter)
	{
		mbHover = true;
	}
	if (event->type() == QEvent::HoverLeave)
	{
		mbOnMouseOver = false;
		mbHover = false;
	}	
	if (event->type() == QEvent::MouseMove)
	{
		//	Qt::MouseButton Button =->button();
		if (mbPress && mbHover)
		{
			QMouseEvent *MouseEvent = static_cast<QMouseEvent *>(event);

			QPoint globalPos = mToolBtn->mapToGlobal(MouseEvent->pos());
			QPoint LocalPos = mToolBtn->mapFromGlobal(globalPos);
			//QPoint LocalMouse = mToolBtn->mapFromGlobal(MouseEvent->pos());

			QRect BtnRectHover = QRect(QPoint(0, 0), mToolBtn->size());
			
			if (!BtnRectHover.intersected(QRect(QPoint(LocalPos), QSize(1, 1))).isEmpty())
				mbOnMouseOver = true;
			else
				mbOnMouseOver = false;			
			//qDebug() << "global:  " << globalPos << "local :" << LocalPos << "ToolHoverArea :" << BtnRectHover;			
		}
	}

	if (event->type() == QEvent::MouseButtonRelease)
	{
		qDebug() << "Release!!";
		mbPress = false;
	}
	return QObject::eventFilter(watched, event);
}
