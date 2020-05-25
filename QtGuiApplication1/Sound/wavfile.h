
#ifndef WAVFILE_H
#define WAVFILE_H

#include <QObject>
#include <QFile>
#include <QAudioFormat>


class WavFile : public QFile
{
public:



	//Wav Header
	struct wav_header_t
	{
		char chunkId[4]; //"RIFF" = 0x46464952
		quint32 chunkSize; //28 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes] + sum(sizeof(chunk.id) + sizeof(chunk.size) + chunk.size)
		char format[4]; //"WAVE" = 0x45564157
		char subchunk1ID[4]; //"fmt " = 0x20746D66
		quint32 subchunk1Size; //16 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes]
		quint16 audioFormat;
		quint16 numChannels;
		quint32 sampleRate;
		quint32 byteRate;
		quint16 blockAlign;
		quint16 bitsPerSample;
		//[quint16 wExtraFormatBytes;]
		//[Extra format bytes]
	};



    WavFile(QObject *parent = 0);

    using QFile::open;
    bool open(const QString &fileName);

	//const QAudioFormat &fileFormat() const;
    qint64 headerLength() const;

	qint64 SampleTotCount()
	{ 
		return m_samplesTotalCount;
	}

	quint32 DataSize()
	{
		return m_ChunkDataSize;
	}
	
	wav_header_t GetHeader()
	{
		return mWaveHeader;
	}

private:
    bool readHeader();

private:
    QAudioFormat m_fileFormat;
    qint64 m_headerLength;

	qint64 m_samplesTotalCount;

	quint32 m_ChunkDataSize = 0;
	//qint64 m_dataSize;


	//CombinedHeader mCombinedHeader;
	//DATAHeader mDataHeader;

	wav_header_t mWaveHeader;

};

#endif // WAVFILE_H
