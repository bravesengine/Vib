#pragma once

#include <QWidget>
#include "singleton.h"

namespace HT
{
	class HapticBufferClip;
	class HapticClipResouce : public QObject
	{
		Q_OBJECT

		friend class HapticDeviceCommand_InsertClip;
		friend class HapticDeviceCommand_Clear;
		friend class HapticDeviceCommand_Stop;
		friend class HapticDeviceCommand_Run;

	public:
		HapticClipResouce();
		~HapticClipResouce();

		//--use sample
		static HapticClipResouce* instance()
		{
			return Singleton<HapticClipResouce>::instance(HapticClipResouce::createInstance);
		}

	private:
		static HapticClipResouce* createInstance();

		QList< HapticBufferClip*> mClips;
		void AppendClip(HapticBufferClip* Clip);
		void Clear();
		void Reset();

	};
};
