// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtWidgets/QStyledItemDelegate>


namespace imtwidgets
{


class CNoEditableItemDelegate : public QStyledItemDelegate
 {
	Q_OBJECT
public:

	CNoEditableItemDelegate(QObject *parent = 0);
	virtual ~CNoEditableItemDelegate();

	// reimplemented (QStyledItemDelegate)
	virtual QWidget* createEditor(
				QWidget* parent,
				const QStyleOptionViewItem& option,
				const QModelIndex& index) const override;
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	virtual void setModelData(QWidget* editor, QAbstractItemModel*model, const QModelIndex& index) const override;
	virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};


} // namespace imtwidgets


