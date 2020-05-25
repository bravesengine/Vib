/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qendian.h>
#include <QVector>
#include <QDebug>
#include <QDataStream>

#include "wavfile.h"

#include <QFile>

WavFile::WavFile(QObject *parent)
	: QFile(parent)
	, m_headerLength(0)
{
	m_ChunkDataSize = 0;
}

bool WavFile::open(const QString &fileName)
{
	close();
	setFileName(fileName);

	//ReadWav(fileName);

	return QFile::open(QIODevice::ReadOnly) && readHeader();
}

//const QAudioFormat &WavFile::fileFormat() const
//{
//    return m_fileFormat;
//}

qint64 WavFile::headerLength() const
{
	return m_headerLength;
}

bool WavFile::readHeader()
{
	seek(0);

	//Read WAV file header
	QDataStream analyzeHeader(this);

	analyzeHeader.setByteOrder(QDataStream::LittleEndian);

	// RIFFHeader..
	analyzeHeader.readRawData(mWaveHeader.chunkId, 4); // "RIFF"
	analyzeHeader >> mWaveHeader.chunkSize; // File Size
	analyzeHeader.readRawData(mWaveHeader.format, 4); // "WAVE"

	// WaveHeader..
	analyzeHeader.readRawData(mWaveHeader.subchunk1ID, 4); // "fmt"
	analyzeHeader >> mWaveHeader.subchunk1Size; // Format length


	analyzeHeader >> mWaveHeader.audioFormat; // Format type
	analyzeHeader >> mWaveHeader.numChannels; // Number of channels
	analyzeHeader >> mWaveHeader.sampleRate; // Sample rate
	analyzeHeader >> mWaveHeader.byteRate; // (Sample Rate * BitsPerSample * Channels) / 8
	analyzeHeader >> mWaveHeader.blockAlign; // (BitsPerSample * Channels) / 8.1
	analyzeHeader >> mWaveHeader.bitsPerSample; // Bits per sample

	if (mWaveHeader.subchunk1Size == 18) // fmt chunk can be 16 or 18 bytes
	{
		qint16 fmtExtraSize;
		read((char*)&fmtExtraSize, sizeof(qint16)); // read extra bytes size				
		read(fmtExtraSize);         // skip over "INFO" chunk
	}


	qDebug() << "WAV File Header read:";
	qDebug() << "File Type: " << mWaveHeader.chunkId;
	qDebug() << "File Size: " << mWaveHeader.chunkSize;

	qDebug() << "WAV Marker: " << mWaveHeader.format;
	qDebug() << "Format Name: " << mWaveHeader.subchunk1ID;
	qDebug() << "Format Length: " << mWaveHeader.subchunk1Size;
	qDebug() << "Format Type: " << mWaveHeader.audioFormat;
	qDebug() << "Number of Channels: " << mWaveHeader.numChannels;
	qDebug() << "Sample Rate: " << mWaveHeader.sampleRate;
	qDebug() << "Sample Rate * Bits/Sample * Channels / 8: " << mWaveHeader.byteRate;
	qDebug() << "Bits per Sample * Channels / 8.1: " << mWaveHeader.blockAlign;
	qDebug() << "Bits per Sample: " << mWaveHeader.bitsPerSample;


	if ((memcmp(&mWaveHeader.chunkId, "RIFF", 4) == 0 || memcmp(&mWaveHeader.chunkId, "RIFX", 4) == 0))
	{
		//--
		if (memcmp(&mWaveHeader.format, "WAVE", 4) != 0)
			return false;

		if (memcmp(&mWaveHeader.subchunk1ID, "fmt ", 4) != 0)
			return false;

		//0x0001	WAVE_FORMAT_PCM	PCM
		//0x0003	WAVE_FORMAT_IEEE_FLOAT	IEEE float
		//0x0006	WAVE_FORMAT_ALAW	8 - bit ITU - T G.711 A - law
		//0x0007	WAVE_FORMAT_MULAW	8 - bit ITU - T G.711 µ - law
		//0xFFFE	WAVE_FORMAT_EXTENSIBLE	Determined by SubFormat

		if (mWaveHeader.audioFormat == 3 || mWaveHeader.audioFormat == 6 || mWaveHeader.audioFormat == 7)
			return false;

	}
	else
	{
		return false;
	}

	m_samplesTotalCount = (mWaveHeader.chunkSize / mWaveHeader.bitsPerSample / 8);  // total samples count in audio data

	//Search data chunk
	m_ChunkDataSize = 0;
	QByteArray temp_buff;
	char buff[0x04];
	while (true)
	{
		QByteArray tmp = this->read(0x04);

		temp_buff.append(tmp);
		int idx = temp_buff.indexOf("data");

		if (idx >= 0)
		{
			int lenOfData = temp_buff.length() - (idx + 4);
			memcpy(buff, temp_buff.constData() + idx + 4, lenOfData);

			int bytesToRead = 4 - lenOfData;

			// finish readind size of chunk
			if (bytesToRead > 0)
			{
				int read = this->read(buff + lenOfData, bytesToRead);

				if (bytesToRead != read)
				{
					qDebug() << "Error: Something awful happens!";
					return false;
				}
			}

			m_ChunkDataSize = qFromLittleEndian<quint32>((const uchar*)buff);
			break;
		}

		if (temp_buff.length() >= 8)
		{
			temp_buff.remove(0, 0x04);
		}
	}

	if (!m_ChunkDataSize)
	{
		qDebug() << "Error: Chunk data not found!";
		return false;
	}

	m_headerLength = pos();
	return true;
}
