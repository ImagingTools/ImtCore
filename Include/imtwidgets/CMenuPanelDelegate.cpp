#include "imtwidgets/CMenuPanelDelegate.h"


// Qt includes
#include "QtGui/QPainter"
#include "QtWidgets/QTreeView"

// ImtCore includes
#include "imtwidgets/CMenuPanel.h"


namespace imtwidgets
{


QSize CMenuPanelDelegate::sizeHint(const QStyleOptionViewItem& /*option*/, const QModelIndex& index) const
{
	QTreeView* pageTreePtr = qobject_cast<QTreeView*>(parent());
	Q_ASSERT(pageTreePtr != nullptr);

	int indent = 0;
	if (pageTreePtr->property("indentMax").isValid()){
		indent = pageTreePtr->property("indentMax").toInt();
	}
	int padding = 0;
	if (pageTreePtr->property("padding").isValid()){
		padding = pageTreePtr->property("padding").toInt();
	}

	QSize size;
	size.setHeight(pageTreePtr->iconSize().height() + 4 * padding);

	int offset = -indent;
	QModelIndex currentIndex = index;
	while (currentIndex.isValid()){
		offset += indent;
		currentIndex = currentIndex.parent();
	}

	int width = offset + pageTreePtr->iconSize().width() + 4 * padding;
	QFontMetrics fm = pageTreePtr->fontMetrics();
	width += fm.boundingRect(index.data(Qt::DisplayRole).toString()).width() + 3 * padding;

	size.setWidth(width);

	return size;
}


void CMenuPanelDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	const QTreeView *pageTreePtr = qobject_cast<const QTreeView*>(option.widget);
	Q_ASSERT(pageTreePtr != nullptr);

	int indent = pageTreePtr->property("indent").toInt();
	int padding = pageTreePtr->property("padding").toInt();

	QColor backgroundColor = Qt::transparent;

	if (option.state & QStyle::State_Selected){
		backgroundColor = pageTreePtr->property("ItemSelectedColor").value<QColor>();
	}

	if (option.state & QStyle::State_MouseOver){
		const QStandardItemModel* modelPtr = dynamic_cast<const QStandardItemModel*>(index.model());
		Q_ASSERT(modelPtr != nullptr);

		if (modelPtr->itemFromIndex(index)->isEnabled()){
			backgroundColor = pageTreePtr->property("ItemMouserOverColor").value<QColor>();

			if (option.state & QStyle::State_Selected){
				backgroundColor = pageTreePtr->property("ItemMouserOverSelectedColor").value<QColor>();
			}
		}
	}

	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);

	QRect singleEllipse;
	singleEllipse.setLeft(padding);
	singleEllipse.setRight(padding + (option.rect.height() - 2 * padding) - 1);
	singleEllipse.setTop(option.rect.top() + padding);
	singleEllipse.setBottom(option.rect.bottom() - padding);

	// Draw background:
	if (option.state & QStyle::State_Selected || option.state & QStyle::State_MouseOver){
		painter->setBrush(QBrush(backgroundColor));

		QRect backgroundRect = option.rect;
		if (indent != 0){
			painter->setPen(Qt::transparent);

			backgroundRect.setLeft(0);
			backgroundRect.setRight(option.rect.right() - (option.rect.height()  - 2 * padding) / 2 - padding);
			backgroundRect.setTop(option.rect.top() + padding);
			backgroundRect.setBottom(option.rect.bottom() - padding);
			painter->fillRect(backgroundRect, backgroundColor);

			QRect backgroundRightEllipse;
			backgroundRightEllipse.setLeft(option.rect.right() - (option.rect.height() - 2 * padding) - padding);
			backgroundRightEllipse.setRight(option.rect.right() - padding);
			backgroundRightEllipse.setTop(option.rect.top() + padding);
			backgroundRightEllipse.setBottom(option.rect.bottom() - padding);
			painter->drawEllipse(backgroundRightEllipse);
		}
		else {
			QRect drawRect = singleEllipse.adjusted(-padding / 2, -padding / 2, padding / 2, padding / 2);

			painter->setPen(pageTreePtr->property("ItemSelectedContourColor").value<QColor>());
			painter->drawEllipse(drawRect);
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
	iconRect = singleEllipse;
	if (indent != 0){
		iconRect.setLeft(singleEllipse.left() + offset);
		iconRect.setRight(singleEllipse.right() + offset);
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
	painter->setPen(pageTreePtr->property("ItemTextColor").value<QColor>());
	if (indent > 0){
		QRect textRect = option.rect;
		textRect.setLeft(iconRect.right() + padding);

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


