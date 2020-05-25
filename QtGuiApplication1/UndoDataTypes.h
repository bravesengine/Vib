#pragma once

#include <QWidget>
#include <QMetaType>
#include "qcustomplot.h"
#include <QVariant>
#include "HapticPlotData.h"
#include "UndoCommands.h"

#include <QByteArray>
#include <QObject>

class QUD_DataCoord {

public:
	QUD_DataCoord() {}
	
	QUD_DataCoord &operator=(const QUD_DataCoord &other);

	
	QUD_DataCoord(const QPointF &oldCoord, const QPointF &newCoord, int actType)
	{
		mOldDataCoord = oldCoord;
		mNewDataCoord = newCoord;

		mActType = actType;
	}

	QUD_DataCoord(const QUD_DataCoord& other);
	~QUD_DataCoord();

	QPointF old_Coord()
	{
		return mOldDataCoord;
	}

	QPointF new_Coord()
	{
		return mNewDataCoord;
	}

	int ActType() { return mActType; }

private:
	
	QPointF mOldDataCoord;
	QPointF mNewDataCoord;

	int mActType;
};

Q_DECLARE_METATYPE(QUD_DataCoord);
Q_DECLARE_METATYPE(QUD_DataCoord*);


class QUD_HzPlotData 
{

public:
	QUD_HzPlotData() {}
	QUD_HzPlotData &operator=(const QUD_HzPlotData &other);


	QUD_HzPlotData(const UnDo_GraphStateData &oldData, const UnDo_GraphStateData &newData, int ModifType, int actType)
	{
		m_OldPlotData = oldData;
		m_NewPlotData = newData;
		mModifType = ModifType;
		mActType = actType;
	}

	QUD_HzPlotData(const QUD_HzPlotData& other);
	~QUD_HzPlotData();

	UnDo_GraphStateData& old_Data()
	{
		return m_OldPlotData;
	}

	UnDo_GraphStateData& new_Data()
	{
		return m_NewPlotData;
	}

	int ModifType() {
		return mModifType;
	}
	int ActType()
	{
		return mActType;
	}

private:

	UnDo_GraphStateData m_OldPlotData;
	UnDo_GraphStateData m_NewPlotData;

	int mModifType;
	int mActType;
};

Q_DECLARE_METATYPE(QUD_HzPlotData);


class QUD_HzCopyPlotData
{

public:
	QUD_HzCopyPlotData() {}
	QUD_HzCopyPlotData &operator=(const QUD_HzCopyPlotData &other);

	QUD_HzCopyPlotData( QSharedPointer<QByteArray> &CopyData, QSharedPointer<QByteArray> &PrevData, int RangeBegin, int RangeEnd)
	{
		mCopyData = CopyData;
		mPrevData = PrevData;
	}

	QUD_HzCopyPlotData(const QUD_HzCopyPlotData& other);
	~QUD_HzCopyPlotData();

	QSharedPointer<QByteArray> old_Data() {
		return mPrevData;
	}

	QSharedPointer<QByteArray> new_Data() {
		return mCopyData;
	}

private:

	QSharedPointer<QByteArray> mCopyData;
	QSharedPointer<QByteArray> mPrevData;

};

Q_DECLARE_METATYPE(QUD_HzCopyPlotData);
