#pragma once

#include <OVR_CAPI.h>
#include <QWidget>
#include "HapticIDevice.h"

namespace HT
{
	class HapticDeviceCommand;
	class HapticBufferClip;

	class HapticDeviceCommander
	{
		friend class HapticDevice_Oculus;
		friend class HapticDeviceCommand_InsertClip;
		friend class HapticDeviceCommand_Clear;
		friend class HapticDeviceCommand_Run;
		friend class HapticDeviceCommand_Stop;

	public:
		HapticDeviceCommander();
		~HapticDeviceCommander();

		int CommandSize() {
			return mCommandLists.size();
		}

		virtual void* getSession() = 0;

		virtual bool IsRunningVibraion();

	private:
		//void    RunCommand();
		void	UpdateCommand_Frame_InValid(double interval, int state);
		void	UpdateCommand_Frame_Valid(double interval, int state);

		bool	AddCommnad(int CommadType);

		bool	ClearCommand();

		virtual void InsertVibraionClip(QByteArray &RawData, int subControllerIndex, double timelength)=0;	
		virtual void StopVibraiin() =0;

	protected:		
		QList< HapticDeviceCommand*> mCommandLists;
		HapticDeviceCommand* mCurCommand;

	//private:
	};

	class HapticDeviceCommander_Oculus : public HapticDeviceCommander
	{
	public:
		HapticDeviceCommander_Oculus(ovrSession* session);
		~HapticDeviceCommander_Oculus();

		virtual void* getSession() {
			return mSession;
		}

	


	private:
		virtual void InsertVibraionClip(QByteArray &RawData, int subControllerIndex, double timelength);
		virtual void StopVibraiin();

		ovrSession* mSession = NULL;
	};
	
};
