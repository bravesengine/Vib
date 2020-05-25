#include "DeviceStateProgressHud.h"

#include "qtimer.h"
#include "qdatetime.h"
#include "qprogressbar.h"
#include "qmovie.h"
#include "qdir.h"

DeviceStateProgressHud::DeviceStateProgressHud(QWidget* parent)
	: QDialog(parent)
{
	this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	ui.setupUi(this);

	m_movie = NULL;

	//QDir dir(".");
	//QString icoPath = dir.absolutePath() + "/Resources/Loading_icon.gif";

	//m_movie = new QMovie(icoPath);
	//ui.waitLabel->setMovie(m_movie);

	//ui.waitLabel->setWindowOpacity(1);
	//m_movie->start();
}


DeviceStateProgressHud::~DeviceStateProgressHud()
{
	if (m_movie)
		delete m_movie;
	m_movie = NULL;
}


void DeviceStateProgressHud::paintEvent(QPaintEvent* /*event*/) {
	QColor backgroundColor = palette().light().color();
	backgroundColor.setAlpha(255);
	QPainter customPainter(this);
	customPainter.fillRect(rect(), backgroundColor);
}

void DeviceStateProgressHud::SetStateMessage(QString Message)
{

}

