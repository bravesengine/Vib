#include "WaveToHaptic.h"

#include <QFileDialog>
#include <QDataStream>

#include "WaveBufferReader.h"
WaveToHaptic::WaveToHaptic()
{
	mBitPerSample = 0;
	mChannel = 0;
	mSampleRate = 0;

	mWaveEngine = new WaveEngine(this);
	
	ConnectEvent();

}

WaveToHaptic::~WaveToHaptic()
{
	if (mWaveEngine)
		delete mWaveEngine;

	mWaveEngine = NULL;
}

bool WaveToHaptic::LoadWaveFile(QString filePath)
{
	if (mWaveEngine == NULL)
		return false;

	QFileInfo info(filePath);
	if (!info.exists())
		return false;

	reset();

	bool bRet = mWaveEngine->loadFile(filePath);

	if(bRet)
		bRet = mWaveEngine->ReqGetBuffer();

	return bRet;
}

bool WaveToHaptic::LoadWaveFile_Dialog()
{
	if (mWaveEngine == NULL)
		return false;

	bool bRet = false;
	const QString dir;
	const QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open WAV file"), dir, "*.wav, *.mp3");

	if (fileNames.count())
		bRet = LoadWaveFile(fileNames.front());

	return bRet;
}

void WaveToHaptic::reset()
{
	mChannel = 0;
	mBitPerSample = 0;

	mDataBuffers[0].clear();
	mDataBuffers[1].clear();

	if(mWaveEngine)
		mWaveEngine->reset();
}


void WaveToHaptic::ConnectEvent()
{	
	//connect(mWaveEngine, &WaveEngine::stateChanged, this, &MainWidget::stateChanged);
	//connect(mWaveEngine, &WaveEngine::formatChanged,this, &MainWidget::formatChanged);
	//connect(mWaveEngine, &WaveEngine::bufferLengthChanged, this, &MainWidget::bufferLengthChanged);
	//connect(mWaveEngine, &WaveEngine::dataLengthChanged, this, &MainWidget::updateButtonStates);
	////m_progressBar->bufferLengthChanged(m_engine->bufferLength());
	////connect(mWaveEngine, &WaveEngine::recordPositionChanged, m_progressBar, &WaveProgressBar::recordPositionChanged);
	////connect(mWaveEngine, &WaveEngine::playPositionChanged, m_progressBar, &WaveProgressBar::playPositionChanged);
	//connect(mWaveEngine, &WaveEngine::recordPositionChanged, this, &MainWidget::audioPositionChanged);
	//connect(mWaveEngine, &WaveEngine::playPositionChanged, this, &MainWidget::audioPositionChanged);
	//connect(mWaveEngine, &WaveEngine::infoMessage, this, &MainWidget::infoMessage);
	//connect(mWaveEngine, &WaveEngine::errorMessage, this, &MainWidget::errorMessage);
	connect(mWaveEngine, &WaveEngine::ResGetbuffer, this, &WaveToHaptic::onResGetbuffer);
}

//const QAudioFormat &format,
bool WaveToHaptic::onResGetbuffer( const WavFile::wav_header_t &Header, qint64 length, const QByteArray &buffer)
{
	// Hz 단위로 표시합니다. (초당 Hz)
	int SampleRate = Header.sampleRate;// format.sampleRate();

	// 음원의 채널 갯수.
	int channel = Header.numChannels;// format.channelCount();

	// 샘플 하나당 사이즈 16Bit, 8Bit,.. 24bit.. 32bit 등등..
	int BitPerSample = Header.bitsPerSample;// format.sampleSize();

	int BlockByteAlign = Header.blockAlign;// CombinedHeader.wave.blockAlign;


	// 2채널 이상 지원안함.
	if (channel > 2)
		return false;

	mSampleRate = SampleRate;
	mBitPerSample = BitPerSample;
	mChannel = channel;
		
	mDataBuffers[0].clear();
	mDataBuffers[1].clear();

	int SamplesTotalCount = length / (mBitPerSample / 8);  // total samples count in audio data

	float TotPlayLenght = (float)length / (float)((SampleRate * mChannel * mBitPerSample) / 8);

	float ReadBytePos = 0;
	QDataStream data(buffer);
	QList<double> retValues;
	
	QSharedPointer< WaveBufferReader> Reader = WaveBufferReader::Create(Header, buffer, this);
	if (Reader.get() == NULL)
		return false;

	//-- 일단 다 읽어와서..
	while (ReadBytePos < length)
	{
		//-- normail
		for (int i = 0; i < mChannel; i++)
		{
			mDataBuffers[i].push_back(Reader.get()->ReadNormailze());
		}
		ReadBytePos += BlockByteAlign;
//		Reader.get()->skipRawData(SkipHapticByte- BlockByteAlign);
	}

	return true;
}

QList<quint8> WaveToHaptic::BufferToHaptic()
{
	QList<quint8> HapticRawBuffer;	
	try
	{
		if (mBitPerSample == 0 || mChannel > 2)
			throw("Invalid Format");

		if (mDataBuffers[0].size() <= 0)
			throw("Invalid Format");

		//float TotPlayLenght = (float)length / (float)((SampleRate * mChannel * mBitPerSample) / 8);
		// 320hz convert..

		int SkipIndex = (mSampleRate / 320.0) + 0.5;

		// 채널이 2개 이상일 경우 
		if (mChannel >= 2)
		{
			int BufferSize = mDataBuffers[0].size();
			for (int i = 0; i < BufferSize; i += SkipIndex)
			{
			   double left =  mDataBuffers[0].at(i);
			   double right = mDataBuffers[1].at(i);

			   HapticRawBuffer.push_back(abs( (left+ right) /2.0 ) * 255.0);
			}
		}
		else
		{
			for (int i = 0; i < mDataBuffers[0].size(); i += SkipIndex)
			{
				//-- 0 ~ 255 변환
				HapticRawBuffer.push_back(abs(mDataBuffers[0].at(i)) * 255.0);
			}
		}
	}
	catch (...)
	{
		return QList<quint8>();
	}

	return HapticRawBuffer;	
}