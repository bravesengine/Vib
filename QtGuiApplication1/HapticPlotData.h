#pragma once

#include <QWidget>
#include <QByteArray>
#define AMP_LIMIT_VALUE 255

enum eHzType
{
	_320_HZ = 0,
	_160_HZ = 1,
	_80_HZ  = 2,
	_40_HZ  = 3,
	_20_HZ  = 4,
	_MAX_HZ,
};

class HzPlotData
{
public:
	eHzType hzType = eHzType::_320_HZ;
	//-- Range Time.	
	int  lowerIndex = 0;
	int  upperIndex = 0;
	quint8 HapticRawData = 0;

	bool operator==(const HzPlotData& other) const 
	{ 
		return lowerIndex == other.lowerIndex && upperIndex == other.upperIndex && HapticRawData == other.HapticRawData;
	}

	bool operator!=(const HzPlotData& other) const { return !(*this == other); }

private:
	friend QDataStream &operator>>(QDataStream &in, HzPlotData *m);

	friend QDataStream &operator<<(QDataStream &out, const HzPlotData &m);

};

class HapticPlotData 
{
public:	
	
	HapticPlotData();
	~HapticPlotData();

	enum eSerchDir
	{
		LOWER,
		UPPPER,
	};


	static bool ReadHzPlotData(QByteArray RawBytes, int Size, QList<HzPlotData> &hzData);


	void MakeRandom(float timeLength);
	QList<HzPlotData> MakePlotData(const QList<quint8> &InputRawData);// , bool bHzGroup);

	const QList<HzPlotData>& GetHzPlotData() {
		return m_HzPlotData;
	}

	float TimeLength = 0;
	int SelectedGraphCount = 0;
	
	bool GetSelectedDeatialDesc(QPoint hzGraphIndex, float &TimeLegth, int &SelectedGraphCount, quint8 &HigherAmpValue);


	QList<HzPlotData> Copy() {
		return m_HzPlotData;
	}

	QList<HzPlotData>* PlotData() {
		return &m_HzPlotData;
	}

	float GetTimeLength() {
		return m_TimeLength;
	}

	QPoint GetHzDataIndex(int begin, int end, float RawDataLerpCount, HapticPlotData::eSerchDir StartCoordSerchDir,
		HapticPlotData::eSerchDir EndCoordSerchDir);

	QList<HzPlotData> GetPlotDataRange(int begin, int end);
	
	bool SetHzRawData(int lowerIndex, int upperIndex, quint8 RawData);


	int GetLastUpperIndex();

	bool DataToReMakeTime(const QList<HzPlotData> &Data);
	bool UpdateReMakeTime();


	QByteArray HzToRaw(const QList<HzPlotData>& data);

	void SetData(QList<HzPlotData>& data)
	{
		m_HzPlotData = data;
	}

	QByteArray Serialize();

	QString GetPlayTimeLengthRangeToString(QPoint hzGraphIndex);
	QString  GetTimeString(float TimeLength);

private:
	float m_TimeLength =0;

	QList<HzPlotData> m_HzPlotData;

	int FindHzIndex(int StartRawDataIndex, int EndRawDataIndex, eSerchDir SerchType);

	
	//int FindPlotDataIndex(int RawDataIndex);

	float GetTimeLengthRange(QPoint hzGraphIndex);
	quint8   GetHigherAmpRange(QPoint hzGraphIndex);

	bool	HzCheckRoul(quint8 FirstRawValue, const QList<quint8> &InputRawData, int &ReadCount, int CheckZeroCount);

	
};
