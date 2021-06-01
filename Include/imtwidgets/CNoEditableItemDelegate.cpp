#include <imtwidgets/CNoEditableItemDelegate.h>


namespace imtwidgets
{


CNoEditableItemDelegate::CNoEditableItemDelegate(QObject* parent)
	:QStyledItemDelegate(parent)
{
}


CNoEditableItemDelegate::~CNoEditableItemDelegate()
{
}


// reimplemented (QStyledItemDelegate)

QWidget* CNoEditableItemDelegate::createEditor(
			QWidget*,
			const QStyleOptionViewItem &,
			const QModelIndex &) const
{
	return nullptr;
}


void CNoEditableItemDelegate::setEditorData(QWidget*, const QModelIndex&) const
{
}


void CNoEditableItemDelegate::setModelData(QWidget*, QAbstractItemModel*, const QModelIndex&) const
{
}


void CNoEditableItemDelegate::updateEditorGeometry(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const
{
}


} // namespace imtwidgets


