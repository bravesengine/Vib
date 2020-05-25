

#ifndef WAVETOHAPTIC_H
#define WAVETOHAPTIC_H

#include <QObject>
#include <QWidget>
#include <QtCore/QVector>
#include <QAudioFormat>

#include "waveengine.h"

class WaveToHaptic : public QWidget
{
	Q_OBJECT
public:
	explicit WaveToHaptic();
	~WaveToHaptic();

	bool LoadWaveFile_Dialog();

	bool LoadWaveFile(QString filePath);

	QList<double> GetLData() { return mDataBuffers[0]; }
	QList<double> GetRData() { return mDataBuffers[1]; }

	QList<quint8> BufferToHaptic();
private:

	WaveEngine *mWaveEngine = NULL;

	QList<double> mDataBuffers[2];

	int mBitPerSample;
	int mChannel;
	int mSampleRate;
private:
	void reset();
	void ConnectEvent();

private slots:
	//bool onResGetbuffer(const QAudioFormat &format, const WavFile::wav_header_t &Header, qint64 length, const QByteArray &buffer);
	bool onResGetbuffer(const WavFile::wav_header_t &Header, qint64 length, const QByteArray &buffer);
};

#endif // WAVETOHAPTIC_H
