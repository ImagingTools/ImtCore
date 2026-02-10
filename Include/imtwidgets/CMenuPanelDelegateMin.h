// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtWidgets/QTreeView>

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


