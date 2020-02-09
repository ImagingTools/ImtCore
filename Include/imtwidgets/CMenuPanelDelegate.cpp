#include "CMenuPanelDelegate.h"
#include "CMenuPanel.h"


// Qt includes
#include "QtGui/QPainter"
#include "QtWidgets/QTreeView"

#include "QDebug"


namespace imtwidgets
{


void CMenuPanelDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	const QTreeView *pageTreePtr = qobject_cast<const QTreeView*>(option.widget);
	int indent = pageTreePtr->property("indent").toInt();
	int indentMax = pageTreePtr->property("indentMax").toInt();
	int padding = pageTreePtr->property("padding").toInt();

	QColor backgroundColor;

	if (option.state & QStyle::State_Selected){
		backgroundColor = pageTreePtr->property("ItemSelectedColor").value<QColor>();//qRgba(240, 200, 80, 255);
	}

	if (option.state & QStyle::State_MouseOver){
		backgroundColor = pageTreePtr->property("ItemMouserOverColor").value<QColor>();//qRgba(240, 220, 100, 50);

		if (option.state & QStyle::State_Selected){
			backgroundColor = pageTreePtr->property("ItemMouserOverSelectedColor").value<QColor>();//qRgba(240, 220, 100, 255);
		}
	}

	painter->save();
	QPen storedPen = painter->pen();
	painter->setRenderHint(QPainter::Antialiasing, true);

	// Draw background

	QRect singleEllipse;
	singleEllipse.setLeft(padding);
	singleEllipse.setRight(option.rect.right() - padding);
	singleEllipse.setTop(option.rect.top());
	singleEllipse.setBottom(option.rect.bottom());

	if (option.state & QStyle::State_Selected || option.state & QStyle::State_MouseOver){
		painter->setBrush(QBrush(backgroundColor));
//		painter->setPen(Qt::transparent);

		QRect backgroundRect = option.rect;
		if (indent != 0){
			//backgroundRect.setLeft(option.rect.height() / 2 + padding);
		//}
		//else{
			backgroundRect.setLeft(0);
		//}
			backgroundRect.setRight(option.rect.right() - option.rect.height() / 2 - padding);
			backgroundRect.setTop(option.rect.top());
			backgroundRect.setBottom(option.rect.bottom());
			painter->fillRect(backgroundRect, backgroundColor);

			QRect backgroundLeftEllipse;
			backgroundLeftEllipse.setLeft(padding);
			backgroundLeftEllipse.setRight(option.rect.height() + padding);
			backgroundLeftEllipse.setTop(option.rect.top());
			backgroundLeftEllipse.setBottom(option.rect.bottom());
			painter->drawEllipse(backgroundLeftEllipse);

			QRect backgroundRightEllipse;
			backgroundRightEllipse.setLeft(option.rect.right() - option.rect.height() - padding);
			backgroundRightEllipse.setRight(option.rect.right() - padding);
			backgroundRightEllipse.setTop(option.rect.top());
			backgroundRightEllipse.setBottom(option.rect.bottom());
			painter->drawEllipse(backgroundRightEllipse);
		}
		else {
			painter->drawEllipse(singleEllipse);
		}
	}

	// Calculate offset from treeview's left side:
	int offset = -indent;
	QModelIndex currentIndex = index;
	while (currentIndex.isValid()){
		offset += indent;
		currentIndex = currentIndex.parent();
	}

	// Draw icon:
	QRect iconRect;
	if (indent == 0){
		iconRect = singleEllipse;
	}
	else{
		iconRect = option.rect;
		iconRect.setLeft(offset + padding + 2 + padding);
		iconRect.setWidth(pageTreePtr->iconSize().width());
	}

	QIcon::Mode iconMode = QIcon::Mode::Normal;

	if (option.state & QStyle::State_Selected){
		iconMode = QIcon::Mode::Selected;
	}

	if (option.state & QStyle::State_MouseOver){
		iconMode = QIcon::Mode::Active;
	}

	if (!index.data(imtwidgets::CMenuPanel::DR_PAGE_ENABLED).toBool()){
		iconMode = QIcon::Mode::Disabled;
	}

	QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();

	QPixmap iconPixmap = icon.pixmap(
				pageTreePtr->iconSize().width(),
				pageTreePtr->iconSize().height(),
				iconMode);

	option.widget->style()->drawItemPixmap(
				painter, iconRect,
				Qt::AlignHCenter | Qt::AlignVCenter,
				iconPixmap);

	// Draw text:
	painter->setPen(storedPen);
	if (indent > 0){
		QRect textRect = option.rect;
		textRect.setLeft(iconRect.right() + 2 * padding);

		option.widget->style()->drawItemText(
					painter,
					textRect,
					Qt::AlignLeft | Qt::AlignVCenter,
					option.palette,
					true,
					index.data(Qt::DisplayRole).toString());
	}

	painter->restore();
}


} // namespace imtwidgets


