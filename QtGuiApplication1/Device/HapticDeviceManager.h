#pragma once

#include <QWidget>
#include "singleton.h"
#include "qmap.h"

#include "HapticDeviceState.h"

#include "HT_GlobalTypes.h"


namespace HT
{
	class DeviceUpdateThread : public QThread
	{
		Q_OBJECT
	public:
		DeviceUpdateThread() : QThread()
		{

		}
	signals:
		void tick(double interval);
	protected:
		void run();
	};

	class HapticIDevice;
	class HapticDeviceManager : public QObject
	{
		Q_OBJECT
	public:
		HapticDeviceManager();
		~HapticDeviceManager();

		enum eDeviceType
		{
			OCULUS_CONTROLLER,
			XBOXPAD_CONTROLLER,
			VIVE__CONTROLLER,
			NONE,
		};

		//--use sample
		static HapticDeviceManager* instance()
		{
			return Singleton<HapticDeviceManager>::instance(HapticDeviceManager::createInstance);
		}

		

		bool IsRunning(eDeviceType t, int ControllerIndex);
		bool IsRunning();
		bool isConnected(eDeviceType t);

		int RunDevice(eDeviceType t, EVENT_SIGNAL signal, HPLAYER_EVENT_SIGNAL playerSignal);


		void InsertVibraionClip(eDeviceType t, int subControllerIndex, QByteArray &RawData, double timelength);
		void RunVibraion(eDeviceType t);
		void StopVibraion(eDeviceType t);
		void ClearVibraionClip(eDeviceType t);

		void SetReplay(bool bRePlay)
		{
			mbReplay = bRePlay;
		}

		bool IsReplay() { return mbReplay; }


	private slots:
		void UpdateDevice(double interval);

	private:

		static HapticDeviceManager* createInstance();

		typedef QMap<eDeviceType, HapticIDevice*>::const_iterator DEVICE_CONST_ITER;
		typedef QMap<eDeviceType, HapticIDevice*>::iterator DEVICE_ITER;

		QMap<eDeviceType, HapticIDevice*> mDevices;

		HapticIDevice* CreateDevice(eDeviceType t, EVENT_SIGNAL signal, HPLAYER_EVENT_SIGNAL playerSignal);

		bool mbReplay = false;
	

	};
};
