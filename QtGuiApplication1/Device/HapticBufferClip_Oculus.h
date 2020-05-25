#pragma once

#include <QWidget>
#include "HapticBufferClip.h"
#include <OVR_CAPI.h>
namespace HT
{
	class HapticBufferClip_Oculus : public HapticBufferClip
	{

		friend class HapticDevice_Oculus;

	public:
		HapticBufferClip_Oculus(QByteArray &RawData, int SubControllerIndex, double timeLength);
		~HapticBufferClip_Oculus();

		virtual bool isRunning();

	private:

		virtual void UpdateFrame_InValid(double interval);
		virtual	void UpdateFrame_Valid(double interval, void* session);

		virtual bool Run(void* session);
		virtual bool Stop(void* session);

		virtual bool Reset();

		bool mbRunning = false;

		typedef struct ovrRawBuffer
		{
			ovrHapticsBuffer OvrBuffer;
			unsigned char* DataBuffer;
			int BufferLength;

			double CurBufferTimeLength;
			double UpdateBufferTimeLength;

		} OVR_RAW_BUFFER;

		QList< OVR_RAW_BUFFER*> mRawBuffers;

		int mTotBufferSize;
		double mTimeLength;

	private:

		OVR_RAW_BUFFER* GetPlayBuffer();
		int mCurPlayBufferIndex = 0;
		int mOldPlayBufferIndex = 0;

	};
};
