#pragma once

#include <QWidget>
#include "HapticIDevice.h"

#include <QtGamepad>

namespace HT
{
	class XBoxDeviceState : public HapticDeviceState
	{
		Q_OBJECT
	public:
		XBoxDeviceState(EVENT_SIGNAL signal, HPLAYER_EVENT_SIGNAL playerSignal) : HapticDeviceState(signal, playerSignal)
		{

		}

		~XBoxDeviceState() {}

		virtual bool isVaild() const;
		virtual QString getStateMessage() const;
	};


	class HapticDevice_XboxPad : public HapticIDevice
	{
		Q_OBJECT
	public:
		HapticDevice_XboxPad();
		~HapticDevice_XboxPad();

		virtual int Initize();

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
