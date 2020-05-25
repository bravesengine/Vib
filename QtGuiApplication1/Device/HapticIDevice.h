#pragma once

#include <QWidget>

#include "HapticDeviceManager.h"
#include "HapticDeviceState.h"

namespace HT
{
	class HapticBufferClip;
	class HapticIDevice : public QObject
	{
		Q_OBJECT
	public:
		HapticIDevice();
		~HapticIDevice();

		virtual int Initize() = 0;
		virtual int reInitize(EVENT_SIGNAL signal, HPLAYER_EVENT_SIGNAL playerSignal) = 0;
		virtual int Update(double interval) = 0;

		virtual bool isConnected() = 0;
		virtual bool isRunning() = 0;


		const HapticDeviceState* getState()
		{
			return mState;
		}

		virtual void InsertVibraionClip(QByteArray &RawData, int subControllerIndex, double timelength) = 0;

		virtual void StopVibraionAll() = 0;
		virtual void ClearVibraionClip() = 0;
		virtual void RunVibraion() = 0;

		virtual bool ClearCommand() = 0;
	private:
		
	protected:
		HapticDeviceState* mState;
	};
}

