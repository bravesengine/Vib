#include "qdebug.h"
#include "HapticClipResouce.h"
#include "HapticBufferClip.h"

using namespace HT;
HapticClipResouce::HapticClipResouce()
{

}

HapticClipResouce::~HapticClipResouce()
{

}

HapticClipResouce* HapticClipResouce::createInstance()
{
	return new HapticClipResouce();
}

void HapticClipResouce::AppendClip(HapticBufferClip* Clip)
{
	qDebug() << "HapticClipResouce Append!! Size = " << mClips.count();

	mClips.append(Clip);
}

void HapticClipResouce::Clear()
{
	qDebug() << "HapticClipResouce Clear!!";

	for (int i = 0; i < mClips.size(); i++)
	{
		HapticBufferClip *clip = mClips[i];
		delete clip;
	}
	mClips.clear();
}

void HapticClipResouce::Reset()
{
	qDebug() << "HapticClipResouce ResetAll PlayBuffer Index!!";

	for (int i = 0; i < mClips.size(); i++)
	{
		HapticBufferClip *clip = mClips[i];
		clip->Reset();
	}	
}
