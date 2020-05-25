#pragma once

#include <QObject>

class QToolButton;
class QToolButtonWacher: public QObject
{
	Q_OBJECT
public:
	QToolButtonWacher(QToolButton *toolBtn);
	~QToolButtonWacher();

	bool IsPress()
	{
		return mbPress;
	}
	bool IsMouseOver()
	{
		return mbOnMouseOver;
	}

protected:
	bool mbPress = false;
	bool mbHover = false;
	bool mbOnMouseOver = false;

	QToolButton* mToolBtn;
private slots :
private:

	bool eventFilter(QObject *watched, QEvent *event);
};