#pragma once

#include <QWidget>
#include "HT_GlobalTypes.h"
namespace HT
{

	class HapticDeviceState : public QObject
	{
		Q_OBJECT
	public:
		HapticDeviceState(EVENT_SIGNAL signal, HPLAYER_EVENT_SIGNAL playerSignal);
		~HapticDeviceState();

		enum eStateType
		{
			NONE,
			OVR_INIT_BEGIN,
			OVR_INIT_END,
			OVR_RE_INIT_BEGIN,
			OVR_RE_INIT_END,
			OVR_INIT_FAILED,			
			OVR_DEVICE_HMD_NO_FOCUS, /// standy
			OVR_DEVICE_FOCUS_LOST,
			OVR_DEVICE_ERROR,
			OVR_DEVICE_STANDY,
			OVR_DEVICE_RUN,
		};

		Q_ENUM(eStateType)

		virtual bool isVaild() const = 0;
		virtual QString getStateMessage() const = 0;

		eStateType getState() const
		{
			return mState;
		}

		void ChangeState(eStateType s, int Count=0, ...);

		void SetForceChange();


protected:

		eStateType mState;
		EVENT_SIGNAL msignal;
		HPLAYER_EVENT_SIGNAL mPlayerSignal;
		bool mbForceChange;
	};

};