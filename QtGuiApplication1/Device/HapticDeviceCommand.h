#pragma once

#include <QWidget>
namespace HT
{
	class HapticBufferClip;
	class HapticDeviceCommand : public QObject
	{
		Q_OBJECT

		friend class  HapticDeviceCommander;

	public:
		HapticDeviceCommand(HapticDeviceCommander* ParentCommander)
		{
			mParentCommander = ParentCommander;
		}

		~HapticDeviceCommand() {}

		enum eCommandType
		{
			RUN,
			STOP,
			CLEAR,
		};

	//static void CreateCommand

	virtual void RunCommand(QString desc) = 0;
	virtual void UpdateFrame_InValid(double interval) = 0;
	virtual void UpdateFrame_Valid(double interval) = 0;


	bool IsFinished() {
		return  mbIsFinised;
	}
	private:
		virtual void Destory() = 0;

	protected:
		bool mbIsFinised = false;
		HapticDeviceCommander* mParentCommander = NULL;
	};
	
	class HapticDeviceCommand_Run : public HapticDeviceCommand
	{
		Q_OBJECT
	public:
		HapticDeviceCommand_Run(HapticDeviceCommander* ParentCommander);
		~HapticDeviceCommand_Run();
		virtual void RunCommand(QString desc);
		virtual void UpdateFrame_InValid(double interval);
		virtual void UpdateFrame_Valid(double interval);

		void	Stop();
	private:
		virtual void Destory();

		HapticBufferClip* mCurPlayClip = NULL;

		int mTotPlayClipSize;
		int mCurPlayClipIndex;
		int mRePlayCount;
		bool mbStop;
	};

	class HapticDeviceCommand_Stop : public HapticDeviceCommand
	{
		Q_OBJECT
	public:
		HapticDeviceCommand_Stop(HapticDeviceCommander* ParentCommander);
		~HapticDeviceCommand_Stop();

		virtual void RunCommand(QString desc);
		virtual void UpdateFrame_InValid(double interval);
		virtual void UpdateFrame_Valid(double interval);

	private:
		virtual void Destory();
	};

	class HapticDeviceCommand_Clear : public HapticDeviceCommand
	{
		Q_OBJECT
	public:
		HapticDeviceCommand_Clear(HapticDeviceCommander* ParentCommander);
		~HapticDeviceCommand_Clear();

		virtual void RunCommand(QString desc);
		virtual void UpdateFrame_InValid(double interval);
		virtual void UpdateFrame_Valid(double interval);

	private:
		virtual void Destory();

	};

	class HapticDeviceCommand_InsertClip : public HapticDeviceCommand
	{
		Q_OBJECT
	public:
		HapticDeviceCommand_InsertClip(HapticBufferClip* Clip, HapticDeviceCommander* ParentCommander);
		~HapticDeviceCommand_InsertClip();

		virtual void RunCommand(QString desc);
		virtual void UpdateFrame_InValid(double interval);
		virtual void UpdateFrame_Valid(double interval);

	private:
		virtual void Destory();

		HapticBufferClip* mClip = NULL;
	

	};
}
