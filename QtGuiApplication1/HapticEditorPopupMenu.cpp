#include "HapticEditorPopupMenu.h"
#include "HapticEditorWindowHud.h"
#include "HapticPlotDataSelectAction.h"
#include "Device/HapticDeviceManager.h"
#include "qdir.h"

using namespace HT;
HapticEditorPopupMenu::HapticEditorPopupMenu(HapticEditorWindowHud *parent)//, QToolBar *toolBar)
{
	if (parent == NULL)
		return;

	mMenu = NULL;
	m_Parent = parent;
	createActions();
	createMenus(parent);
}

HapticEditorPopupMenu::~HapticEditorPopupMenu()
{

}

static QIcon CreateIcon(QString icoFile)
{
	QDir dir(".");
	QImage q(dir.absolutePath() + "/Resources/" + icoFile);// "Play.png");
	QIcon Icon = QIcon(QPixmap::fromImage(q));
	return Icon;
}

void HapticEditorPopupMenu::onCutGraph()
{
	if (HapticDeviceManager::instance()->IsRunning())
		return;

	qDebug() << "onCutGraph";
	bool bRet = m_Parent->BeginModifGraphData(HapticEditorWindowHud::POPMENU, HaticModifGraph::eModifGraphDataType::CUT_GRAHP, true);

	if(bRet)
		m_Parent->UpdateModifGraphData(HapticEditorWindowHud::POPMENU, HaticModifGraph::eModifGraphDataType::CUT_GRAHP);
	
	m_Parent->EndModifGraphData(HapticEditorWindowHud::POPMENU, HaticModifGraph::eModifGraphDataType::CUT_GRAHP, bRet);
}

void HapticEditorPopupMenu::onCopyGraph()
{
	if (HapticDeviceManager::instance()->IsRunning())
		return;

	if (m_Parent == NULL)
		return;

	m_Parent->CopySelectedGraphtoClipBoard();
}

void HapticEditorPopupMenu::onPasteGraph()
{
	if (HapticDeviceManager::instance()->IsRunning())
		return;

	bool bRet = m_Parent->BeginModifGraphData(HapticEditorWindowHud::POPMENU, HaticModifGraph::eModifGraphDataType::PASTH_GRAHP);
	m_Parent->UpdateModifGraphData(HapticEditorWindowHud::POPMENU, HaticModifGraph::eModifGraphDataType::PASTH_GRAHP);

	m_Parent->EndModifGraphData(HapticEditorWindowHud::POPMENU, HaticModifGraph::eModifGraphDataType::PASTH_GRAHP, bRet);
}

void HapticEditorPopupMenu::onViewInfo()
{
	/*QCPItemRect  *SelectItem = m_Parent->SelectAction()->GetCurItemRect();
	if (SelectItem == NULL)
		return;

	float StartCoord = SelectItem->topLeft->coords().x();
	float EndCoord = SelectItem->bottomRight->coords().x();

	int hzIndex1 = (StartCoord * 1000) / 3.125f;
	int hzIndex2 = (EndCoord * 1000) / 3.125f;

	QPoint hzGraphIndex = m_Parent->GetPlotData()->GetHzDataIndex(hzIndex1, hzIndex2, 1);
	if (hzGraphIndex.x() == -1 || hzGraphIndex.y() == -1)
		return;

	const QList<HzPlotData> PloHztData = m_Parent->GetPlotData()->GetHzPlotData();	
	int begiDataIndex = PloHztData[hzGraphIndex.x()].lowerIndex;
	int endDataIndex = PloHztData[hzGraphIndex.y()].upperIndex;*/
	//int xxx = 0;
}

void HapticEditorPopupMenu::createActions()
{		
	mCutAct = new QAction(CreateIcon("Cut.png"), tr("&Cut Graph"), this);
	mCutAct->setShortcut(tr("Run"));	
	connect(mCutAct, SIGNAL(triggered()), this, SLOT(onCutGraph()));

	mCopyAct = new QAction(CreateIcon("Copy.png"), tr("&Copy Graph"), this);
	mCopyAct->setShortcut(tr("Copy"));
	connect(mCopyAct, SIGNAL(triggered()), this, SLOT(onCopyGraph()));

	mPasteAct = new QAction(CreateIcon("Paste.png"), tr("&Paste Graph"), this);
	mPasteAct->setShortcut(tr("Paste"));
	connect(mPasteAct, SIGNAL(triggered()), this, SLOT(onPasteGraph()));

	mViewInfo = new QAction(CreateIcon("Paste.png"), tr("&ViewInfo"), this);
	mViewInfo->setShortcut(tr("mViewInfo"));
	connect(mViewInfo, SIGNAL(triggered()), this, SLOT(onViewInfo()));
}


void HapticEditorPopupMenu::createMenus(HapticEditorWindowHud *parent)
{
	mMenu= new QMenu(this);

	mMenu->addAction(mCutAct);
	mMenu->addAction(mCopyAct);
	mMenu->addAction(mPasteAct);
	//mMenu->addAction(mViewInfo);
}

void HapticEditorPopupMenu::ShowPopUp(QPoint pt)
{
	if (mMenu == NULL)
		return;

	pt = m_Parent->mapToGlobal(pt);

	mMenu->popup(pt);
}