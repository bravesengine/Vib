#include "HapticIDevice.h"
#include "HapticBufferClip.h"

using namespace HT;


HapticIDevice::HapticIDevice()
{
	mState = NULL;
}

HapticIDevice::~HapticIDevice()
{

	if (mState)
		delete mState;

	mState = NULL;
}


