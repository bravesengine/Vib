#include "HapticDeviceCommand.h"
#include "HapticDeviceCommander.h"
#include "HapticClipResouce.h"
#include "HapticBufferClip.h"
#include <QtCore>

using namespace HT;

//////////////////////////////////////////////////////////////////////////////
//
//
HapticDeviceCommand_Run::HapticDeviceCommand_Run(HapticDeviceCommander* ParentCommander) : HapticDeviceCommand(ParentCommander)
{
	qDebug() << "AddCommand Run";
	mCurPlayClip = NULL;
	mTotPlayClipSize = 0;
	mCurPlayClipIndex = 0;
	mRePlayCount = 0;
	mbStop = false;
}

HapticDeviceCommand_Run::~HapticDeviceCommand_Run()
{

	

}

void HapticDeviceCommand_Run::RunCommand(QString desc)
{
	mCurPlayClip = NULL;

	if (HapticClipResouce::instance()->mClips.size() == 0)
	{
		mbIsFinised = true;
		return;
	}

	mTotPlayClipSize = HapticClipResouce::instance()->mClips.size();
	qDebug() << desc;
	qDebug() << "HapticDeviceCommand_Run - Total ClipSize = " << mTotPlayClipSize << "CurClipIndex =" << mCurPlayClipIndex << "RePlayCount = " << mRePlayCount;

	if (mCurPlayClipIndex > mTotPlayClipSize)
	{
		qDebug() << "Run Haptic!! Error Over ClipIndex";
		mbIsFinised = true;
		return;
	}

	mCurPlayClip = HapticClipResouce::instance()->mClips[mCurPlayClipIndex];
	mCurPlayClip->Run(mParentCommander->getSession());
	
}

void HapticDeviceCommand_Run::UpdateFrame_InValid(double interval)
{
	if (mCurPlayClip == NULL)
		return;
	
	mCurPlayClip->UpdateFrame_InValid(interval);

	if (mCurPlayClipIndex+1 == HapticClipResouce::instance()->mClips.size())
	{
		if (!mCurPlayClip->isRunning() && !HapticDeviceManager::instance()->IsReplay())
			mbIsFinised = true;
	}

	//if (mCurPlayClip->isRunning())
	//	return;
}

void HapticDeviceCommand_Run::UpdateFrame_Valid(double interval)
{
	if (mCurPlayClip == NULL)
		return;

	if (mbStop)
	{
		mCurPlayClip->Stop(mParentCommander->getSession());
		mbIsFinised = true;
		return;
	}

	mCurPlayClip->UpdateFrame_Valid(interval, mParentCommander->getSession());
	if (mCurPlayClip->isRunning())
		return;

	if (mCurPlayClipIndex +1  < HapticClipResouce::instance()->mClips.size())
	{
		mCurPlayClipIndex++;
		RunCommand("HapticDeviceCommand_Run -  UpdateFrame_Valid");
	}
	else if (HapticDeviceManager::instance()->IsReplay())
	{		
		RunCommand("HapticDeviceCommand_Run - Repaly UpdateFrame_Valid ");
		mRePlayCount++;
		HapticClipResouce::instance()->Reset();
		mCurPlayClipIndex = 0;		
	}
	else
	{
		mbIsFinised = true;
	}
}


//void HapticDeviceCommand_Run::Update(double interval)
//{
//	
//}

void HapticDeviceCommand_Run::Destory()
{
	qDebug() << "Destory Run Command";
}


void HapticDeviceCommand_Run::Stop()
{
	qDebug() << "HapticDeviceCommand_Run -  StopPlay!!";
	mbStop = true;
	////ovrHapticsPlaybackState playbackState;
	////-- 이걸로 정지 하면되나?? .. @_@..
	////ovrResult result = ovr_SetControllerVibration(*mSession, ovrControllerType::ovrControllerType_Touch, 0, 0);

	//mbIsFinised = true;
}



//////////////////////////////////////////////////////////////////////////////
//
//
HapticDeviceCommand_Stop::HapticDeviceCommand_Stop(HapticDeviceCommander* ParentCommander) : HapticDeviceCommand(ParentCommander)
{
	qDebug() << "AddCommand Stop";
}

HapticDeviceCommand_Stop::~HapticDeviceCommand_Stop()
{

}

void HapticDeviceCommand_Stop::RunCommand(QString desc)
{	
	mParentCommander->StopVibraiin();
	mbIsFinised = true;
}

void HapticDeviceCommand_Stop::UpdateFrame_InValid(double interval)
{
}

void HapticDeviceCommand_Stop::UpdateFrame_Valid(double interval)
{

}

void HapticDeviceCommand_Stop::Destory()
{
	qDebug() << "Destory Stop Command";
}

//////////////////////////////////////////////////////////////////////////////
//
//
HapticDeviceCommand_Clear::HapticDeviceCommand_Clear(HapticDeviceCommander* ParentCommander) : HapticDeviceCommand(ParentCommander)
{
	qDebug() << "AddCommand Clear";
}

HapticDeviceCommand_Clear::~HapticDeviceCommand_Clear()
{
	

}

void HapticDeviceCommand_Clear::RunCommand(QString desc)
{
	HapticClipResouce::instance()->Clear();
	mbIsFinised = true;
}

void HapticDeviceCommand_Clear::UpdateFrame_InValid(double interval)
{
}

void HapticDeviceCommand_Clear::UpdateFrame_Valid(double interval)
{

}


void HapticDeviceCommand_Clear::Destory()
{
	qDebug() << "Destory Clear Command";
}

//////////////////////////////////////////////////////////////////////////////
//
HapticDeviceCommand_InsertClip::HapticDeviceCommand_InsertClip(HapticBufferClip* Clip, HapticDeviceCommander* ParentCommander) : HapticDeviceCommand(ParentCommander)
{
	mClip = Clip;
	mParentCommander = ParentCommander;
}

HapticDeviceCommand_InsertClip::~HapticDeviceCommand_InsertClip()
{
	

	Destory();
}

void HapticDeviceCommand_InsertClip::RunCommand(QString desc)
{	
	HapticClipResouce::instance()->AppendClip(mClip);
//	mParentCommander->AppendClip(mClip);	
	mbIsFinised = true;
	mClip = NULL;
}

void HapticDeviceCommand_InsertClip::UpdateFrame_InValid(double interval)
{
}

void HapticDeviceCommand_InsertClip::UpdateFrame_Valid(double interval)
{

}

void HapticDeviceCommand_InsertClip::Destory()
{
	qDebug() << "Destory Insert Command";

	if (mClip != NULL)
		delete mClip;
	mClip = NULL;
}
