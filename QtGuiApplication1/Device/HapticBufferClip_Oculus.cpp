#include "HapticBufferClip_Oculus.h"
#include <QtCore>

using namespace HT;

HapticBufferClip_Oculus::HapticBufferClip_Oculus(QByteArray &RawData, int SubControllerIndex, double timeLength)
	:HapticBufferClip(RawData, SubControllerIndex, timeLength)
{
	//mDataBuffer = NULL;
	//mTotBufferSize = 0;
	//mOvrBuffer.Samples = NULL;
	//mOvrBuffer.SamplesCount = 0;
	mSubControllerIndex = SubControllerIndex;
	mCurPlayBufferIndex = mOldPlayBufferIndex = 0;

	//--  size가 0 개여도 시간에 의해서 End..
	mTotBufferSize = RawData.size();

	double RealTotTime = 3.125 * mTotBufferSize;
	Q_ASSERT(RealTotTime == timeLength);

	mTimeLength = RealTotTime;


	if(mTotBufferSize <= 256)
	{
		OVR_RAW_BUFFER* RawBuffer = new OVR_RAW_BUFFER();
		RawBuffer->DataBuffer = new unsigned char[mTotBufferSize];
		RawBuffer->OvrBuffer.SubmitMode = ovrHapticsBufferSubmit_Enqueue;
		RawBuffer->OvrBuffer.SamplesCount = mTotBufferSize;
		
		RawBuffer->CurBufferTimeLength = 0;
		RawBuffer->UpdateBufferTimeLength = mTotBufferSize * 3.125;

		for(int i=0; i < mTotBufferSize; i++)
			RawBuffer->DataBuffer[i] = RawData[i];

		RawBuffer->OvrBuffer.Samples = (void *)RawBuffer->DataBuffer;
		mRawBuffers.push_back(RawBuffer);
	}
	else
	{
		//-- 0.8초식 Copy
		////-- 총 0.8초.. 3.125 * 256 ( 최대 햅틱 버퍼 사이즈는 256개)
		int Index = 0;
		while (true)
		{
			if (Index + 256 > mTotBufferSize)
				break;

			OVR_RAW_BUFFER* RawBuffer = new OVR_RAW_BUFFER();
			RawBuffer->DataBuffer = new unsigned char[256];
			RawBuffer->OvrBuffer.SubmitMode = ovrHapticsBufferSubmit_Enqueue;
			RawBuffer->OvrBuffer.SamplesCount = 256;
			RawBuffer->CurBufferTimeLength = 0;
			RawBuffer->UpdateBufferTimeLength = 256 * 3.125;

			for (int i = 0; i < 256; i++)
				RawBuffer->DataBuffer[i] = RawData[Index + i];

			RawBuffer->OvrBuffer.Samples = (void *)RawBuffer->DataBuffer;
			mRawBuffers.push_back(RawBuffer);

			Index += 256;
		}

		//--  나머지 버퍼.
		int RestBuffer = mTotBufferSize - Index;
		if (RestBuffer != 0)
		{
			OVR_RAW_BUFFER* RawBuffer = new OVR_RAW_BUFFER();
			RawBuffer->DataBuffer = new unsigned char[RestBuffer];
			RawBuffer->OvrBuffer.SubmitMode = ovrHapticsBufferSubmit_Enqueue;
			RawBuffer->OvrBuffer.SamplesCount = RestBuffer;
			RawBuffer->CurBufferTimeLength = 0;
			RawBuffer->UpdateBufferTimeLength = RestBuffer * 3.125;

			for (int i = 0; i < RestBuffer; i++)
				RawBuffer->DataBuffer[i] = RawData[Index + i];

			RawBuffer->OvrBuffer.Samples = (void *)RawBuffer->DataBuffer;
			mRawBuffers.push_back(RawBuffer);
		}
    }
	



}


HapticBufferClip_Oculus::~HapticBufferClip_Oculus()
{
	for (int i = 0; i < mRawBuffers.size(); i++)
	{
		OVR_RAW_BUFFER* RawBuffer = mRawBuffers[i];
		delete RawBuffer->DataBuffer;
		delete RawBuffer;
	}

	mRawBuffers.clear();
}

bool HapticBufferClip_Oculus::Stop(void* session)
{
	ovrSession *vrSession = static_cast<ovrSession*>(session);
	if (vrSession == NULL)
		return false;

	ovrControllerType vrControllerType = ovrControllerType::ovrControllerType_None;
	switch (mSubControllerIndex)
	{
	case 0: vrControllerType = ovrControllerType::ovrControllerType_LTouch; break;
	case 1: vrControllerType = ovrControllerType::ovrControllerType_RTouch; break;
	default:
		vrControllerType = ovrControllerType::ovrControllerType_None;
		break;
	}

	if (vrControllerType == ovrControllerType::ovrControllerType_None)
		return false;

	////-- 이걸로 정지 하면되나??
	//ovrResult result = ovr_SetControllerVibration(*vrSession, vrControllerType, 0, 0);
	//if (!OVR_SUCCESS(result))
	//	return false;
	//-- 이걸 실시간으로 0으로 만들어볼까?
	//ovrRawBuffer->OvrBuffer


	return true;
}

void HapticBufferClip_Oculus::UpdateFrame_InValid(double interval)
{
	mCurUpdateTime += interval;
	if (mbRunning)
	{
		HapticBufferClip_Oculus::OVR_RAW_BUFFER* RawBuffer = GetPlayBuffer();
		if (RawBuffer == NULL)
		{
			mbRunning = false;
		}
		else
		{
			//-- 나누어진 time별로 .. 또 해야되는군 ㅡㅡ; 습..
			RawBuffer->CurBufferTimeLength += interval;
			if (RawBuffer->CurBufferTimeLength >= RawBuffer->UpdateBufferTimeLength)
			{
				qDebug() << "HapticBufferClip_Oculus:UpdateFrame_InValid RawBuffer is End Index =" << mCurPlayBufferIndex;
				mCurPlayBufferIndex++;				
			}
		}
	}

	if (mCurUpdateTime >= mTimeLength)
	{
		qDebug() << "HapticBufferClip_Oculus:UpdateFrame_InValid Time End  ";
		//-- End??
		//-- 시간이 다됬으면 
		mbRunning = false;
	}
}

bool HapticBufferClip_Oculus::Reset()
{
	mCurPlayBufferIndex = 0;
	mOldPlayBufferIndex = 0;
	mCurUpdateTime = 0;

	qDebug() << "HapticBufferClip_Oculus PlayBufferIndex Reset!!";

	for (int i = 0; i < mRawBuffers.size(); i++)
		mRawBuffers[i]->CurBufferTimeLength = 0;

	return true;
}


void HapticBufferClip_Oculus::UpdateFrame_Valid(double interval, void* session)
{
	////-- 다음??
	if (mCurPlayBufferIndex == mOldPlayBufferIndex)
		return;

	mOldPlayBufferIndex = mCurPlayBufferIndex;
	ovrSession *vrSession = static_cast<ovrSession*>(session);
	if (vrSession == NULL)
	{
		mbRunning = false;
		return;
	}

	//-- NULL이 나와야 정상인데 됐다는건뒤??
	OVR_RAW_BUFFER* ovrRawBuffer = GetPlayBuffer();
	if (ovrRawBuffer == NULL)
	{
		mbRunning = false;
		return;
	}


	qDebug() << QString("HapticBufferClip_Oculus:UpdateFrame_Valid mCurPlayBufferIndex = %1 RawBufferSize = %2").
		arg(mCurPlayBufferIndex).
		arg(mRawBuffers.size());

	if (!Run(session))
		mbRunning = false;
}

bool HapticBufferClip_Oculus::isRunning()
{
	return mbRunning;
}

HapticBufferClip_Oculus::OVR_RAW_BUFFER* HapticBufferClip_Oculus::GetPlayBuffer()
{
	if (mRawBuffers.size() == 0)
		return NULL;

	if (mCurPlayBufferIndex >= mRawBuffers.size())
		return NULL;

	return mRawBuffers[mCurPlayBufferIndex];
}

bool HapticBufferClip_Oculus::Run(void* session)
{	
	ovrSession *vrSession = static_cast<ovrSession*>(session);
	if (vrSession == NULL)
		return false;

	//ovrControllerType_XBox

	ovrControllerType vrControllerType = ovrControllerType::ovrControllerType_None;
	switch (mSubControllerIndex)
	{
	case 0: vrControllerType = ovrControllerType::ovrControllerType_LTouch; break;
	case 1: vrControllerType = ovrControllerType::ovrControllerType_RTouch; break;
	default:
		vrControllerType = ovrControllerType::ovrControllerType_None;
		break;
	}

	if (vrControllerType == ovrControllerType::ovrControllerType_None)
		return false;

	ovrTouchHapticsDesc hapticdesc = ovr_GetTouchHapticsDesc(*vrSession, vrControllerType);
	if (hapticdesc.SampleSizeInBytes != 1)
	{
		qDebug() << "Our assumption of 1 byte per element, is no longer valid\n";
		return false;
	}

	OVR_RAW_BUFFER* ovrRawBuffer = GetPlayBuffer();
	if (ovrRawBuffer == NULL)
		return false;

	//mRawBuffers
	if (hapticdesc.SubmitMaxSamples < ovrRawBuffer->BufferLength)
	{
		qDebug() << "Can't handle this many samples";
		return false;
	}

	ovrHapticsPlaybackState playbackState;
	ovrResult result = ovr_GetControllerVibrationState(*vrSession, vrControllerType, &playbackState);
	if (!OVR_SUCCESS(result))
	{
		qDebug() << "ovr_GetControllerVibrationState Failed =" << result;

		return false;
	}

	if (playbackState.RemainingQueueSpace < ovrRawBuffer->BufferLength)
	{
		qDebug() << "RemainingQueueSpace QueueSpace =" << playbackState.RemainingQueueSpace << " PalyBufferSize = " << ovrRawBuffer->BufferLength;
		return false;
	}

	qDebug() << "ovr_SubmitControllerVibration type = " << vrControllerType;
	result =  ovr_SubmitControllerVibration(*vrSession, vrControllerType, &ovrRawBuffer->OvrBuffer);

	if (!OVR_SUCCESS(result))
	{
		qDebug() << "ovr_SubmitControllerVibration Failed =" << result;
		return false;
	}

	mbRunning = true;
	return true;
}
