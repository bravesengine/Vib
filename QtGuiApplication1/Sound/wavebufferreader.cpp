
#include "wavebufferReader.h"
#include <math.h>

#include <QDebug>
#include "Int24.h"

QSharedPointer<WaveBufferReader> WaveBufferReader::Create(const WavFile::wav_header_t &Header, const QByteArray& Buffer, QObject *parent)
{
	QSharedPointer<WaveBufferReader> Reader;
	switch (Header.bitsPerSample)//sampleSize())
	{
	case 8: Reader = QSharedPointer< WaveBufferReader>(new WaveBufferReader_8( Header, Buffer, parent));
		break;
	case 16: Reader = QSharedPointer< WaveBufferReader>(new WaveBufferReader_16(Header, Buffer, parent));
		break;
	case 24: Reader = QSharedPointer< WaveBufferReader>(new WaveBufferReader_24(Header,Buffer, parent));
		break;
	case 32: Reader = QSharedPointer< WaveBufferReader>(new WaveBufferReader_32(Header, Buffer, parent));
		break;
	}
	return Reader;
}

void WaveBufferReader::skipRawData(int SeekPos)
{
	if (mStream == NULL || SeekPos <= 0)
		return;

	//int QDataStream::skipRawData(int len)
	//	Skips len bytes from the device.Returns the number of bytes actually skipped, or -1 on error.
	//	This is equivalent to calling readRawData() on a buffer of length len and ignoring the buffer.
	//	This function was introduced in Qt 4.1.
	//	See also QIODevice::seek().
	mStream->skipRawData(SeekPos);
}

double WaveBufferReader_8::Read()
{
	quint8 v = 0;
	*mStream >> v;
	return v - 128;
}

double WaveBufferReader_16::Read()
{
	qint16 v = 0;
	*mStream >> v;
	return v;
}

double WaveBufferReader_24::Read()
{	
	unsigned char bits[3];

	*mStream >> bits[0] >> bits[1] >> bits[2];

	Int24 Value24Bit(bits[0], bits[1], bits[2]);

	return Value24Bit;
}


double WaveBufferReader_32::Read()
{
	qint32 v = 0;
	*mStream >> v;
	return v;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
double WaveBufferReader_8::ReadU()
{
	quint8 v = 0;
	*mStream >> v;
	return v;
}

double WaveBufferReader_16::ReadU()
{
	quint16 v=0;
	*mStream >> v;
	return v;
}

double WaveBufferReader_24::ReadU()
{
	Uint24 uint24;
	*mStream >> uint24.bits[0] >> uint24.bits[1] >> uint24.bits[2];

	return (double) uint24.as_unsigned();
}

double WaveBufferReader_32::ReadU()
{
	quint32 v = 0;
	*mStream >> v;
	return v;
}

//
////#define MB_LEN_MAX    5             // max. # bytes in multibyte char
////#define SHRT_MIN    (-32768)        // minimum (signed) short value
////#define SHRT_MAX      32767         // maximum (signed) short value
////#define USHRT_MAX     0xffff        // maximum unsigned short value
////#define INT_MIN     (-2147483647 - 1) // minimum (signed) int value
////#define INT_MAX       2147483647    // maximum (signed) int value
////#define UINT_MAX      0xffffffff    // maximum unsigned int value
////#define LONG_MIN    (-2147483647L - 1) // minimum (signed) long value
////#define LONG_MAX      2147483647L   // maximum (signed) long value
////#define ULONG_MAX     0xffffffffUL  // maximum unsigned long value
////#define LLONG_MAX     9223372036854775807i64       // maximum signed long long int value
////#define LLONG_MIN   (-9223372036854775807i64 - 1)  // minimum signed long long int value
////#define ULLONG_MAX    0xffffffffffffffffui64       // maximum unsigned long long int value
//

//1 - channel, 8 - bit
//data type : unsigned 8 - bit
//2 - channel, 8 - bit
//data type : unsigned 8 - bit

//데이터가 8 - bit인 경우 입력이 없을 때 값이 128이며 128을 기준으로 변한다
double WaveBufferReader_8::ReadNormailze()
{
	quint8 v = 0;
	*mStream >> v;
	double nv = 128 - v;// -128;
	return (nv /128);
}

//2 - channel, 16 - bit
//data type : signed 16 - bit
double WaveBufferReader_16::ReadNormailze()
{	
	qint16 v = 0;
	*mStream >> v;	 
	return ((double) v / SHRT_MAX);
}

double WaveBufferReader_24::ReadNormailze()
{
	unsigned char bits[3];
	*mStream >> bits[0];
	*mStream >> bits[1];
	*mStream >> bits[2];
	 
	Int24 int24(bits[0], bits[1], bits[2]);

	return  ( (double) int24 / INT24_MAX);
}

double WaveBufferReader_32::ReadNormailze()
{
	qint32 v = 0;
	*mStream >> v;
	return ( (double) v / INT_MAX);
}
