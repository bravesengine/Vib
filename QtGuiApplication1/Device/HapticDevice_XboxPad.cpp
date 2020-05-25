#include "HapticDevice_XboxPad.h"
#include <QtCore>
//#include "QtGamepad/private/qgamepadbackend_p.h"


using namespace HT;

HapticDevice_XboxPad::HapticDevice_XboxPad()
{	
	

}

HapticDevice_XboxPad::~HapticDevice_XboxPad()
{	
}

int HapticDevice_XboxPad::Initize()
{
	auto gamepads = QGamepadManager::instance()->connectedGamepads();
	if (gamepads.isEmpty()) {
		qDebug() << "Did not find any connected gamepads";
		return 0;
	}
	
	qDebug() << "Number of gamepads:" << gamepads.size();

	for (auto i : gamepads) {

		QGamepad *gamepad = new QGamepad(i);
		qDebug() << "Gamepad:" << i;
		qDebug() << "  device id:   " << gamepad->deviceId();
		qDebug() << "  name:        " << gamepad->name();
		qDebug() << "  is connected?" << gamepad->isConnected();
	}

	//- 이게 QGameoadManagerPrivate 인데 ... 얻어올수가 없네 ㅡ,.ㅡ..
	//Q//GamepadManager::instance()->parent();


	//QGamepadManager::instance()->In
	//QGamepadBackend *backkend = NULL;
	//QGamepadManager
	return 1;

}

//Connections{
//		target: GamepadManager
//		onGamepadConnected : gamepad.deviceId = deviceId
//}
//
//Gamepad{
//	id: gamepad
//	deviceId : GamepadManager.connectedGamepads.length > 0 ? GamepadManager.connectedGamepads[0] : -1
//}
//

//-- xboxpad 진동세기 0~ 65535 값..

//XINPUT_VIBRATION structure
//‎12‎ / ‎05‎ / ‎2018
//2 minutes to read
//Specifies motor speed levels for the vibration function of a controller.
//Syntax
//C++
//
//Copy
//typedef struct _XINPUT_VIBRATION {
//	WORD wLeftMotorSpeed;
//	WORD wRightMotorSpeed;
//} XINPUT_VIBRATION, *PXINPUT_VIBRATION;
//Members
//wLeftMotorSpeed
//Speed of the left motor.Valid values are in the range 0 to 65, 535. Zero signifies no motor use; 65, 535 signifies 100 percent motor use.
//wRightMotorSpeed
//Speed of the right motor.Valid values are in the range 0 to 65, 535. Zero signifies no motor use; 65, 535 signifies 100 percent motor use.



///////////////////////////////////////////////////////////////////////////////
//
//
//
bool XBoxDeviceState::isVaild() const
{
	return false;
}

QString XBoxDeviceState::getStateMessage() const
{
	return "";
}
