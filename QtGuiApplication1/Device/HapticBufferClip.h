#pragma once

#include <QWidget>
namespace HT
{
	class HapticBufferClip {

		friend class HapticDevice_Oculus;
		friend class HapticDeviceCommand_Run;
		friend class HapticClipResouce;
		//friend class HapticDeviceCommander_Oculus;

	public:
		HapticBufferClip(QByteArray &RawData, int SubControllerIndex, double timeLength);
		~HapticBufferClip();

		virtual bool isRunning() = 0;

	private:
		QByteArray mRawData;

		virtual void UpdateFrame_InValid(double interval) = 0;
		virtual	void UpdateFrame_Valid(double interval, void* session) = 0;

		virtual bool Run(void* session)=0;
		virtual bool Stop(void* session) = 0;
		virtual bool Reset() = 0;

	protected:
		double mTimeLength;
		double mCurUpdateTime;

		int mSubControllerIndex;
	};
};
