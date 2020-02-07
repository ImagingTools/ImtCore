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
	explicit CMenuPanelDelegate(imtwidgets::CMenuPanel *menuPanelPtr = nullptr){}

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


} // namespace imtwidgets


