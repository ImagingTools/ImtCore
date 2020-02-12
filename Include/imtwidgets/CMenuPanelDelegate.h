#pragma once


// Qt includes
#include <QtWidgets/QStyledItemDelegate>

// ImtCore includes
#include <imtwidgets/CMenuPanel.h>


namespace imtwidgets
{


class CMenuPanelDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	explicit CMenuPanelDelegate(QTreeView* menuPanelPtr = nullptr): QStyledItemDelegate(menuPanelPtr) {}

	virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};


} // namespace imtwidgets


