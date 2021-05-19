#pragma once


// ImtCore includes
#include <imtwidgets/CMenuPanelDelegate.h>

namespace imtwidgets
{


class CMenuPanelDelegateMin: public CMenuPanelDelegate
{
public:
	explicit CMenuPanelDelegateMin(QTreeView* menuPanelPtr = nullptr);

	// reimplemented (IMenuPanelDelegate)
	int GetMinimumWidth() override;

protected:
	// reimplemented (QItemDelegate)
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};


} // namespace imtwidgets


