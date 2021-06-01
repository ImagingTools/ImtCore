#include <imtgui/CCollectionNumericItemDelegate.h>


// Qt includes
#include <QtGui/QPainter>
#include <QtCore/QDebug>

// Imt includes
#include <imtgui/CObjectCollectionViewComp.h>


namespace imtgui
{


CCollectionNumericItemDelegate::CCollectionNumericItemDelegate(Type type, QObject *parent)
	: CNumericItemDelegate(type, parent)
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

void CCollectionNumericItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
										const QModelIndex &index) const
{
	if (m_collectionViewDelegate != nullptr && model != nullptr && index.isValid()){
		QVariant value;
		if(m_type == CNumericItemDelegate::SpinInt)
		{
			QSpinBox *spinbox = qobject_cast<QSpinBox*>(editor);
			value = spinbox->value();
		}
		else if(m_type == CNumericItemDelegate::SpinDouble)
		{
			QDoubleSpinBox *spinbox = qobject_cast<QDoubleSpinBox*>(editor);
			value = spinbox->value();
		}
		QByteArray objectId = model->data(index, CObjectCollectionViewComp::DR_OBJECT_ID).toByteArray();
		m_collectionViewDelegate->OnColumnItemDelegateEditFinished(objectId, m_columnId, value);
	}
}





} // namespace imtgui
