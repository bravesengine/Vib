#include "DeviceSelectHud.h"

#include "DeviceStateProgressHud.h"

#include <QtWidgets>

#include "HapticDevice_Oculus.h"
#include "HapticDevice_XboxPad.h"

#include "QtHapticMain.h"
#include "HapticEditorWindowHud.h"
#include "OVR_CAPI.h"

using namespace HT;


static QMutex DeviceStateChangeMutex;

DeviceSelectHud::eSelectedDeviceType DeviceSelectHud::mCurSelectDeviceType = eSelectedDeviceType::NONE;

static QPixmap CreatePixmap(QString fileName)
{
	QDir dir(".");
	QPixmap pixmap = QPixmap(dir.absolutePath() + "/Resources/" + fileName);
	//int w = pixmap.width();
	//int h = pixmap.height();
	return pixmap;
}

void DeviceSelectHud::setUpDeviceBtn(int Index, QToolButton *toolButton, QLabel* StateLabel, 
	QString OffImageName, QString OnImageName,
	QString StateIcon_Connect_Name, QString StateIcon_DisConnect_Name)
{
	if (toolButton == NULL)
		return;

	if (mDeviceBtns.contains(Index))
		return;
	DEVICE_BUTTON* deviceBtn = new DEVICE_BUTTON();

	deviceBtn->SelectImg = CreatePixmap(OnImageName);
	deviceBtn->DeSelectImg = CreatePixmap(OffImageName);

	deviceBtn->Index = Index;
	deviceBtn->toolButton = toolButton;
	deviceBtn->StateLabel = StateLabel;
	deviceBtn->StateIcon_Connect = StateIcon_Connect_Name;
	deviceBtn->StateIcon_DisConnect = StateIcon_DisConnect_Name;

	QIcon ico;
	ico.addPixmap(deviceBtn->DeSelectImg, QIcon::Normal, QIcon::On);
	//ico.addPixmap(toggle_off_pixImg, QIcon::Normal, QIcon::Off);

	toolButton->setIcon(ico);
	toolButton->setIconSize(deviceBtn->DeSelectImg.rect().size());
	toolButton->setCheckable(false);

	QColor col = QColor(51, 51, 51, 255);
	//QString qss = QString("background-color: %1").arg(col.name());

	//toolButton->setStyleSheet("QToolButton { \
 //       background: rgb(51, 51, 51);\
 //       color: rgb(51, 51, 51);\
 //       text-align: center;\
 //       font-size: 12pt;\
 //       border: none;\
 //   }\
 //       QToolButton:checked {\
 //       background: rgb(51, 51, 51);\
 //   }\
 //   ");

	QVariant variant = QColor(qRgba(51, 51, 51, 255));
	QString colcode = variant.toString();
	QVariant variant2 = QColor(qRgba(128, 128, 128, 255));
	QString txtcol = variant2.toString();
	//StateLabel->setStyleSheet("QLabel { background-color :" + colcode + ";" + "color : " + txtcol + ";}");

	//StateIcon_DisConnect_Name

	StateLabel->setTextFormat(Qt::RichText);
	StateLabel->setText( QString("<img height=10 src= \"Resources/%1\"> DisConnect").arg(StateIcon_DisConnect_Name));// (Qt::RichText);
	//StateLabel->setText("¿¿ <img height=10  src= \"Resources/connect.png\"> DisConnect");// (Qt::RichText);

	mDeviceBtns.insert(Index, deviceBtn);
}

DeviceSelectHud::DeviceSelectHud(QWidget *parent) : QDialog(parent)
{


	ui.setupUi(this);
	
	mStatePrgoressHud = NULL;

	m_EventSignal =
		std::bind(&DeviceSelectHud::OnDeivceChanageState, this, std::placeholders::_1, std::placeholders::_2);
	//std::bind(&DeviceSelectHud::OnDeivceChanageState, this, std::placeholders::_1);// , std::placeholders::_2, std::placeholders::_3));



	setUpDeviceBtn(0, ui.toolButton_l, ui.touchState_l, "TouchLeft.png", "TouchLeft_on.png", "connect.png", "disconnect.png");
	setUpDeviceBtn(1, ui.toolButton_r, ui.touchState_r, "TouchRight.png", "TouchRight_on.png", "connect.png", "disconnect.png");
	setUpDeviceBtn(2, ui.toolButton_pad, ui.touchState_pad, "GamePad.png", "GamePad_on.png", "connect.png", "disconnect.png");

	connect(ui.toolButton_l, SIGNAL(clicked()), this, SLOT(clicked_device_left()));
	connect(ui.toolButton_r, SIGNAL(clicked()), this, SLOT(clicked_device_right()));
	connect(ui.toolButton_pad, SIGNAL(clicked()), this, SLOT(clicked_device_gamepad()));

	//-- timer..
	mDeviceStateUpdateTimer = new QTimer(this);
	connect(mDeviceStateUpdateTimer, SIGNAL(timeout()), this, SLOT(UpdateDeviceState()));
	mDeviceStateUpdateTimer->start(100);
}

DeviceSelectHud::~DeviceSelectHud()
{
	for (int i = 0; i < mDeviceBtns.size(); i++)
	{
		DEVICE_BUTTON* deviceBtn = mDeviceBtns[i];
		delete deviceBtn;
	}

	for (int i = 0; i < mChangeDeviceStateBuffer.size(); i++)
	{
		DEVICE_CHANGED_DATA* data = mChangeDeviceStateBuffer[i];
		delete data;
	}

	mChangeDeviceStateBuffer.clear();
	mDeviceBtns.clear();
}

void DeviceSelectHud::UpdateDeviceState()
{
	QMutexLocker locker(&DeviceStateChangeMutex);
	//qDebug() << "DeivceChanaged Buffer = " << mChangeDeviceStateBuffer.size();
	
	if (mChangeDeviceStateBuffer.size() == 0)
		return;

	DEVICE_CHANGED_DATA *changedData = mChangeDeviceStateBuffer[0];

	switch (changedData->ChangeState)
	{
	case HT::HapticDeviceState::eStateType::OVR_INIT_BEGIN:
		UpdateDeviceStateDesc(ui.touchState_l, "Progress.gif", "Serching Device");
		UpdateDeviceStateDesc(ui.touchState_r, "Progress.gif", "Serching Device");
		break;
	case HT::HapticDeviceState::eStateType::OVR_INIT_END:
		break;
	case HT::HapticDeviceState::eStateType::OVR_RE_INIT_BEGIN:
		break;
	case HT::HapticDeviceState::eStateType::OVR_RE_INIT_END:
		break;
	case HT::HapticDeviceState::eStateType::OVR_INIT_FAILED:
		UpdateDeviceStateDesc(ui.touchState_l, "disconnect.png", "Disconnect(Failed Initized)");
		UpdateDeviceStateDesc(ui.touchState_r, "disconnect.png", "Disconnect(Failed Initized)");
		break;
	case HT::HapticDeviceState::eStateType::OVR_DEVICE_HMD_NO_FOCUS:
		UpdateDeviceStateDesc(ui.touchState_l, "disconnect.png", "Rift Connect Waiting");
		UpdateDeviceStateDesc(ui.touchState_r, "disconnect.png", "Rift Connect Waiting");
		break;
	case HT::HapticDeviceState::eStateType::OVR_DEVICE_FOCUS_LOST:
		UpdateDeviceStateDesc(ui.touchState_l, "disconnect.png", "Disconnect");
		UpdateDeviceStateDesc(ui.touchState_r, "disconnect.png", "Disconnect");
		break;
	case HT::HapticDeviceState::eStateType::OVR_DEVICE_ERROR:
		UpdateDeviceStateDesc(ui.touchState_l, "disconnect.png", "Disconnect(Device Error)");
		UpdateDeviceStateDesc(ui.touchState_r, "disconnect.png", "Disconnect(Device Error)");
		break;
	case HT::HapticDeviceState::eStateType::OVR_DEVICE_STANDY:
	{
		UpdateDeviceStateDesc(ui.touchState_l, "disconnect.png", "Connect Waiting!!");
		UpdateDeviceStateDesc(ui.touchState_r, "disconnect.png", "Connect Waiting!!");
		break;
	}
	case HT::HapticDeviceState::eStateType::OVR_DEVICE_RUN:
	{
		QVariant v = changedData->Values.at(0);
		ovrControllerType_ ConnectType = (ovrControllerType_)v.value<int>();
		if (ConnectType == ovrControllerType_::ovrControllerType_Touch)
		{
			UpdateDeviceStateDesc(ui.touchState_l, "connect.png", "Connected");
			UpdateDeviceStateDesc(ui.touchState_r, "connect.png", "Connected");
		}
		else if (ConnectType == ovrControllerType_::ovrControllerType_LTouch)
		{
			UpdateDeviceStateDesc(ui.touchState_l, "connect.png", "Connected");
			UpdateDeviceStateDesc(ui.touchState_r, "disconnect.png", "Connect Waiting!!");
		}
		else if (ConnectType == ovrControllerType_::ovrControllerType_RTouch)
		{
			UpdateDeviceStateDesc(ui.touchState_l, "disconnect.png", "Connect Waiting!!");
			UpdateDeviceStateDesc(ui.touchState_r, "connect.png", "Connected");
		}
	}
	break;
	default:
		break;
	};

	mChangeDeviceStateBuffer.removeAt(0);
	delete changedData;
	changedData = NULL;
	//////////////////////////////////////////////////////////
}

void DeviceSelectHud::UpdateDeviceStateDesc(QLabel *label, QString IconName, QString Desc)
{
	if (label == NULL)
		return;

	QDir dir(".");
	QString path = dir.absolutePath() + "/Resources/" + IconName;
	QFileInfo fi(path);
	QString ext = fi.suffix();
	if (ext == "gif")
	{
		label->setTextFormat(Qt::RichText);
		label->setText(" ");

		QMovie *mv = new QMovie(path);
		mv->start();
		label->setAttribute(Qt::WA_NoSystemBackground);
		label->setMovie(mv);
	}
	else
	{
		label->setMovie(NULL);
		label->setTextFormat(Qt::RichText);
		label->setText(QString("<img height=10 src= \"Resources/%1\"> %2").arg(IconName).arg(Desc));// (Qt::RichText);
	}

}

void DeviceSelectHud::UpdateDeviceBtn(int Index, bool bChekable)
{
	for each (DEVICE_BUTTON* var in mDeviceBtns)
	{
		QIcon ico;
		if (var->Index != Index)
		{
			if (!var->toolButton->isCheckable())
				continue;

			var->toolButton->setCheckable(false);
		}
		else
		{
			var->toolButton->setCheckable(bChekable);
		}

		if (var->toolButton->isCheckable())
		{
			ico.addPixmap(var->SelectImg, QIcon::Normal, QIcon::On);
			var->toolButton->setIcon(ico);
			var->toolButton->setIconSize(var->SelectImg.rect().size());
		}
		else
		{
			ico.addPixmap(var->DeSelectImg, QIcon::Normal, QIcon::On);
			var->toolButton->setIcon(ico);
			var->toolButton->setIconSize(var->DeSelectImg.rect().size());
		}
	}
}

void DeviceSelectHud::OnDeivceChanageState(const HT::HapticDeviceState* state, const QVariantList &vList)
{

	QMutexLocker locker(&DeviceStateChangeMutex);

	HapticDeviceState::eStateType StateType = state->getState();

	QMetaEnum metaEnum = QMetaEnum::fromType<HapticDeviceState::eStateType>();
	qDebug() << "OnDeivceChanageState = " << metaEnum.valueToKey(StateType) << "ValueSize =" << vList.size();

	DEVICE_CHANGED_DATA *changedData = new DEVICE_CHANGED_DATA();
	changedData->ChangeState = StateType;
	changedData->Values = vList;
	mChangeDeviceStateBuffer.push_back(changedData);
}



void DeviceSelectHud::clicked_device_left()
{
	if (mStatePrgoressHud != NULL && mStatePrgoressHud->isActiveWindow())
		return;

	if (ui.toolButton_l->isCheckable()) return;

	bool bCheckable = !ui.toolButton_l->isCheckable();

	UpdateDeviceBtn(0, bCheckable);
	//---

	if (bCheckable)
		mCurSelectDeviceType = eSelectedDeviceType::OVR_TOUCH_L;

	QtHapticMain* hapticMainHud = static_cast< QtHapticMain* >(this->parent()) ;
	if (hapticMainHud == NULL)
	{
		qDebug() << "QtHapticMain Casting Failed!!";
		return;
	}

	HapticEditorWindowHud* editorHud = QtHapticMain::Instance()->getEditorHud();
	HT::HPLAYER_EVENT_SIGNAL  playerSignal = editorHud->getEventSignal();

	HapticDeviceManager::instance()->RunDevice(HapticDeviceManager::eDeviceType::OCULUS_CONTROLLER, m_EventSignal, playerSignal);
}

void DeviceSelectHud::clicked_device_right()
{
	if (mStatePrgoressHud != NULL && mStatePrgoressHud->isActiveWindow())
		return;
	if (ui.toolButton_r->isCheckable()) return;

	bool bCheckable = !ui.toolButton_r->isCheckable();
	UpdateDeviceBtn(1, bCheckable);

	if (bCheckable)
		mCurSelectDeviceType = eSelectedDeviceType::OVR_TOUCH_R;

	QtHapticMain* hapticMainHud = static_cast<QtHapticMain*>(this->parent());
	if (hapticMainHud == NULL)
	{
		qDebug() << "QtHapticMain Casting Failed!!";
		return;
	}

	HapticEditorWindowHud* editorHud = QtHapticMain::Instance()->getEditorHud();
	HT::HPLAYER_EVENT_SIGNAL  playerSignal = editorHud->getEventSignal();

	HapticDeviceManager::instance()->RunDevice(HapticDeviceManager::eDeviceType::OCULUS_CONTROLLER, m_EventSignal, playerSignal);
}

void DeviceSelectHud::clicked_device_gamepad()
{
	if (mStatePrgoressHud != NULL && mStatePrgoressHud->isActiveWindow())
		return;
	if (ui.toolButton_pad->isCheckable()) return;

	bool bCheckable = !ui.toolButton_pad->isCheckable();
	UpdateDeviceBtn(2, !ui.toolButton_pad->isCheckable());

	if (bCheckable)
		mCurSelectDeviceType = eSelectedDeviceType::X_INPUT;

	HapticEditorWindowHud* editorHud = QtHapticMain::Instance()->getEditorHud();
	HT::HPLAYER_EVENT_SIGNAL  playerSignal = editorHud->getEventSignal();

	HapticDeviceManager::instance()->RunDevice(HapticDeviceManager::eDeviceType::XBOXPAD_CONTROLLER, m_EventSignal, playerSignal);
}

//class MyButton : public QPushButton {
//public:
//	explicit MyButton(QWidget* parent = nullptr) : QPushButton(parent) {}
//	virtual ~MyButton() {}//
//	void setPixmap(const QPixmap& pixmap) { m_pixmap = pixmap; }

//	virtual QSize sizeHint() const override {
//		const auto parentHint = QPushButton::sizeHint();
//		// add margins here if needed
//		return QSize(parentHint.width() + m_pixmap.width(), std::max(parentHint.height(), m_pixmap.height()));
//	}
//
//protected:
//	virtual void paintEvent(QPaintEvent* e) override {
//		QPushButton::paintEvent(e);
//
//		if (!m_pixmap.isNull()) {
//			const int y = (height() - m_pixmap.height()) / 2; // add margin if needed
//			QPainter painter(this);
//			painter.drawPixmap(5, y, m_pixmap); // hardcoded horizontal margin
//		}
//	}
//
//private:
//	QPixmap m_pixmap;
//};