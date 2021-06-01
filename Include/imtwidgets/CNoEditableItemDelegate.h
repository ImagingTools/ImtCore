#pragma once


// Qt includes
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStyledItemDelegate>
//#include <QtCore/QModelIndex>
//#include <QtCore/QSize>

namespace imtwidgets
{


class CNoEditableItemDelegate : public QStyledItemDelegate
 {
     Q_OBJECT
public:

	CNoEditableItemDelegate(QObject *parent = 0);
	~CNoEditableItemDelegate();

	// reimplemented (IMenuPanelDelegate)
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
						   const QModelIndex &index) const override;
	 void setEditorData(QWidget *editor, const QModelIndex &index) const override;
     void setModelData(QWidget *editor, QAbstractItemModel *model,
					   const QModelIndex &index) const override;
     void updateEditorGeometry(QWidget *editor,
		 const QStyleOptionViewItem &option, const QModelIndex &index) const override;

 };


} // namespace imtwidgets


