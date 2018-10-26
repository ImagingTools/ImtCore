#include <imtgui/CThumbpageItemGuiDelegate.h>


// Qt includes
#include <QtGui/QPainter>
#include <QtCore/QDir>
#include <QApplication>


// ACF includes
#include <istd/IInformationProvider.h>
#include <iimg/IBitmapProvider.h>
#include <iimg/CBitmap.h>
#include <icomp/CComponentBase.h>
#include <ilog/CExtMessage.h>
#include <i2d/CPolygon.h>


namespace imtgui
{


// public methods

CThumbpageItemGuiDelegate::CThumbpageItemGuiDelegate(const QStandardItemModel& itemModel, int horizontalSpacing, int verticalSpacing, QObject* parent)
	:BaseClass(parent),
	m_itemModel(itemModel)
{
	m_verticalMargin = int(qMax(verticalSpacing, 6) / 2);
	m_horizontalMargin = int(qMax(horizontalSpacing, 6) / 2);
}


// reimplemented (QItemDelegate/)

QSize CThumbpageItemGuiDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const
{
	return option.decorationSize;
}


void CThumbpageItemGuiDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStandardItem* itemPtr = m_itemModel.itemFromIndex(index);
	Q_ASSERT(itemPtr != NULL);

//ugly solution for tableview item color
	QRect mainRect = option.rect;
	const QColor& color = option.palette.background().color();
	painter->fillRect(mainRect, color);

	if (!(itemPtr->data(DR_PAGE_ID).isValid())){
		return;
	}

	QIcon itemIcon = itemPtr->icon();

// basic settings
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setRenderHint(QPainter::TextAntialiasing);
	QFont font;
	font.setPointSize(14);
	painter->setFont(font);

	const QColor& textColor = option.palette.text().color();
	painter->setPen(textColor);

// geometry parameters
	QRect drawArea = QRect(mainRect.left() + m_horizontalMargin, mainRect.top() + m_verticalMargin,
				mainRect.width() - 2*m_horizontalMargin, mainRect.height() - 2*m_verticalMargin);

	int minPadding = 7;

	int oneThirdth = int(float(drawArea.height()) / 3 - minPadding);
	int oneHalf = int(float(drawArea.height()) / 2 - minPadding);
	int twoThirdth = 2 * oneThirdth;
	int widthAvailable = drawArea.width() - 2 * minPadding;

	int iconSize = qMin(oneHalf, widthAvailable);
	int fontSize = 18;

// button appearance
	QStyleOptionButton button;
	button.rect = drawArea;
	button.state = QStyle::State_Enabled;
	button.palette = option.palette;

// icon parameters
	int iconRectLeft = drawArea.left() + (drawArea.width() - iconSize) / 2;
	QRect iconRect(iconRectLeft, drawArea.top() + minPadding + (twoThirdth - iconSize)/2, iconSize, iconSize);
	QPixmap itemPixmap = itemIcon.pixmap(QSize(iconRect.width(), iconRect.height()));

// text parameters
	int textRectTop = drawArea.top() + twoThirdth + minPadding;
	QRect textRect(drawArea.left() + minPadding, textRectTop, drawArea.width() - 2 * minPadding, oneThirdth);

	if (option.state & QStyle::State_Selected){
		// move textRect & iconRect right and down as if pressed
		iconRect.adjust(minPadding, minPadding, 0, 0);
		textRect.adjust(minPadding, minPadding, 0, 0);
	}
	else{
		//do nothing
	}

//draw
	QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
	painter->drawPixmap(iconRect, itemPixmap);
	painter->drawText(textRect, Qt::AlignCenter /* | Qt::AlignVCenter*/, itemPtr->text());

//cover if hovered
	if ((option.state & QStyle::State_MouseOver)){
		//highlight item under the cursor
		QColor curtainColor(255, 255, 255, 70);
		painter->fillRect(option.rect, curtainColor);
	}
}


QWidget* CThumbpageItemGuiDelegate::createEditor(QWidget* /*parent*/, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const
{
	return NULL;
}


void CThumbpageItemGuiDelegate::SetStyleSheet()
{

}

} // namespace imtgui


