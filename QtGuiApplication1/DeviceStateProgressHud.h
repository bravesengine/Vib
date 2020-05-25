#pragma once
#include <QWidget>
#include <QDialog>

#include <ui_DeviceStateProgress.h>

#include "qlabel.h"
#include "qpainter.h"
#include "qpropertyanimation.h"

class QProgressDialog;
class QMovie;
class DeviceStateProgressHud : public QDialog
{
Q_OBJECT
	
public:
	
	Ui::DeviceStateProgress ui;

public:
	explicit DeviceStateProgressHud(QWidget* parent = nullptr);
	~DeviceStateProgressHud();
	
	void SetStateMessage(QString Message);

protected:
	void paintEvent(QPaintEvent* event);

private:

	QMovie* m_movie;
private slots:

};
