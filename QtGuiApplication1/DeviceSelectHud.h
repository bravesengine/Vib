#pragma once

#include <QWidget>
#include <QDialog>
#include <ui_Device.h>
#include "Device/HapticDeviceManager.h"

#include "HT_GlobalTypes.h"

class DeviceStateProgressHud;

class DeviceSelectHud : public QDialog
{
	Q_OBJECT

public:
	
	DeviceSelectHud(QWidget *parent = Q_NULLPTR);
	~DeviceSelectHud();

	enum eSelectedDeviceType
	{
		OVR_TOUCH_L,
		OVR_TOUCH_R,		
		X_INPUT,
		NONE,
	};

	typedef struct _DeviceButton
	{
		int Index;

		QToolButton *toolButton;

		QPixmap SelectImg;
		QPixmap DeSelectImg;

		QLabel* StateLabel;
		QString StateIcon_Connect;
		QString StateIcon_DisConnect;
	}DEVICE_BUTTON;

	static eSelectedDeviceType mCurSelectDeviceType;

private:
	Ui::Device ui;

	void setUpDeviceBtn(int Index, QToolButton *toolButton, QLabel* StateLabel,
		QString OffImageName, QString OnImageName, QString StateIcon_Connect_Name, QString StateIcon_DisConnect_Name);

	QMap<int,DEVICE_BUTTON*> mDeviceBtns;

	DeviceStateProgressHud* mStatePrgoressHud;



private slots:
	void clicked_device_left();
	void clicked_device_right();
	void clicked_device_gamepad();
private:


	void UpdateDeviceBtn(int Index, bool bChekable);
	void UpdateDeviceStateDesc(QLabel *label,QString IconName, QString Desc);


	void OnDeivceChanageState(const HT::HapticDeviceState* state, const QVariantList &vList);

	HT::EVENT_SIGNAL m_EventSignal;

private:
	
	typedef struct
	{
		HT::HapticDeviceState::eStateType ChangeState;
		QVariantList Values;

	} DEVICE_CHANGED_DATA;

	QList<DEVICE_CHANGED_DATA*> mChangeDeviceStateBuffer;

	QTimer* mDeviceStateUpdateTimer;

private slots:
	void UpdateDeviceState();
};
