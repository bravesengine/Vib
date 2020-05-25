#include "HapticDevice_Oculus.h"
#include "HapticBufferClip.h"
#include "HapticBufferClip_Oculus.h"
#include "HapticDeviceCommander.h"
#include "HapticDeviceCommand.h"
#include <QtCore>

using namespace HT;

static int waitingForStatsMessagedCount = 0;// false;

static ovrSession mVrSession;
static ovrPerfStats lastPerfStats;
static ovrPerfStatsPerCompositorFrame *lastFrameStats;

HapticDevice_Oculus::HapticDevice_Oculus(EVENT_SIGNAL signal, HPLAYER_EVENT_SIGNAL playerSignal) : HapticIDevice()//, HapticDeviceCommander(this)
{
	mCommader = NULL;
	hmdRefreshRate = 0;
	mVrSession = NULL;
	m_TouchConnectType = 0;
	m_OldTouchConnectType = 0;
	mState = new  OculusDeviceState(signal, playerSignal);	
}

HapticDevice_Oculus::~HapticDevice_Oculus()
{	
	if (mCommader != NULL)
		delete mCommader;

	mCommader = NULL;

	if (mVrSession == NULL)
		return;

	ovr_Destroy(mVrSession);
	ovr_Shutdown();

	mVrSession = NULL;
}

int HapticDevice_Oculus::reInitize(EVENT_SIGNAL signal, HPLAYER_EVENT_SIGNAL playerSignal)
{
	return 0;
}

void HapticDevice_Oculus::InsertVibraionClip(QByteArray &RawData, int subControllerIndex, double timelength)
{
	if (RawData.size() == 0 || mCommader == NULL)
	{
		qDebug() << "HapticDevice_Oculus::InsertVibraionClip RawData Size is zero!!";
		return;
	}

	mCommader->InsertVibraionClip(RawData, subControllerIndex, timelength);
}

void HapticDevice_Oculus::StopVibraionAll()
{
	if (mCommader == NULL)
		return;

	//-- 만약에 뭔가 플레이중이라면?
	if (mCommader->IsRunningVibraion())
	{
		mCommader->StopVibraiin();
	}
	else
	{
		mCommader->AddCommnad(HapticDeviceCommand::STOP);
	}
}

void HapticDevice_Oculus::ClearVibraionClip()
{
	if (mCommader == NULL)
		return;
	mCommader->AddCommnad(HapticDeviceCommand::CLEAR);
}

void HapticDevice_Oculus::RunVibraion()
{
	if (mCommader == NULL)
		return;
	mCommader->AddCommnad(HapticDeviceCommand::RUN);
}

int HapticDevice_Oculus::Initize()
{
	if (mVrSession != NULL)
		return 0;

	waitingForStatsMessagedCount = 0;

	mState->ChangeState(HapticDeviceState::eStateType::OVR_INIT_BEGIN);
	ovrInitParams initParams = { ovrInit_Invisible | ovrInit_RequestVersion, OVR_MINOR_VERSION, NULL, 0, 0 };
	ovrResult result = ovr_Initialize(&initParams);
	// Failed to initialize libOVR			
	if (!OVR_SUCCESS(result))
		goto End;

	ovrGraphicsLuid luid;
	result = ovr_Create(&mVrSession, &luid);
	if (!OVR_SUCCESS(result))		
		goto End;

	ovrHmdDesc hmdDesc = ovr_GetHmdDesc(mVrSession);	
	hmdRefreshRate = hmdDesc.DisplayRefreshRate;
	lastFrameStats = &lastPerfStats.FrameStats[0]; // [0] contains the most recent stats
	lastFrameStats->HmdVsyncIndex = -1;  // reset to make sure we know it's not updated via the SDK yet
	lastPerfStats.VisibleProcessId = -1;	

End:
	if (!OVR_SUCCESS(result))
		mState->ChangeState(HapticDeviceState::eStateType::OVR_INIT_FAILED);

	if(mCommader == NULL)
		mCommader = new HapticDeviceCommander_Oculus(&mVrSession);

	mState->ChangeState(HapticDeviceState::eStateType::OVR_INIT_END);

	return result;
}

int HapticDevice_Oculus::Update(double interval)
{
	if (mVrSession == NULL)
		return 0;

	ovrPerfStats perfStats;
	// Main loop
	ovrSessionStatus sessionStatus;
	ovr_GetSessionStatus(mVrSession, &sessionStatus);
	if (sessionStatus.ShouldQuit || sessionStatus.DisplayLost || !sessionStatus.HmdPresent)
	{
		mState->ChangeState(HapticDeviceState::eStateType::OVR_DEVICE_ERROR);
		return 0;
	}

	if (mState->getState() == HapticDeviceState::eStateType::OVR_DEVICE_FOCUS_LOST)
		return 0;

	assert(!sessionStatus.IsVisible);
	// this session should never be visible since it's not submitting any frames
	//if (!sessionStatus.IsVisible)
	//	return 0;

	ovr_GetPerfStats(mVrSession, &perfStats);
	if (lastPerfStats.VisibleProcessId == -1)
	{
		
	}

	if (mCommader) mCommader->UpdateCommand_Frame_InValid(interval, mState->getState());

	// interval 최소 1프레임 이상... check
	if (perfStats.FrameStatsCount <= 0)
	{
		if (waitingForStatsMessagedCount < 1)//!waitingForStatsMessaged)
		{
			//qDebug() << QString("Waiting for performance stats... pID = %1  StateCount = %2 \n").arg(perfStats.VisibleProcessId).arg(perfStats.FrameStatsCount);
			mState->ChangeState(HapticDeviceState::eStateType::OVR_DEVICE_HMD_NO_FOCUS);
			// This can happen if there are no VR applications currently in focus, or if the HMD has been
			// static for a certain amount of time, causing the SDK to put the HMD display to sleep.
			//qDebug() << "Waiting for performance statsn";

			waitingForStatsMessagedCount++;
			//waitingForStatsMessaged = true;
		}
		// In this app, since we don't care about individual frame time values and only looking at frame rate,
		// we can update the frame rate once a second (i.e. 1000 ms) and look at the number of frames rendered.
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000 ));
		return 0;
	}

	// Did we process a frame before?
	if (lastFrameStats->HmdVsyncIndex > 0)
	{
		// Are we still looking at the same app, or did focus shift?
		if (lastPerfStats.VisibleProcessId == perfStats.VisibleProcessId)
		{
			int framesSinceLastInterval = perfStats.FrameStats[0].HmdVsyncIndex - lastFrameStats->HmdVsyncIndex;
			int appFramesSinceLastInterval = perfStats.FrameStats[0].AppFrameIndex - lastFrameStats->AppFrameIndex;
			int compFramesSinceLastInterval = perfStats.FrameStats[0].CompositorFrameIndex - lastFrameStats->CompositorFrameIndex;
			float appFrameRate = (float)appFramesSinceLastInterval / framesSinceLastInterval * hmdRefreshRate;
			float compFrameRate = (float)compFramesSinceLastInterval / framesSinceLastInterval * hmdRefreshRate;
			//qDebug() << QString("App PID: %1\tApp FPS: %2\tCompositor FPS: %3 StateCount = %4 \n").
			//	arg(perfStats.VisibleProcessId).arg(appFrameRate).arg(compFrameRate).arg(perfStats.FrameStatsCount);
		}
		else
		{
			//	printf("Focus shifted to another VR app. Resetting perf stats...\n");
			int result = ovr_ResetPerfStats(mVrSession);
			if (!OVR_SUCCESS(result))
			{
				//-- 다른 VR 어플이 실행되어서 포커스를 잃어버림..
				mState->ChangeState(HapticDeviceState::eStateType::OVR_DEVICE_FOCUS_LOST);
				//-- 종료..?
				return 0;
			}
		}
	}

	

	ovrInputState    inputState;
	m_TouchConnectType = ovr_GetConnectedControllerTypes(mVrSession);
	bool isConnected = (m_TouchConnectType & ovrControllerType_Touch) ? true : false;
	if (isConnected)
	{
		//m_TouchConnectType 바뀌어도 강제로 상태변경
		if (m_OldTouchConnectType != m_TouchConnectType)
			mState->SetForceChange();

		mState->ChangeState(HapticDeviceState::eStateType::OVR_DEVICE_RUN, 1, QVariant(m_TouchConnectType));
		
	}
	else
	{
		mState->ChangeState(HapticDeviceState::eStateType::OVR_DEVICE_STANDY, 1, QVariant(m_TouchConnectType));
	}


	if (mCommader) mCommader->UpdateCommand_Frame_Valid(interval, mState->getState());

	m_OldTouchConnectType = m_TouchConnectType;


	// save off values for next interval's calculations			
	waitingForStatsMessagedCount = 0;
	//waitingForStatsMessaged = false;
	lastPerfStats = perfStats;

	return 1;
}

bool HapticDevice_Oculus::isConnected()
{
	if (this->mState->getState() == HapticDeviceState::eStateType::OVR_DEVICE_RUN)
		return true;

	return false;
}

bool HapticDevice_Oculus::isRunning()
{
	if (mCommader == NULL)
		return false;

	if (mCommader->CommandSize() != 0)
		return true;

	return false;
}

bool HapticDevice_Oculus::ClearCommand()
{
	if (mCommader == NULL)
		return false;

	mCommader->ClearCommand();
	return false;
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
bool OculusDeviceState::isVaild() const
{
	if (mVrSession == NULL)
		return false;

	if(this->getState() == HapticDeviceState::eStateType::OVR_DEVICE_RUN ||
	   this->getState() == HapticDeviceState::eStateType::OVR_DEVICE_STANDY ||
	   this->getState() == HapticDeviceState::eStateType::OVR_DEVICE_HMD_NO_FOCUS)
			return true;

	return false;
}

QString OculusDeviceState::getStateMessage() const
{
	return "";
}
