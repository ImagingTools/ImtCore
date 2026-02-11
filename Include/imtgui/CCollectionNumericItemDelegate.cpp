// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgui/CCollectionNumericItemDelegate.h>

// Qt includes
#include <QtWidgets/QSpinBox>

// ImtCore includes
#include <imtgui/CObjectCollectionViewComp.h>


namespace imtgui
{


CCollectionNumericItemDelegate::CCollectionNumericItemDelegate(Type type, QObject *parent)
	:CNumericItemDelegate(type, parent)
{
}


CCollectionNumericItemDelegate::~CCollectionNumericItemDelegate()
{
	m_collectionViewDelegate = nullptr;
}


void CCollectionNumericItemDelegate::SetCollectionViewDelegate(const ICollectionViewDelegate *collectionViewDelegate)
{
	m_collectionViewDelegate = collectionViewDelegate;
}


void CCollectionNumericItemDelegate::SetColumnId(const QByteArray &columnId)
{
	m_columnId = columnId;
}


// reimplemented (QStyledItemDelegate)

void CCollectionNumericItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	if (m_collectionViewDelegate != nullptr && model != nullptr && index.isValid()){
		QVariant value;
		if(m_type == CNumericItemDelegate::SpinInt){
			QSpinBox *spinbox = qobject_cast<QSpinBox*>(editor);

			value = spinbox->value();
		}
		else if(m_type == CNumericItemDelegate::SpinDouble){
			QDoubleSpinBox *spinbox = qobject_cast<QDoubleSpinBox*>(editor);

			value = spinbox->value();
		}

		QByteArray objectId = model->data(index, ICollectionViewDelegate::DR_OBJECT_ID).toByteArray();

		m_collectionViewDelegate->OnColumnItemDelegateEditFinished(objectId, m_columnId, value);
	}
}


} // namespace imtgui


