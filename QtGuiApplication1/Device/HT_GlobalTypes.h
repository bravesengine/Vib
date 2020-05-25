#pragma once

#include <QWidget>
#include <QtCore>
namespace HT
{
	class HapticDeviceState;

	typedef  std::function<void(const HapticDeviceState*, const QVariantList &vList)> EVENT_SIGNAL;
	
	// 1st begin, stop.. end , update?? 정도인가??
	// 2st 플레이된 time..
	typedef  std::function<void(int, double)> HPLAYER_EVENT_SIGNAL;
};


