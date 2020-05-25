#include "HapticBufferClip.h"
#include <QtCore>

using namespace HT;

HapticBufferClip::HapticBufferClip(QByteArray &RawData, int SubControllerIndex, double timeLength)
{
	mRawData.append(RawData.constData(), RawData.size());

	mTimeLength = timeLength;
	mCurUpdateTime = 0;
	mSubControllerIndex = 0;
}

HapticBufferClip::~HapticBufferClip()
{
	mRawData.clear();

}
