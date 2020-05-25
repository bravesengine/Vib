#pragma once

#include <QWidget>

#include "HT_GlobalTypes.h"
#include "HapticIDevice.h"
#include "HapticDeviceState.h"
#include "HapticDeviceCommander.h"

#include "OVR_CAPI.h"

namespace HT
{
	class HapticBufferClip;
	class OculusDeviceState : public HapticDeviceState
	{
		Q_OBJECT
	public:
		OculusDeviceState(EVENT_SIGNAL signal, HPLAYER_EVENT_SIGNAL playerSignal) : HapticDeviceState(signal, playerSignal)
		{

		}

		~OculusDeviceState() {}

		virtual bool isVaild() const;
		virtual QString getStateMessage() const;
	};

	class HapticDevice_Oculus : public HapticIDevice//, public HapticDeviceCommander
	{
		Q_OBJECT
	public:
		HapticDevice_Oculus(EVENT_SIGNAL signal, HPLAYER_EVENT_SIGNAL playerSignal);
		~HapticDevice_Oculus();

		virtual int Initize();
		virtual int reInitize(EVENT_SIGNAL signal, HPLAYER_EVENT_SIGNAL playerSignal);
		virtual int Update(double interval);

		virtual void InsertVibraionClip(QByteArray &RawData, int subControllerIndex, double timelength);

		virtual void StopVibraionAll();
		virtual void ClearVibraionClip();
		virtual void RunVibraion();

		virtual bool isConnected();
		virtual bool isRunning();
		virtual bool ClearCommand();


	private:
		float hmdRefreshRate;
		int m_TouchConnectType;
		int m_OldTouchConnectType = 0;

		HapticDeviceCommander* mCommader;	




	};

};