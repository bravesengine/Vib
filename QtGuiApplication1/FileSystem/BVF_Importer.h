#pragma once

#include <QWidget>
#include "H_Importer.h"

class HapticPlotData;

namespace HT
{
	class BVF_Importer : public H_Importer
	{
		Q_OBJECT
	public:
		BVF_Importer();
		~BVF_Importer();

	private:

		virtual bool LoadFile(QString File, HapticPlotData* ImportData);
	};
};

