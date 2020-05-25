#pragma once

#include "qcustomplot.h"

/* including file 'src/items/item-tracer.h', size 4762                       */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class QCP_LIB_DECL QCPItemTracerExt : public QCPItemTracer
{
	Q_OBJECT
public:
	explicit QCPItemTracerExt(QCustomPlot *parentPlot);
	virtual ~QCPItemTracerExt();

	// non-virtual methods:
	virtual bool updatePosition();

protected:

};
