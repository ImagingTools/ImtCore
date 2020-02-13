#pragma once


// Qt includes
#include <QtWidgets/QStyledItemDelegate>

// ImtCore includes
#include <imtwidgets/CMenuPanel.h>


namespace imtwidgets
{


class CMenuPanelDelegate: public QStyledItemDelegate
{
public:
	explicit CMenuPanelDelegate(QTreeView* menuPanelPtr = nullptr): QStyledItemDelegate(menuPanelPtr) {}

protected:
	// reimplemented (QItemDelegate)
	virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};


} // namespace imtwidgets


