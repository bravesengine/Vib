#pragma once

#include <QWidget>
#include <QDialog>
#include <ui_GenDocument.h>

class QStandardItemModel;
class GenDocument : public QDialog
{
	Q_OBJECT

public:	
	GenDocument(QWidget *parent = Q_NULLPTR);
	~GenDocument();

	bool IsOk() {
		return mbOk;
	}

	int GetSelectionIndex()
	{
		return mSelectionIndex;
	}
	int GetRepeatCount()
	{
		return mRepeatCount;
	}

private:
	Ui::genDocument ui;

private slots:
	void clicked_Cancle();
	void clicked_Ok();
private:
	bool mbOk = false;

	void resizeEvent(QResizeEvent* event);

	int mSelectionIndex =0;
	int mRepeatCount = 0;

	QStandardItemModel *mModel;

};
