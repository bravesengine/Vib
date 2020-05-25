#pragma once

#include <QWidget>
#include "HapticIDevice.h"
namespace HT
{

	class HapticDevice_Vive : public HapticIDevice
	{
		Q_OBJECT
	public:
		HapticDevice_Vive();
		~HapticDevice_Vive();

		virtual int Initize()
		{
			return 0;
		}
		virtual int reInitize(EVENT_SIGNAL signal, HPLAYER_EVENT_SIGNAL playerSignal)
		{
			return 0;
		}

		virtual int Update(double interval)
		{
			return 0;
		}


	private:
	};

};