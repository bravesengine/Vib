#include "GenDocument.h"
#include <QStandardItemModel>
#include "Parser/TableManager.h"
#include "Samples.h"
#include "ComboBoxDelegate.h"
#include "QtHapticMain.h"
#include "HapticEditorWindowHud.h"
using namespace HT;

GenDocument::GenDocument(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
	
	mbOk = false;

	connect(ui.CancleBtn, SIGNAL(clicked()), this, SLOT(clicked_Cancle()));
	connect(ui.OkBtn, SIGNAL(clicked()), this, SLOT(clicked_Ok()));

	mModel = new QStandardItemModel(0, 3);

	QStringList headerLabels;
	headerLabels.append("Title");
	headerLabels.append("Desc");
	headerLabels.append("Repeat");

	mModel->setHorizontalHeaderLabels(headerLabels);

	ui.tableView->setModel(mModel);
	//ui.tableView->horizontalHeader()->setcolorsetStyleSheet("background - color:rgb(48, 48, 72);color:white;
	//);

	ui.tableView->setStyleSheet("QHeaderView::section { background-color:gray }");
	

	ui.tableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

	//ui.tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

	//ui.tableView->setColumnWidth(1, 200);
	ui.tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	
	//ui.tableView->setStrsetStretchLastSection(true);

	//ui.tableView->horizontalHeader()->setho
	QSharedPointer<Samples> data = TableManager::instance()->GetSampleTable();
	if (data.get() != NULL)
	{
		for (int i = 0; i < data.get()->countOfSamples(); i++)
		{
			mModel->insertRow(i);

			Sample sampleData = data.get()->getSampleAt(i);

			QModelIndex index1 = mModel->index(i, 0, QModelIndex());
			mModel->setData(index1, QVariant(sampleData.getTitle()));

			QModelIndex index2 = mModel->index(i, 1, QModelIndex());
			mModel->setData(index2, QVariant(sampleData.getDesc()));

			QModelIndex index3 = mModel->index(i, 2, QModelIndex());
			mModel->setData(index3, QVariant(sampleData.getDefaultRepeat()));

			QStandardItem *item1 = mModel->takeItem(i, 0);
			QStandardItem *item2 = mModel->takeItem(i, 1);
			
			item1->setFlags(item1->flags() ^ Qt::ItemIsEditable);
			mModel->setItem(i, 0, item1);

			item2->setFlags(item2->flags() ^ Qt::ItemIsEditable);
			mModel->setItem(i, 1, item2);

			QStringList comboItems;

			for (int i = 0; i < sampleData.getMaxRepeat(); i++)
			{
				comboItems.append( QString("%1").arg(i + 1));
			}

			ui.tableView->setItemDelegateForColumn(2, new ComboBoxDelegate(NULL, comboItems));
		}		
	}



		//QStandardItem *takeItem(int row, int column = 0);

	ui.tableView->setModel(mModel);

	ui.tableView->verticalHeader()->setVisible(false);

	//ui.tableView->horizontalHeader()->setStretchLastSection(true);
	////tableView.setWindowTitle(QObject::tr("blabla.."));
}

GenDocument::~GenDocument()
{	
}

void GenDocument::clicked_Cancle()
{
	mSelectionIndex = -1;
	this->hide();
}

void GenDocument::clicked_Ok()
{
	QItemSelectionModel *select = ui.tableView->selectionModel();
	QModelIndexList selection = select->selectedRows();
	if (selection.count() == 0)
		return;

	mbOk = true;
	qDebug() << "GenDocument : Current Select Count  = " << selection.count() << "Index = " << selection[0].row();
	mSelectionIndex = selection[0].row();

	QStandardItem *ComboItem =  mModel->takeItem(mSelectionIndex, 2);
	mRepeatCount = ComboItem->text().toInt();

	//UpdateWorkDesc

	this->hide();
}

void GenDocument::resizeEvent(QResizeEvent *event)
{
	ui.tableView->setColumnWidth(0, ui.tableView->width() * 0.4f);
	ui.tableView->setColumnWidth(1, ui.tableView->width() * 0.4f);
	ui.tableView->setColumnWidth(2, (ui.tableView->width() * 0.19f));
	QDialog::resizeEvent(event);
}
