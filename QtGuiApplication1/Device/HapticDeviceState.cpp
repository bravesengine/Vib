#include <QtGlobal>
#include "HapticDeviceState.h"
#include "HapticDeviceManager.h"

using namespace HT;

HapticDeviceState::HapticDeviceState(EVENT_SIGNAL signal, HPLAYER_EVENT_SIGNAL playerSignal)
{
	mState = eStateType::NONE;
	msignal = signal;
	mPlayerSignal = playerSignal;
	mbForceChange = false;
}

HapticDeviceState::~HapticDeviceState()
{	

}


void HapticDeviceState::SetForceChange()
{
	mbForceChange = true;
}

void HapticDeviceState::ChangeState(eStateType s, int Count, ...)
{
	if (s == mState && mbForceChange == false)
	{
		return;
	}

	mState = s;
	mbForceChange = false;

	try
	{
		if (msignal != NULL)
		{
			va_list ap;
			va_start(ap, Count);

			QVariantList vList;
			for (int i = 0; i < Count; i++)
			{
				QVariant v = va_arg(ap, QVariant);
				vList.append(v);
			}
			va_end(ap);
			
			msignal(this, vList);
		}
	}
	catch(...)
	{

	}
}
