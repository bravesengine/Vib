#ifndef WAVE_BUFFER_READER_H
#define WAVE_BUFFER_READER_H

#include <QBuffer>
#include <QByteArray>
#include <QDir>
#include <QObject>
#include <QDataStream>
#include <QAudioFormat>
#include "WavFile.h"

class WaveBufferReader : public QObject
{
	Q_OBJECT
public:
	WaveBufferReader::WaveBufferReader(const WavFile::wav_header_t &Header, const QByteArray& Buffer, QObject *parent)
		: QObject(parent)
	{
	
		mCurOffset = 0;
		mBuffer = Buffer;
		mStream = new QDataStream(Buffer);

		//const WavFile::wav_header_t &Header
		if (memcmp(&Header.chunkId, "RIFF", 4) == 0)
			mStream->setByteOrder(QDataStream::ByteOrder::LittleEndian);//   QAudioFormat::LittleEndian);
		else
			mStream->setByteOrder(QDataStream::ByteOrder::BigEndian);// QAudioFormat::BigEndian);

	}

	~WaveBufferReader()
	{
		delete mStream;
		mStream = NULL;
	}

	virtual double Read() = 0;
	virtual double ReadU() = 0;
	virtual double ReadNormailze() = 0;

	//static QSharedPointer<WaveBufferReader> Create(const QAudioFormat &format, const QByteArray& Buffer, QObject *parent);
	static QSharedPointer<WaveBufferReader> Create(const WavFile::wav_header_t &Header, const QByteArray& Buffer, QObject *parent);

	void skipRawData(int SeekPos);

private:
	QByteArray mBuffer;
	

protected:
	int mCurOffset;
	int mReadByte;
	QDataStream *mStream;
	
};

class WaveBufferReader_8 : public WaveBufferReader
{
    Q_OBJECT
public:
	explicit WaveBufferReader_8(const WavFile::wav_header_t &Header, const QByteArray& Buffer, QObject *parent = 0)
		: WaveBufferReader(Header, Buffer, parent)
	{

		//mStream->setByteOrder( .setByteOrder(byteOrder);


	}
	~WaveBufferReader_8()
	{

	}

	virtual double Read();
	virtual double ReadU();
	virtual double ReadNormailze();
};



class WaveBufferReader_16 : public WaveBufferReader
{
	Q_OBJECT
public:
	explicit WaveBufferReader_16(const WavFile::wav_header_t &Header, const QByteArray& Buffer, QObject *parent = 0)
		: WaveBufferReader(Header, Buffer, parent)
	{
	
	}
	~WaveBufferReader_16()
	{

	}
	virtual double Read();
	virtual double ReadU();
	virtual double ReadNormailze();
};

class WaveBufferReader_24 : public WaveBufferReader
{
	Q_OBJECT
public:
	explicit WaveBufferReader_24(const WavFile::wav_header_t &Header, const QByteArray& Buffer, QObject *parent = 0)
		: WaveBufferReader(Header, Buffer, parent)
	{

	}
	~WaveBufferReader_24()
	{

	}
	virtual double Read();
	virtual double ReadU();
	virtual double ReadNormailze();
};


class WaveBufferReader_32 : public WaveBufferReader
{
	Q_OBJECT
public:
	explicit WaveBufferReader_32(const WavFile::wav_header_t &Header, const QByteArray& Buffer, QObject *parent = 0)
		: WaveBufferReader(Header, Buffer, parent)
	{

	}
	~WaveBufferReader_32()
	{

	}
	virtual double Read();
	virtual double ReadU();
	virtual double ReadNormailze();
};

#endif // WAVE_BUFFER_READER_H
