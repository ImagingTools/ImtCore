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
			QWidget* /* widget */,
			const QStyleOptionViewItem& /* options */,
			const QModelIndex& /* index */) const
{
	return nullptr;
}


void CNoEditableItemDelegate::setEditorData(QWidget* /* widget */, const QModelIndex& /* index */) const
{
}


void CNoEditableItemDelegate::setModelData(QWidget* /* widget */, QAbstractItemModel* /* model */, const QModelIndex& /* index */) const
{
}


void CNoEditableItemDelegate::updateEditorGeometry(QWidget* /* widget */, const QStyleOptionViewItem& /* item */, const QModelIndex& /* index */) const
{
}


} // namespace imtwidgets


