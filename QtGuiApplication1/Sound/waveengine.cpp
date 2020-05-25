
#include "waveengine.h"
#include <math.h>

#include <QDebug>
#include <QFile>
#include <QMessageBox>

//
bool isPCM(const QAudioFormat &format)
{
	return (format.codec() == "audio/pcm");
}


bool isPCMS16LE(const QAudioFormat &format)
{
	return isPCM(format) &&
		format.sampleType() == QAudioFormat::SignedInt &&
		format.sampleSize() == 16 &&
		format.byteOrder() == QAudioFormat::LittleEndian;
}

WaveEngine::WaveEngine(QObject *parent)
    :   QObject(parent)

{
  
	m_file = NULL;
	m_analysisFile = NULL;
	m_dataLength = 0;
}

WaveEngine::~WaveEngine()
{

}

void WaveEngine::reset()
{
	m_dataLength = 0;
	m_buffer.clear();

//	m_format = QAudioFormat();
	if(m_file)
		delete m_file;
	
	m_file = NULL;
	
	if(m_analysisFile)
		delete m_analysisFile;
	
	m_analysisFile = NULL;
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

bool WaveEngine::loadFile(const QString &fileName)
{
    reset();
  

	Q_ASSERT(!m_file);
    Q_ASSERT(!fileName.isEmpty());
	
	m_file = new WavFile(this);

	bool result = m_file->open(fileName);

	if(result == false)
	{
		QMessageBox::warning(NULL, "Error", "Invalid Audio format!!", QMessageBox::Close);
	}
	else
	{	
       m_analysisFile = new WavFile(this);
       m_analysisFile->open(fileName);
    }

    return result;
}


//-- 전체 버퍼 읽어오기..
bool WaveEngine::ReqGetBuffer()
{
	if (m_file == NULL || m_analysisFile == NULL)
		return false;

	//if (!isPCM(m_file->fileFormat()))
	//	return false;

	if (!m_analysisFile->seek(m_analysisFile->headerLength()))
		return false;

	qint64 TotalDataCount = m_file->DataSize();
	m_buffer.resize(TotalDataCount);

	m_dataLength = m_analysisFile->read(m_buffer.data(), TotalDataCount);
	//// metadata:
	//long moreBytes = reader.BaseStream.Length - reader.BaseStream.Position;
	//if (moreBytes > 0)
	//	metaData = reader.ReadBytes((int)moreBytes);

	return emit ResGetbuffer(m_file->GetHeader(), m_dataLength, m_buffer);
	
	//return true;
}
