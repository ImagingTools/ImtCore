// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtwidgets/CNumericItemDelegate.h>
#include <imtgui/ICollectionViewDelegate.h>


namespace imtgui
{


class CCollectionNumericItemDelegate : public imtwidgets::CNumericItemDelegate
 {
	Q_OBJECT
public:
	CCollectionNumericItemDelegate(Type type = CCollectionNumericItemDelegate::SpinInt, QObject *parent = 0);
	~CCollectionNumericItemDelegate();

	void SetCollectionViewDelegate(const ICollectionViewDelegate* collectionViewDelegate);
	void SetColumnId(const QByteArray &columnId);

	// reimplemented (QStyledItemDelegate)
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

private:
	const ICollectionViewDelegate* m_collectionViewDelegate;
	QByteArray m_columnId;
 };


} // namespace imtgui


