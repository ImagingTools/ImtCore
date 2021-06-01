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
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

private:
	const ICollectionViewDelegate* m_collectionViewDelegate;
	QByteArray m_columnId;
 };


} // namespace imtgui


