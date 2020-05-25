#pragma once

#include <QWidget>
#include "qcustomplot.h"
#include "HapticPlotData.h"
#include "singleton.h"
//class QCPGraphDataContainer;
//class HapticPlotData;
class HapticPlotGraph;
class HapticPlotDataSelectAction;

class HaticModifGraph : public QObject
{
	Q_OBJECT
public:
	enum eModifGraphDataType
	{
		AMP_UP,
		AMP_DOWN,
		HZ_UP,
		HZ_DOWN,
		PASTH_GRAHP,
		CUT_GRAHP,
		NONE,
	};
	Q_ENUM(eModifGraphDataType)
		// 일단 데이터들.. 필요하고..

	HaticModifGraph() {}
	HaticModifGraph( eModifGraphDataType t);

	virtual bool RunModif(HapticPlotGraph *HapticGraph,
		HapticPlotData* UserData, HapticPlotDataSelectAction *SelectAction, double StartKey, double EndKey, QCPItemRect* RectRangeItem) { return false; }

protected:

	HapticPlotGraph *mHapticGraph = NULL;
	HapticPlotData* mPlotData = NULL;
	double mStartKey = 0;
	double mEndKey = 0;

	eModifGraphDataType mModifType = eModifGraphDataType::NONE;
	
	bool  ModifAmpData(HapticPlotData* UserData, QList<HzPlotData> &hzBodyData, int IncreaseValue);
	bool  ModifHzData(HapticPlotData* UserData, QList<HzPlotData> &hzBodyData);

	bool   GetCliptBoardData(QString MimeName, QList<HzPlotData> &OutList, int &ByteArraySize);

	bool  IsEqualHzPlotData(QList<HzPlotData> v1, QList<HzPlotData> v2);

};

//Q_DECLARE_METATYPE(HaticModifGraph)
Q_DECLARE_METATYPE(HaticModifGraph::eModifGraphDataType)

class HapticModifFuncAmpUp : public HaticModifGraph
{
public:
	HapticModifFuncAmpUp();

	virtual bool RunModif(HapticPlotGraph *HapticGraph,
		HapticPlotData* UserData, HapticPlotDataSelectAction *SelectAction, double StartKey, double EndKey, QCPItemRect* RectRangeItem);

	////--use sample
	//static HapticModifFunc* instance()
	//{
	//	return Singleton<HapticModifFunc>::instance(HapticModifFunc::createInstance);
	//}

	//bool ModifGraphData(HapticPlotGraph *HapticGraph,
	//	 HapticPlotData* UserData, double StartKey, double EndKey, QCPItemRect* RectRangeItem, eModifGraphDataType t);

private:
	//static HapticModifFunc* createInstance();
	//bool  ModifHzData(HapticPlotData* UserData, QList<HzPlotData> &hzBodyData, eModifGraphDataType t);
};

class HapticModifFuncAmpDown : public HaticModifGraph
{
public:
	HapticModifFuncAmpDown();

	virtual bool RunModif(HapticPlotGraph *HapticGraph,
		HapticPlotData* UserData, HapticPlotDataSelectAction *SelectAction, double StartKey, double EndKey, QCPItemRect* RectRangeItem);
private:
};


class HapticModifFuncHz : public HaticModifGraph
{
public:
	HapticModifFuncHz(eModifGraphDataType t) : HaticModifGraph(t)
	{
	}

	virtual bool RunModif(HapticPlotGraph *HapticGraph,
		HapticPlotData* UserData, HapticPlotDataSelectAction *SelectAction, double StartKey, double EndKey, QCPItemRect* RectRangeItem);
};

class HapticModifFuncHzDown : public HapticModifFuncHz
{
public:
	HapticModifFuncHzDown();

	virtual bool RunModif(HapticPlotGraph *HapticGraph,
		HapticPlotData* UserData, HapticPlotDataSelectAction *SelectAction, double StartKey, double EndKey, QCPItemRect* RectRangeItem);
private:
};

class HapticModifFuncHzUp : public HapticModifFuncHz
{
public:
	HapticModifFuncHzUp();

	virtual bool RunModif(HapticPlotGraph *HapticGraph,
		HapticPlotData* UserData, HapticPlotDataSelectAction *SelectAction, double StartKey, double EndKey, QCPItemRect* RectRangeItem);
private:
};


class HapticModifFuncPasth : public HaticModifGraph
{
public:
	HapticModifFuncPasth();

	virtual bool RunModif(HapticPlotGraph *HapticGraph,
		HapticPlotData* UserData, HapticPlotDataSelectAction *SelectAction, double StartKey, double EndKey, QCPItemRect* RectRangeItem);

private:
};

class HapticModifFuncCut : public HaticModifGraph
{
public:
	HapticModifFuncCut();

	virtual bool RunModif(HapticPlotGraph *HapticGraph,
		HapticPlotData* UserData, HapticPlotDataSelectAction *SelectAction, double StartKey, double EndKey, QCPItemRect* RectRangeItem);

private:
};



