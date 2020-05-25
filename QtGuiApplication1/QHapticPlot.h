#pragma once

#include <QObject>
#include "qcustomplot.h"

class QHapticPlot : public QObject
{
	Q_OBJECT

public:
	QHapticPlot(QObject *parent);
	~QHapticPlot();


};
