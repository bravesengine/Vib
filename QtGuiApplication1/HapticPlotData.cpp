#include "HapticPlotData.h"
#include "qcustomplot.h"


static int randInt(int low, int high)
{
	return qrand() % ((high + 1) - low) + low;
}

QDataStream &operator>>(QDataStream &in, HzPlotData *m)
{
	int hzType = 0;
	in >> hzType >> m->lowerIndex >> m->upperIndex >> m->HapticRawData;

	m->hzType = (eHzType) hzType;

	return in;

}

QDataStream &operator<<(QDataStream &out, const HzPlotData &m)
{
	out << (int) m.hzType << m.lowerIndex << m.upperIndex << m.HapticRawData;
	return out;
}

HapticPlotData::HapticPlotData()
{
}

HapticPlotData::~HapticPlotData()
{

}

QByteArray HapticPlotData::Serialize()
{
	QByteArray byteArray;
	QDataStream stream(&byteArray, QIODevice::WriteOnly);
	stream.setVersion(QDataStream::Qt_4_5);

	for (int i = 0; i < m_HzPlotData.size(); i++)
	{
		HzPlotData* hzSavedata = &m_HzPlotData[i];
		stream << hzSavedata->HapticRawData;
		stream << (quint32&)hzSavedata->hzType;
		stream << hzSavedata->lowerIndex;
		stream << hzSavedata->upperIndex;
	}

	return byteArray;
}

//  이거 수정...
QByteArray HapticPlotData::HzToRaw(const QList<HzPlotData>& data)
{
	if(m_HzPlotData.size() == 0)
		return QByteArray(0);

	int TotalRawSize =  m_HzPlotData.last().upperIndex;
	char* RawData = new char[TotalRawSize];
	memset(RawData, 0, sizeof(TotalRawSize));

	QByteArray RawArray =  QByteArray(RawData, TotalRawSize);

	for (int i = 0; i < m_HzPlotData.size(); i++)
	{	
		HzPlotData* d = &m_HzPlotData[i];

		for (int j = d->lowerIndex; j < d->upperIndex; ++j)
		{
			if( j == d->lowerIndex)
				RawArray[j] = d->HapticRawData;
			else
				RawArray[j] = 0;
			//if ( j+ 1 == (d->upperIndex))				
			//	RawArray[j] = d->HapticRawData;
			//else
			//	RawArray[j] = 0;
		}
	}

	return RawArray;
}


bool  HapticPlotData::HzCheckRoul(quint8 FirstRawValue, const QList<quint8> &InputRawData, int &ReadCount, int CheckZeroCount)
{
	if (FirstRawValue == 0)
		return false;


	int TempReadCount = ReadCount +1;
	for (int i = 0; i < CheckZeroCount; i++)
	{
		if (TempReadCount >= InputRawData.count())
			return false;

		if (InputRawData[TempReadCount] != 0)
			return false;

		TempReadCount++;
	}

	ReadCount += CheckZeroCount+1;
	return true;

}

//  이거..
QList<HzPlotData> HapticPlotData::MakePlotData(const QList<quint8> &InputRawData)//, bool bHzGroup)
{
	QList<HzPlotData> hzData;
	int ReadIndex = 0;
	do
	{
		HzPlotData data;
		data.lowerIndex = ReadIndex;
		data.HapticRawData = InputRawData[ReadIndex];

		if (HzCheckRoul(InputRawData[ReadIndex], InputRawData, ReadIndex, 4))
		{
			data.hzType = eHzType::_20_HZ;
		}
		else if (HzCheckRoul(InputRawData[ReadIndex], InputRawData, ReadIndex, 3))
		{
			data.hzType = eHzType::_40_HZ;
		}
		else if (HzCheckRoul(InputRawData[ReadIndex], InputRawData, ReadIndex, 2))
		{
			data.hzType = eHzType::_80_HZ;
		}
		else if (HzCheckRoul(InputRawData[ReadIndex], InputRawData, ReadIndex, 1))
		{
			data.hzType = eHzType::_160_HZ;
		}
		else 
		{
			data.hzType = eHzType::_320_HZ;
			ReadIndex++;
		}	

		data.upperIndex = ReadIndex;
		hzData.push_back(data);

	} while (ReadIndex < InputRawData.count());

	//for (int i = 0; i < InputRawData.size(); i++)
	//{
	//	// 4개식 끊어서 체크?
	//	//for (int h = 0; h < 4; h++)
	//	//{			
	//	//	i++;
	//	//}
	//	//if (bHzGroup)
	//	//{
	//		// 어떤 숫자가 앞에 있고? 뒤에가 0 인지 체크 , 앞 + 3개까지?
	//		// -- 
	//		if (InputRawData[i] == 0)
	//		{
	//			hzType = (eHzType)((int)hzType + 1);
	//			//lowerIndex = i;
	//		}
	//		else
	//		{
	//			data.hzType = hzType;
	//			data.lowerIndex = lowerIndex;
	//			data.upperIndex = i + 1;
	//			data.HapticRawData = InputRawData[i];
	//			hzData.push_back(data);

	//			/////////////////////////////////
	//			hzType = eHzType::_320_HZ;
	//			lowerIndex = i + 1;
	//			//////////////////////////////////
	//		}
	//	//}
	//	//else
	//	//{
	//	//	data.hzType = hzType;
	//	//	data.lowerIndex = lowerIndex;
	//	//	data.upperIndex = i + 1;
	//	//	data.HapticRawData = InputRawData[i];

	//	//	hzData.push_back(data);
	//	//	/////////////////////////////////
	//	//	hzType = eHzType::_320_HZ;
	//	//	lowerIndex = i + 1;
	//	//}
	//}

	return hzData;
}


//  뭐 중간 사이 집어넣는다거나?
//--  hz 변경한다거나..?

void HapticPlotData::MakeRandom(float timeLength)
{
	QList<quint8> HatpticRawData;
	
	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());

	//-- TestData..
	HatpticRawData.push_back(64);
	HatpticRawData.push_back(128);
	HatpticRawData.push_back(128);

	HatpticRawData.push_back(0);
	HatpticRawData.push_back(0);
	HatpticRawData.push_back(90);
	HatpticRawData.push_back(0);
	HatpticRawData.push_back(64);
	HatpticRawData.push_back(0);
	HatpticRawData.push_back(0);
	HatpticRawData.push_back(0);
	HatpticRawData.push_back(0);
	HatpticRawData.push_back(64);
	HatpticRawData.push_back(128);
	HatpticRawData.push_back(128);

	for(int i=0;i < 30; i++)
		HatpticRawData.push_back(randInt(0,128));

	//-- 테스트로 일단 1초.
	m_TimeLength = HatpticRawData.size() * 3.125;
	//-- 최대 햅틱 버퍼 256개, 3.125 ms  최대 (320hz)
	float DeltaKeyTime = ((800 / 256.0f));
	int TotalDataArrayCount = m_TimeLength / DeltaKeyTime;

	for (int i = HatpticRawData.size(); i < TotalDataArrayCount; i++)
		HatpticRawData.push_back( (quint8) randInt(0, 128));

	m_HzPlotData = MakePlotData(HatpticRawData);// , true);
}

bool HapticPlotData::DataToReMakeTime(const QList<HzPlotData> &Data)
{
	m_HzPlotData = Data;
	return UpdateReMakeTime();
}

bool HapticPlotData::UpdateReMakeTime()
{
	int RawDataCount = 0;
	for (int i = 0; i < m_HzPlotData.size(); i++)
	{
		RawDataCount += m_HzPlotData[i].hzType + 1;
	}
	m_TimeLength = RawDataCount * 3.125;
	return true;
}

//-- 30식 끊어짐.
int HapticPlotData::FindHzIndex(int StartRawDataIndex, int EndRawDataIndex,  eSerchDir SerchType)
{	
	//int StartIndex = qMin(StartRawDataIndex, EndRawDataIndex);
	//int EndIndex = qMax(StartRawDataIndex, EndRawDataIndex);
	//int HapticPlotData::FindHzIndex(int RawDataIndex)
	//{
	//	for (int i = 0; i < m_HzPlotData.size(); i++)
	//	{
	//		if (RawDataIndex >= m_HzPlotData[i].lowerIndex)
	//		{
	//			if (RawDataIndex < m_HzPlotData[i].upperIndex)
	//				return i;
	//		}
	//	}

	//	return -1;
	//}

	if(SerchType == eSerchDir::LOWER)
	{
		for (int i = 0; i < m_HzPlotData.size(); i++)
		{
			if (StartRawDataIndex >= m_HzPlotData[i].lowerIndex)
			{
				if(EndRawDataIndex < m_HzPlotData[i].upperIndex)
					return i;
			}
		}
	}
	else
	{
		for (int i = m_HzPlotData.size() -1 ; i >= 0 ; i--)
		{
			if (StartRawDataIndex >= m_HzPlotData[i].lowerIndex)
				return i;

		}


	}

	return -1;
}

int HapticPlotData::GetLastUpperIndex()
{
	int Size = m_HzPlotData.size();
	if (Size == 0)
		return 0;
	
	return m_HzPlotData[Size - 1].upperIndex;
}

QPoint HapticPlotData::GetHzDataIndex(int begin, int end, float RawDataLerpCount, HapticPlotData::eSerchDir StartCoordSerchDir,
	HapticPlotData::eSerchDir EndCoordSerchDir)
{	
	int hzDataBeginIndex =  begin / RawDataLerpCount;
	int hzDataEndIndex = end / RawDataLerpCount;

	QPoint DataRangeIndex;	

	DataRangeIndex.setX(FindHzIndex(hzDataBeginIndex, hzDataBeginIndex, StartCoordSerchDir));
	
	DataRangeIndex.setY(FindHzIndex(hzDataEndIndex, hzDataEndIndex, EndCoordSerchDir));


	return DataRangeIndex;
}



bool HapticPlotData::GetSelectedDeatialDesc(QPoint hzGraphIndex, float &TimeLegth, int &SelectedGraphCount, quint8 &HigherAmpValue)
{
	if (hzGraphIndex.y() > m_HzPlotData.size() || hzGraphIndex.y() < 0)
		return false;

	if (hzGraphIndex.x() > m_HzPlotData.size() || hzGraphIndex.x() < 0)
		return false;

	SelectedGraphCount = hzGraphIndex.y() - hzGraphIndex.x();
	if (SelectedGraphCount == 0) SelectedGraphCount = 1;

	TimeLegth = GetTimeLengthRange(hzGraphIndex);	
	HigherAmpValue = GetHigherAmpRange(hzGraphIndex);

	return true;
}

QString HapticPlotData::GetPlayTimeLengthRangeToString(QPoint hzGraphIndex)
{
	int low = m_HzPlotData[hzGraphIndex.x()].lowerIndex;

	int upper = 0;

	if (hzGraphIndex.y() >= m_HzPlotData.size())
		upper = m_HzPlotData[m_HzPlotData.size() - 1].upperIndex;
	else
		upper = m_HzPlotData[hzGraphIndex.y()].lowerIndex;

	int range = upper - low;
	if (range == 0) range = 1;

	int start = qMin(low, upper);
	int end = qMax(low, upper);

	QString LowTimeStr = GetTimeString((start * 3.125));
	QString UppperTimeStr = GetTimeString((end * 3.125));

	return  QString("%1 ~ %2 m/s").arg(LowTimeStr).arg(UppperTimeStr);

}

QString HapticPlotData::GetTimeString(float TimeLength)
{
	int sec = TimeLength / 1000;
	float msec =  (TimeLength - (float)(sec * 1000)) /  1000.f;

	QString LengthStr;
	LengthStr.sprintf("%03f", msec);
	LengthStr = LengthStr.replace("0.", "");

	QString TimeStr = QString("%1.%2").arg(sec).arg(LengthStr);


	////getUi().HapticLengthLabel->setText(m_TimeLength);
//int sec = m_TimeLength / 1000;
////float msec = m_TimeLength -  ( sec * 1000 );
//float msec = (m_TimeLength - (float)(sec * 1000)) / 1000;
//int min = sec / 60;
////sprintf("%02d", 10)
//QString LengthStr;
////LengthStr.sprintf("%02d:%02d:%03.1f", min, sec, msec);
//LengthStr.sprintf("%03f", msec);
//LengthStr = LengthStr.replace("0.", "");

//QString TimeStr = QString("%1.%2 ms").arg(sec).arg(LengthStr);
//ui.HapticLengthLabel->setText(TimeStr);
////ui.StatusDesc->setText(QString("Total Graph points: %1 Time = %2/ms").arg(ui.Plot->graph(0)->data()->size()).arg(m_TimeLength));

	return TimeStr;
}

float HapticPlotData::GetTimeLengthRange(QPoint hzGraphIndex)
{
   int low   = m_HzPlotData[hzGraphIndex.x()].lowerIndex;

   int upper = 0;

   if (hzGraphIndex.y() >= m_HzPlotData.size())
	   upper = m_HzPlotData[m_HzPlotData.size() -1].upperIndex;
   else
	   upper = m_HzPlotData[hzGraphIndex.y()].lowerIndex;


   int range = upper - low;

   if (range == 0) range = 1;

   return ( range * 3.125) / 1000;
}

quint8 HapticPlotData::GetHigherAmpRange(QPoint hzGraphIndex)
{
	int RangeStart = hzGraphIndex.x();
	int RangeEnd = hzGraphIndex.y();

	quint8 MaxAmpValue = m_HzPlotData[RangeStart].HapticRawData;


	for (int i = RangeStart; i < RangeEnd; i++)
	{
		MaxAmpValue = qMax<quint8>(MaxAmpValue, m_HzPlotData[i].HapticRawData);
	}

	return MaxAmpValue;
}

QList<HzPlotData> HapticPlotData::GetPlotDataRange(int begin, int end)
{
	QList<HzPlotData> hzData;
	QList<int> hzDataIndex;

	int rStart = qMin(begin, end);
	int rEnd = qMax(begin, end);

	for (int i = rStart; i < rEnd; i++)
	{
		int hzDataArrayIndex = FindHzIndex(i, i, eSerchDir::LOWER);
		if (hzDataArrayIndex == -1)
			continue;

		if (!hzDataIndex.contains(hzDataArrayIndex))
			hzDataIndex.push_back(hzDataArrayIndex);
	}

	for (int i = 0; i < hzDataIndex.size(); i++)
	{
		HzPlotData PlotData =  m_HzPlotData.at(hzDataIndex[i]);
		hzData.push_back(PlotData);
	}

	return hzData;
}

bool HapticPlotData::SetHzRawData(int lowerIndex, int upperIndex, quint8 RawData)
{
	for (int i = 0; i < m_HzPlotData.size(); i++)
	{
		if (m_HzPlotData[i].lowerIndex == lowerIndex && m_HzPlotData[i].upperIndex == upperIndex)
		{
			m_HzPlotData[i].HapticRawData = RawData;

			return true;
		}
	}

	return false;
}


bool HapticPlotData::ReadHzPlotData(QByteArray RawBytes, int Size, QList<HzPlotData> &hzData)
{
	QDataStream RealDataStrem(&RawBytes, QIODevice::ReadOnly);

	for (int i = 0; i < Size; i++)
	{
		HzPlotData hzReadData;
		RealDataStrem >> hzReadData.HapticRawData;
		RealDataStrem >> (quint32&)hzReadData.hzType;
		RealDataStrem >> hzReadData.lowerIndex;
		RealDataStrem >> hzReadData.upperIndex;

		hzData.push_back(hzReadData);
	}
	
	return true;
}
