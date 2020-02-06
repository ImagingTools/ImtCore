#include "CMenuPanelDelegate.h"


// Qt includes
#include "QtGui/QPainter"
#include "QtWidgets/QTreeView"

#include "QDebug"

void CMenuPanelDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QColor bgColor;
	QStyleOptionViewItem opt(option);

	if (option.state & QStyle::State_Selected){
		bgColor = qRgba(0, 255, 0, 255);
	}

	if (option.state & QStyle::State_MouseOver){
		bgColor = qRgba(255, 0, 0, 255);

		if (option.state & QStyle::State_Selected){
			bgColor = qRgba(0, 0, 255, 255);
		}
	}

	
	const QTreeView *treeViewPtr = qobject_cast<const QTreeView*>(option.widget);
	int indent = treeViewPtr->property("indent").toInt();
	int indentMax = treeViewPtr->property("indentMax").toInt();
	int padding = treeViewPtr->property("padding").toInt();

	QPen savePen = painter->pen();
	QPainter::RenderHints saveRenderHints = painter->renderHints();
	painter->setRenderHint(QPainter::Antialiasing, true);

	// Draw background

	if (option.state & QStyle::State_Selected || option.state & QStyle::State_MouseOver){
		QRect backgroundRect = option.rect;
		if (indent == 0){
			backgroundRect.setLeft(option.rect.height() / 2 + padding);
		}
		else{
			backgroundRect.setLeft(0);
		}
		backgroundRect.setRight(option.rect.right() - option.rect.height() / 2 - padding);
		backgroundRect.setTop(option.rect.top());
		backgroundRect.setBottom(option.rect.bottom());
		painter->fillRect(backgroundRect, bgColor);

		painter->setBrush(QBrush(bgColor));
		painter->setPen(Qt::transparent);

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

	painter->setPen(savePen);

	// Calculate offset from treeview left side

	int offset = -indent;
	QModelIndex currentIndex = index;
	while (currentIndex.isValid()){
		offset += indent;
		currentIndex = currentIndex.parent();
	}

	// Draw icon

	QRect iconRect = option.rect;
	iconRect.setLeft(offset + padding + 2 + padding);
	iconRect.setWidth(treeViewPtr->iconSize().width());
	option.widget->style()->drawItemPixmap(painter, iconRect, Qt::AlignHCenter | Qt::AlignVCenter, QIcon(index.data(Qt::DecorationRole).value<QIcon>()).pixmap(treeViewPtr->iconSize().width(), treeViewPtr->iconSize().height()));

	// Draw text

	if (indent > 0){
		QRect textRect = option.rect;
		textRect.setLeft(iconRect.right() + 2 * padding);
		option.widget->style()->drawItemText(painter, textRect, Qt::AlignLeft | Qt::AlignVCenter, option.palette, true, index.data(Qt::DisplayRole).toString());
	}

	painter->setRenderHints(saveRenderHints);
}
