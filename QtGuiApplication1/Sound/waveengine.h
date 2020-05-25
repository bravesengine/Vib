#ifndef WAVE_ENGINE_H
#define WAVE_ENGINE_H

#include "wavfile.h"

#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QBuffer>
#include <QByteArray>
#include <QDir>
#include <QObject>

class WaveEngine : public QObject
{
    Q_OBJECT

public:
    explicit WaveEngine(QObject *parent = 0);
    ~WaveEngine();

  //  const QAudioFormat& format() const { return m_format; }
    void reset();

    bool loadFile(const QString &fileName);
	bool ReqGetBuffer();

public slots:

signals:
	bool ResGetbuffer(const WavFile::wav_header_t &Header, qint64 length, const QByteArray &buffer);
	//bool ResGetbuffer(const QAudioFormat &format, const WavFile::wav_header_t &Header, qint64 length, const QByteArray &buffer);

private:
    WavFile*            m_file;
    WavFile*            m_analysisFile;

  //  QAudioFormat        m_format;

	QByteArray          m_buffer;
	qint64              m_dataLength;
private:
	

};

#endif // ENGINE_H
