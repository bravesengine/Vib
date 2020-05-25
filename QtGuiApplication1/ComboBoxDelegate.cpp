
#include "ComboBoxDelegate.h"

#include <qcombobox.h>
//#include <QSpinBox>

//! [0]
ComboBoxDelegate::ComboBoxDelegate(QObject *parent, QStringList &comboItems)
	: QStyledItemDelegate(parent)
{
	mItems = comboItems;
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const
{

	QComboBox *editor = new QComboBox(parent);
	for (int i = 0; i < mItems.size(); i++)
	{
		editor->insertItem(i, mItems[i]);
	}
	

	

	return editor;
  //  editor->setMinimum(0);
  //  editor->setMaximum(100); 
	return NULL;
}

//! [1]
//! [2]
void ComboBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
	//int value = index.model()->data(index, Qt::EditRole).toInt();

	QString value = index.model()->data(index, Qt::EditRole).toString();

	QComboBox *ComboEditor = qobject_cast<QComboBox*>(editor);

	int Index = ComboEditor->findText(value);

	ComboEditor->setCurrentIndex(Index);
	//editor->setCurrentIndex(2);


	//QComboBox *comboBox = static_cast<QComboBox*>(editor);
	
}
//! [2]

//! [3]
void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
	QComboBox *comboBox = static_cast<QComboBox*>(editor);
	model->setData(index, comboBox->currentText(), Qt::EditRole);

   // QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
   // spinBox->interpretText();
   // int value = spinBox->value();

   // model->setData(index, value, Qt::EditRole);
}
//! [3]

//! [4]
void ComboBoxDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    
	editor->setGeometry(option.rect);
}
//! [4]
