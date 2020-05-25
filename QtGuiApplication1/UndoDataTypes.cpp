#include "UndoDataTypes.h"


QUD_DataCoord &QUD_DataCoord::operator=(const QUD_DataCoord &other)
{
	if (&other != this)
	{
		mOldDataCoord= other.mOldDataCoord;
		mNewDataCoord = other.mNewDataCoord;
		mActType = other.mActType;
	}

	return *this;
}

QUD_DataCoord::QUD_DataCoord(const QUD_DataCoord& other)
{
	mOldDataCoord = other.mOldDataCoord;
	mNewDataCoord = other.mNewDataCoord;
	mActType = other.mActType;
	//Q_UNUSED(other)
}

QUD_DataCoord::~QUD_DataCoord()
{

}

///////////////////////////////////////////////
QUD_HzPlotData &QUD_HzPlotData::operator=(const QUD_HzPlotData &other)
{
	if (&other != this)
	{
		m_OldPlotData = other.m_OldPlotData;
		m_NewPlotData = other.m_NewPlotData;
		mModifType = other.mModifType;
		mActType = other.mActType;
	}

	return *this;
}

QUD_HzPlotData::QUD_HzPlotData(const QUD_HzPlotData& other)
{
	m_OldPlotData = other.m_OldPlotData;
	m_NewPlotData = other.m_NewPlotData;
	mModifType = other.mModifType;
	mActType = other.mActType;
	//Q_UNUSED(other)
}

QUD_HzPlotData::~QUD_HzPlotData()
{

}



///////////////////////////////


QUD_HzCopyPlotData &QUD_HzCopyPlotData::operator=(const QUD_HzCopyPlotData &other)
{
	if (&other != this)
	{
		mCopyData = other.mCopyData;
		mPrevData = other.mPrevData;

	}

	return *this;
}

QUD_HzCopyPlotData::QUD_HzCopyPlotData(const QUD_HzCopyPlotData& other)
{
	mCopyData = other.mCopyData;
	mPrevData = other.mPrevData;
	//Q_UNUSED(other)
}

QUD_HzCopyPlotData::~QUD_HzCopyPlotData()
{
	mCopyData = NULL;
	mPrevData = NULL;
}