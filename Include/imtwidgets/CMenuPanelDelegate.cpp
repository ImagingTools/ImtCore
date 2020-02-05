#include "CMenuPanelDelegate.h"
#include "qtreeview.h"
#include "qpainter.h"
#include "QDebug"
#include "ui_CMenuPanel.h"

void CMenuPanelDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	static int i = 0;
	qDebug() << option.state;

	//background
	QColor bgColor;
	int bgColorType(0);
	bgColorType = index.data(Qt::UserRole + 9).toInt();//custom flag I set to determine which color i want

	//color logic
	if (bgColorType == 0)
		bgColor = QColor(Qt::transparent);//default is transparent to retain alternate row colors
	else if (bgColorType == 1)
		bgColor = qRgba(237, 106, 106, 255);//red
	else if (bgColorType == 2)
		bgColor = qRgba(241, 167, 226, 255);//pink
	//etc...

	QStyleOptionViewItem opt(option);

	if (option.state & QStyle::State_Selected)//check if item is selected
	{
		//more color logic
		if (bgColorType == 0)
			bgColor = qRgba(150, 180, 200, 255);
		else
			bgColor = qRgba(bgColor.red() - 25, bgColor.green() - 25, bgColor.blue() - 25, 255);

		//background color won't show on selected items unless you do this
		opt.palette.setBrush(QPalette::Highlight, QBrush(bgColor));
	}

	if (option.state & QStyle::State_MouseOver)//check if item is hovered
	{
		//more color logic
		bgColor = qRgba(bgColor.red() - 25, bgColor.green() - 25, bgColor.blue() - 25, 255);

		if (option.state & QStyle::State_Selected)//check if it is hovered AND selected
		{
			//more color logic
			if (bgColorType == 0)
			{
				bgColor = qRgba(100, 150, 200, 255);
			}

			//background color won't show on selected items unless you do this
			opt.palette.setBrush(QPalette::Highlight, QBrush(bgColor));
		}
	}//set the backgroundBrush to our color. This affects unselected items.
	//opt.backgroundBrush = QBrush(bgColor);

	//draw the item background
	QPen penBack = painter->pen();
	//option.widget->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter);
	if (option.state & QStyle::State_Selected || option.state & QStyle::State_MouseOver){
		painter->fillRect(option.rect, QColor(Qt::transparent));
		QRect bgRect = option.rect;
		bgRect.setX(0);
		bgRect.setWidth(bgRect.width() - bgRect.height() / 2);
		painter->fillRect(bgRect, bgColor);
		QRect bgEllipse;
		bgEllipse.setX(bgRect.width() - bgRect.height() / 2);
		bgEllipse.setY(bgRect.y());
		bgEllipse.setWidth(bgRect.height());
		bgEllipse.setHeight(bgRect.height());
		painter->setBrush(QBrush(bgColor));
		painter->setPen(Qt::transparent);
		painter->drawEllipse(bgEllipse);
	}
	else {
	}
	//option.widget->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter);

	painter->setPen(penBack);

	const QTreeView *treeViewPtr = qobject_cast<const QTreeView*>(option.widget);

	int offset = -treeViewPtr->property("indent").toInt();
	QModelIndex check = index;
	while (check.isValid()){
		offset += treeViewPtr->property("indent").toInt();
		check = check.parent();
	}

	QRect iconRect = option.rect;
	iconRect.setLeft(iconRect.left() + 3 + offset);//offset it a bit to the right
	//draw in icon, this can be grabbed from Qt::DecorationRole
	//altho it appears icons must be set with setIcon()
	option.widget->style()->drawItemPixmap(painter, iconRect, Qt::AlignLeft | Qt::AlignVCenter, QIcon(index.data(Qt::DecorationRole).value<QIcon>()).pixmap(24, 24));

	//text
	QRect textRect = option.rect;
	textRect.setLeft(textRect.left() + 35 + offset);//offset it a bit to the right
	//draw in text, this can be grabbed from Qt::DisplayRole
	option.widget->style()->drawItemText(painter, textRect, Qt::AlignLeft | Qt::AlignVCenter, option.palette, true, index.data(Qt::DisplayRole).toString());
}
