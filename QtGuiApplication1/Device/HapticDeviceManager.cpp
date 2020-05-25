#include "HapticDeviceManager.h"
#include <QtCore>

#include "HapticIDevice.h"

#include "HapticDevice_XboxPad.h"
#include "HapticDevice_Oculus.h"
#include "HapticDevice_Vive.h"
#include "HapticBufferClip.h"

#include "qthread.h"

using namespace HT;

void DeviceUpdateThread::run()
{
	//-- 음 간격이 문제군..
	static unsigned long tickStep = 1000 / 40;
	while (1)
	{
		//qDebug("Thread::run");
		QThread::msleep(tickStep);
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 30));
		emit tick(tickStep);
	}
};

static DeviceUpdateThread* UpdateThread = NULL;
static QMutex DeviceUpdateMutex;
static QWaitCondition DeviceUpdateEnd;

HapticDeviceManager::HapticDeviceManager()
{
	UpdateThread = new DeviceUpdateThread();
	//UpdateThread->setTerminationEnabled(true);	
	connect(UpdateThread, SIGNAL(tick(double)), this, SLOT(UpdateDevice(double)));

	UpdateThread->start();
}

HapticDeviceManager::~HapticDeviceManager()
{	
	if (UpdateThread)
	{
		UpdateThread->terminate();
		UpdateThread->wait();
	}

	delete UpdateThread;
	UpdateThread = NULL;

	DEVICE_ITER i = mDevices.begin();
	while (i != mDevices.end())
	{
		HapticIDevice* device = i.value();
		delete device;
		++i;
	}
	mDevices.clear();
}

HapticDeviceManager* HapticDeviceManager::createInstance()
{
	return new HapticDeviceManager();
}

static bool isInitizeDevice = false;
void HapticDeviceManager::UpdateDevice(double interval)
{	
	QMutexLocker locker(&DeviceUpdateMutex);

	if (isInitizeDevice)
		DeviceUpdateEnd.wait(&DeviceUpdateMutex);

	DEVICE_CONST_ITER i = mDevices.constBegin();
	while (i != mDevices.constEnd())
	{
		i.value()->Update(interval);	

		if (!i.value()->isConnected())
		{
			i.value()->ClearCommand();
		}
		
		++i;
	}
}

HapticIDevice* HapticDeviceManager::CreateDevice(eDeviceType t, EVENT_SIGNAL signal, HPLAYER_EVENT_SIGNAL playerSignal)
{
	HapticIDevice* device = NULL;
	int Ret = 0;
	switch (t)
	{
	case HapticDeviceManager::OCULUS_CONTROLLER:
	{
		device = new HapticDevice_Oculus(signal, playerSignal);
	}
	break;
	case HapticDeviceManager::XBOXPAD_CONTROLLER:
		break;
	case HapticDeviceManager::VIVE__CONTROLLER:
		break;
	default:
		break;
	}
	return device;
}

int HapticDeviceManager::RunDevice(eDeviceType t, EVENT_SIGNAL signal, HPLAYER_EVENT_SIGNAL playerSignal)
{
	//-- 업데이트 중이면 return;
	QMutexLocker locker(&DeviceUpdateMutex);	
	isInitizeDevice = true;

	int Ret = 0;
	HapticIDevice* device = NULL;

	if (mDevices.contains(t))
	{
		device = mDevices[t];
		const HapticDeviceState *DeviceState = device->getState();
		if (!DeviceState->isVaild())
			goto End;

		Ret = device->reInitize(signal, playerSignal);
	}
	else
	{
		device = CreateDevice(t, signal, playerSignal);
		if (device == NULL)
			goto End;

		Ret = device->Initize();
		mDevices.insert(t, device);
	}
End:
	DeviceUpdateEnd.wakeAll();
	isInitizeDevice = false;

	return Ret;
}

bool HapticDeviceManager::IsRunning(eDeviceType t, int ControllerIndex)
{
	HapticIDevice* device = NULL;
	if (!mDevices.contains(t))
		return false;

	device = mDevices[t];
	return device->isRunning();
}

bool HapticDeviceManager::IsRunning()
{
	DEVICE_CONST_ITER i = mDevices.constBegin();
	while (i != mDevices.constEnd())
	{
		if (i.value()->isRunning())
			return true;
		++i;
	}

	return false;
}

bool HapticDeviceManager::isConnected(eDeviceType t)
{
	HapticIDevice* device = NULL;
	if (!mDevices.contains(t))
		return false;
	
	device = mDevices[t];
	return device->isConnected();
}



void HapticDeviceManager::ClearVibraionClip(eDeviceType t)
{
	HapticIDevice* device = NULL;
	if (!mDevices.contains(t))
		return;

	device = mDevices[t];
	device->ClearVibraionClip();
}

void HapticDeviceManager::InsertVibraionClip(eDeviceType t, int subControllerIndex,  QByteArray &RawData, double timelength)
{
	HapticIDevice* device = NULL;
	if (!mDevices.contains(t))
		return;

	device = mDevices[t];
	device->InsertVibraionClip(RawData, subControllerIndex, timelength);
}

void HapticDeviceManager::StopVibraion(eDeviceType t)
{
	HapticIDevice* device = NULL;
	if (!mDevices.contains(t))
		return;
	device = mDevices[t];
	device->StopVibraionAll();
}

void HapticDeviceManager::RunVibraion(eDeviceType t)
{
	HapticIDevice* device = NULL;
	if (!mDevices.contains(t))
		return;

	device = mDevices[t];
	device->RunVibraion();

	//device->StopVibraionAll();
	//device->StopVibraionAll();
}

