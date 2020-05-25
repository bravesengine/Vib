#include "HapticModifFunc.h"
//#include "qcustomplot.h"

#include "HapticPlotGraph.h"

#include "UndoCommands.h"
#include "HapticPlotDataSelectAction.h"

HaticModifGraph::HaticModifGraph(eModifGraphDataType t)
{
	mModifType = t;
}


bool HaticModifGraph::IsEqualHzPlotData(QList<HzPlotData> v1, QList<HzPlotData> v2)
{
	if (v1.size() != v2.size())
		return false;

	int Size = v1.size();

	for (int i = 0; i < Size; i++)
	{
		if (v1[i] != v2[i])
			return false;
	}

	return true;
}

bool HapticModifFuncHz::RunModif(HapticPlotGraph *HapticGraph, HapticPlotData* UserData, HapticPlotDataSelectAction *SelectAction,
	double StartKey, double EndKey, QCPItemRect* RectRangeItem)
{
	if (UserData == NULL || HapticGraph == NULL)
		return false;

	int RangeBegin = qRound((StartKey * 1000) / 3.125f);
	int RangeEnd = qRound((EndKey * 1000) / 3.125f);

	//-- Old Data.
	QList<HzPlotData> OldData = UserData->GetHzPlotData();


	QList<HzPlotData> hzHeadData = UserData->GetPlotDataRange(0, RangeBegin);
	QList<HzPlotData> hzBodyData = UserData->GetPlotDataRange(RangeBegin, RangeEnd);
	QList<HzPlotData> hzTailData = UserData->GetPlotDataRange(RangeEnd, UserData->GetLastUpperIndex());

	bool bUpdate = ModifHzData(UserData, hzBodyData);
	if (bUpdate == false)
		return false;



	HzPlotData firstData = hzBodyData.first();
	HzPlotData lastData = hzBodyData.last();
	float FixedStartCoord = (firstData.lowerIndex * 3.125) / 1000;
	float FixedEndCoord = (lastData.upperIndex * 3.125) / 1000;

	RectRangeItem->topLeft->setCoords(FixedStartCoord, AMP_LIMIT_VALUE);
	RectRangeItem->bottomRight->setCoords(FixedEndCoord, -AMP_LIMIT_VALUE);

    SelectAction->UpdateCursor(RectRangeItem);


	HzPlotData *PrevData = &lastData;

	for (QList<HzPlotData>::iterator it = hzTailData.begin(); it != hzTailData.end(); ++it)
	{
		HzPlotData *current = &(*it);
		int OldRange = current->upperIndex - current->lowerIndex;
		current->lowerIndex = PrevData->upperIndex;
		current->upperIndex = current->lowerIndex + OldRange;
		PrevData = current;
	}

	QList<HzPlotData> hzRetData;
	hzRetData = hzHeadData + hzBodyData + hzTailData;

	UserData->DataToReMakeTime(hzRetData);
	HapticGraph->SetGraph(hzRetData, UserData->GetTimeLength());

	QList<HzPlotData> NewData = UserData->GetHzPlotData();

	bool bIsEqual = IsEqualHzPlotData(OldData, NewData);

	//qDebug() << "IsEqual = " << bIsEqual;

	return !bIsEqual;
}

bool  HaticModifGraph::ModifAmpData(HapticPlotData* UserData, QList<HzPlotData> &hzBodyData, int IncreaseValue)
{
	if (UserData == NULL)
		return false;

	if (hzBodyData.size() == 0)
		return false;

	for (QList<HzPlotData>::iterator it = hzBodyData.begin(); it != hzBodyData.end(); ++it)
	{
		HzPlotData *current = &(*it);
		int RawDataValue = current->HapticRawData;

		RawDataValue += IncreaseValue;

		if (RawDataValue > AMP_LIMIT_VALUE) 
			RawDataValue = AMP_LIMIT_VALUE;

		if (RawDataValue < 0) 
			RawDataValue = 0;

		current->HapticRawData = RawDataValue;
		UserData->SetHzRawData(current->lowerIndex, current->upperIndex, current->HapticRawData);
	}

	return true;
}

bool  HaticModifGraph::ModifHzData(HapticPlotData* UserData, QList<HzPlotData> &hzBodyData)
{
	if (UserData == NULL)
		return false;

	if (hzBodyData.size() == 0)
		return false;

	bool bUpdate = false;
	HzPlotData *PrevData = NULL;
	for (QList<HzPlotData>::iterator it = hzBodyData.begin(); it != hzBodyData.end(); ++it)
	{
		HzPlotData *current = &(*it);

		if (mModifType == eModifGraphDataType::HZ_DOWN)
		{
			int DownHz = ((int)current->hzType + 1);
			if (DownHz > _20_HZ) DownHz = _20_HZ;

			if (PrevData)
			{
				current->lowerIndex = PrevData->upperIndex;
				current->upperIndex = current->lowerIndex + DownHz + 1;

			}
			else
			{
				if (DownHz != current->hzType)
					current->upperIndex = current->upperIndex + 1;
			}

			current->hzType = (eHzType)DownHz;
			bUpdate = true;
			//}
		}
		else if (mModifType == eModifGraphDataType::HZ_UP)
		{
			int UpHz = ((int)current->hzType - 1);
			if (UpHz < _320_HZ) UpHz = _320_HZ;

			if (PrevData)
			{
				current->lowerIndex = PrevData->upperIndex;
				current->upperIndex = current->lowerIndex + UpHz + 1;
			}
			else
			{
				if (UpHz != current->hzType)
					current->upperIndex = current->upperIndex - 1;
			}

			current->hzType = (eHzType)UpHz;
			bUpdate = true;
		}

		PrevData = current;
	}

	return bUpdate;
}


bool HaticModifGraph::GetCliptBoardData(QString MimeName, QList<HzPlotData> &OutList, int &ByteArraySize)
{

	QClipboard* _clipboard = QApplication::clipboard();
	const QMimeData* _mimeData = _clipboard->mimeData();
	QStringList _formats = _mimeData->formats();

	QByteArray _ClipBoardData = _mimeData->data(MimeName);
	if (_ClipBoardData.length() == 0)
		return false;

	ByteArraySize = _ClipBoardData.length();

	try
	{
		QDataStream inData(&_ClipBoardData, QIODevice::ReadOnly);

		int CopySize = 0;
		int RangeBegin = 0;
		int RangeEnd = 0;

		inData >> CopySize >> RangeBegin >> RangeEnd;

		for (int i = 0; i < CopySize; i++)
		{
			HzPlotData d;
			inData >> &d;

			// 유효성 검사.. return false;..
			OutList.push_back(d);
		}
	}
	catch (...)
	{

		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HapticModifFuncAmpUp
//
HapticModifFuncAmpUp::HapticModifFuncAmpUp() : HaticModifGraph(HaticModifGraph::eModifGraphDataType::AMP_UP)
{

}

bool HapticModifFuncAmpUp::RunModif(HapticPlotGraph *HapticGraph, HapticPlotData* UserData, HapticPlotDataSelectAction *SelectAction,
	double StartKey, double EndKey, QCPItemRect* RectRangeItem)
{

	if (UserData == NULL || HapticGraph == NULL)
		return false;

	int RangeBegin = qRound((StartKey * 1000) / 3.125f);
	int RangeEnd = qRound ( (EndKey * 1000) / 3.125f);

	QList<HzPlotData> hzBodyData = UserData->GetPlotDataRange(RangeBegin, RangeEnd);

	 bool bUpdate = ModifAmpData(UserData, hzBodyData, 1);
	if(bUpdate)
		HapticGraph->UpdateGraph(hzBodyData);

	return bUpdate;
}




////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HapticModifFuncAmpDown
//
HapticModifFuncAmpDown::HapticModifFuncAmpDown() : HaticModifGraph(HaticModifGraph::eModifGraphDataType::AMP_DOWN)
{
}

bool HapticModifFuncAmpDown::RunModif(HapticPlotGraph *HapticGraph, HapticPlotData* UserData, HapticPlotDataSelectAction *SelectAction,
	double StartKey, double EndKey, QCPItemRect* RectRangeItem)
{

	if (UserData == NULL || HapticGraph == NULL)
		return false;

	int RangeBegin = qRound((StartKey * 1000) / 3.125f);
	int RangeEnd = qRound((EndKey * 1000) / 3.125f);

	QList<HzPlotData> hzBodyData = UserData->GetPlotDataRange(RangeBegin, RangeEnd);

	bool bUpdate = ModifAmpData(UserData, hzBodyData, -1);
	if (bUpdate)
		HapticGraph->UpdateGraph(hzBodyData);

	return bUpdate;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HapticModifFuncHzDown
//
HapticModifFuncHzDown::HapticModifFuncHzDown() : HapticModifFuncHz(HaticModifGraph::eModifGraphDataType::HZ_DOWN)
{
	
}

bool HapticModifFuncHzDown::RunModif(HapticPlotGraph *HapticGraph, HapticPlotData* UserData, HapticPlotDataSelectAction *SelectAction, 
	double StartKey, double EndKey, QCPItemRect* RectRangeItem)
{
	return HapticModifFuncHz::RunModif(HapticGraph, UserData, SelectAction, StartKey, EndKey, RectRangeItem);
}




////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HapticModifFuncHzUp
//
HapticModifFuncHzUp::HapticModifFuncHzUp() : HapticModifFuncHz(HaticModifGraph::eModifGraphDataType::HZ_UP)
{
}

bool HapticModifFuncHzUp::RunModif(HapticPlotGraph *HapticGraph, HapticPlotData* UserData, HapticPlotDataSelectAction *SelectAction,
	double StartKey, double EndKey, QCPItemRect* RectRangeItem)
{
	return HapticModifFuncHz::RunModif(HapticGraph, UserData, SelectAction, StartKey, EndKey, RectRangeItem);
}





////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HapticModifFuncPasth
//
HapticModifFuncPasth::HapticModifFuncPasth() : HaticModifGraph(HaticModifGraph::eModifGraphDataType::PASTH_GRAHP)
{

}

bool HapticModifFuncPasth::RunModif(HapticPlotGraph *HapticGraph, HapticPlotData* UserData, HapticPlotDataSelectAction *SelectAction,
	double StartKey, double EndKey, QCPItemRect* RectRangeItem)
{
	if (UserData == NULL || HapticGraph == NULL)
		return false;

	int CopyByteArraySize;
	QList<HzPlotData> hzCopyData;	
	if (!GetCliptBoardData(MIME_COPY_GRAPH, hzCopyData, CopyByteArraySize))
		return false;

	if (hzCopyData.length()== 0)
		return false;

	int RangeBegin = qRound((StartKey * 1000) / 3.125f);
	int RangeEnd = qRound((EndKey * 1000) / 3.125f);

	QList<HzPlotData> hzHeadData = UserData->GetPlotDataRange(0, RangeBegin);
	QList<HzPlotData> hzBodyData = UserData->GetPlotDataRange(RangeBegin, RangeEnd);
	QList<HzPlotData> hzTailData = UserData->GetPlotDataRange(RangeEnd, UserData->GetLastUpperIndex());

	QList<HzPlotData> hzRetData;

	//if (hzCopyData.length() == 1)
	//{
	//	//hzRetData = hzHeadData + hzBodyData + hzCopyData + hzTailData;
		hzRetData = hzHeadData + hzCopyData + hzTailData;
	//}
	//else 
	//{	 
	//	// 여러개 잡고 했을때..
	//	hzRetData = hzHeadData + hzCopyData + hzTailData;
	//}

	qDebug() << "Pasth Grahp Size = " << hzCopyData.length() << " ,ClipBoardSize = " << CopyByteArraySize;

	HzPlotData *PrevData = NULL;
	for (QList<HzPlotData>::iterator it = hzRetData.begin(); it != hzRetData.end(); ++it)
	{
		HzPlotData *current = &(*it);		

		if (PrevData != NULL)
			current->lowerIndex = PrevData->upperIndex;
		else
			current->lowerIndex = 0;

		current->upperIndex = current->lowerIndex + current->hzType + 1;

		PrevData = current;
	}

	//-- Last Uppder...  갱신 해야되네..
	UserData->DataToReMakeTime(hzRetData);
	HapticGraph->SetGraph(hzRetData, UserData->GetTimeLength());

	int RightIndex = 0;
	if (hzHeadData.size() != 0)
		RightIndex = hzHeadData[hzHeadData.size() - 1].upperIndex;

	float tCoord = (RightIndex * 3.125) / 1000;
	RectRangeItem->topLeft->setCoords(tCoord, AMP_LIMIT_VALUE);
	RectRangeItem->bottomRight->setCoords(tCoord, -AMP_LIMIT_VALUE);
	SelectAction->UpdateCursor(RectRangeItem);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HapticModifFuncCut
//
HapticModifFuncCut::HapticModifFuncCut() : HaticModifGraph(HaticModifGraph::eModifGraphDataType::CUT_GRAHP)
{

}

bool HapticModifFuncCut::RunModif(HapticPlotGraph *HapticGraph, HapticPlotData* UserData, HapticPlotDataSelectAction *SelectAction,
	double StartKey, double EndKey, QCPItemRect* RectRangeItem)
{
	if (UserData == NULL || HapticGraph == NULL)
		return false;

	int CopyByteArraySize = 0;
	QList<HzPlotData> hzCopyData;

	if (!GetCliptBoardData(MIME_COPY_GRAPH, hzCopyData, CopyByteArraySize))
		return false;

	if (hzCopyData.length() == 0)
		return false;

	int RangeBegin = qRound((StartKey * 1000) / 3.125f);
	int RangeEnd = qRound((EndKey * 1000) / 3.125f);

	QList<HzPlotData> hzHeadData = UserData->GetPlotDataRange(0, RangeBegin);
	//QList<HzPlotData> hzBodyData = UserData->GetPlotDataRange(RangeBegin, RangeEnd);
	QList<HzPlotData> hzTailData = UserData->GetPlotDataRange(RangeEnd, UserData->GetLastUpperIndex());

	QList<HzPlotData> hzRetData;
	hzRetData = hzHeadData + hzTailData;

	qDebug() << "Cut Graph Size = " << hzCopyData.length() << ", CopyClipBoardSize = " << CopyByteArraySize;

	HzPlotData *PrevData = NULL;
	for (QList<HzPlotData>::iterator it = hzRetData.begin(); it != hzRetData.end(); ++it)
	{
		HzPlotData *current = &(*it);

		if (PrevData != NULL)
			current->lowerIndex = PrevData->upperIndex;
		else
			current->lowerIndex = 0;

		current->upperIndex = current->lowerIndex + current->hzType + 1;
		PrevData = current;
	}

	UserData->DataToReMakeTime(hzRetData);
	HapticGraph->SetGraph(hzRetData, UserData->GetTimeLength());

	float tCoord = (RangeBegin * 3.125) / 1000;

	RectRangeItem->topLeft->setCoords(tCoord, AMP_LIMIT_VALUE);
	RectRangeItem->bottomRight->setCoords(tCoord, -AMP_LIMIT_VALUE);
	SelectAction->UpdateCursor(RectRangeItem);

	//-- cursor.. RangeBegin 으로.. 맨끝으로?
	//SelectAction->UpdateCursor( )

	return true;
}