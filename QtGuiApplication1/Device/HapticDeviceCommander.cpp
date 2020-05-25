#include "HapticDeviceCommander.h"
//#include "HapticIDevice.h"
#include "HapticBufferClip.h"
#include "HapticBufferClip_Oculus.h"
#include "HapticDeviceCommand.h"

#include <QtCore>
#include "OVR_CAPI.h"

using namespace HT;

HapticDeviceCommander::HapticDeviceCommander()
{
	//mCurClip = NULL;
	mCurCommand = NULL;
}

HapticDeviceCommander::~HapticDeviceCommander()
{
	ClearCommand();
}

//void  HapticDeviceCommander::RunCommand()
//{

//}
void HapticDeviceCommander::UpdateCommand_Frame_Valid(double interval, int state)
{
	if (mCurCommand == NULL)
	{
		if (mCommandLists.size() == 0)
			return;

		mCurCommand = mCommandLists.at(0);
		mCurCommand->RunCommand( QString("HapticDeviceCommander UpdateCommand_Frame_Valid - RunCommand State = %1").arg(state));
	}

	mCurCommand->UpdateFrame_Valid(interval);

}

void HapticDeviceCommander::UpdateCommand_Frame_InValid( double interval, int state)
{
	if (mCurCommand == NULL)
		return;

	mCurCommand->UpdateFrame_InValid(interval);
	if (mCurCommand->IsFinished())
	{
		mCurCommand->Destory();
		mCommandLists.removeFirst();
		delete 	mCurCommand;
		mCurCommand = NULL;
	}
}

bool HapticDeviceCommander::IsRunningVibraion()
{
	if (mCurCommand == NULL)
		return false;

	HapticDeviceCommand_Run* RunCommand = qobject_cast<HapticDeviceCommand_Run*>(mCurCommand);
	if (RunCommand == NULL)
		return false;

	return true;
}


bool HapticDeviceCommander::AddCommnad(int CommadType)
{
	HapticDeviceCommand::eCommandType t = (HapticDeviceCommand::eCommandType) CommadType;
	
	HapticDeviceCommand* Command = NULL;
	switch (t)
	{
	case HT::HapticDeviceCommand::RUN:
	{
		Command = new HapticDeviceCommand_Run(this);
	}
	break;
	case HT::HapticDeviceCommand::STOP:
	{
	
		Command = new HapticDeviceCommand_Stop(this);
		break;
	}
	case HT::HapticDeviceCommand::CLEAR:
	{
	
		Command = new HapticDeviceCommand_Clear(this);
	}
	break;
	default:
		break;
	}

	if (Command == NULL)
		return false;

	mCommandLists.push_back(Command);
	return true;
}


bool HapticDeviceCommander::ClearCommand()
{
	if (mCommandLists.size() != 0)
	{
		qDebug() << "HapticDeviceCommander ClearCommand";

		for (int i = 0; i < mCommandLists.size(); i++)
		{
			HapticDeviceCommand *command = mCommandLists[i];
			command->Destory();
			delete command;
		}

		mCommandLists.clear();
	}

	mCurCommand = NULL;
	return true;
}

//void HapticDeviceCommander::Stop()
//{
//
//}


////////////////////////////////////////////////////////////////////////
//
//
HapticDeviceCommander_Oculus::HapticDeviceCommander_Oculus(ovrSession* session)
{
	mSession = session;
}

HapticDeviceCommander_Oculus::~HapticDeviceCommander_Oculus()
{

};

void HapticDeviceCommander_Oculus::InsertVibraionClip(QByteArray &RawData, int subControllerIndex, double timelength)
{
	qDebug() << "InsertVibraion Clip";
	HapticBufferClip_Oculus *Clip = new HapticBufferClip_Oculus(RawData, subControllerIndex, timelength);	
	mCommandLists.push_back(new HapticDeviceCommand_InsertClip(Clip, this));	
}

void HapticDeviceCommander_Oculus::StopVibraiin()
{
	if (mCurCommand == NULL)
		return;

	//qDebug() << "HapticDeviceCommander_Oculus StopVibraiin !!";
	HapticDeviceCommand_Run* RunCommand = qobject_cast<HapticDeviceCommand_Run*>(mCurCommand);
	if (RunCommand == NULL)
	{
		qDebug() << "HapticDeviceCommander_Oculus StopVibraiin Failed!!";
		return;
	}

	RunCommand->Stop();
}
