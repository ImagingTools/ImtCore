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


// reimplemented (QItemDelegate)

QSize CThumbpageItemGuiDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const
{
	return option.decorationSize;
}


void CThumbpageItemGuiDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStandardItem* itemPtr = m_itemModel.itemFromIndex(index);
	Q_ASSERT(itemPtr != NULL);

	if (!(itemPtr->data(DR_PAGE_ID).isValid())){
		return;
	}

	QIcon itemIcon = itemPtr->icon();
	QRect mainRect = option.rect;
	QRect drawArea = QRect(mainRect.left() + m_horizontalMargin, mainRect.top() + m_verticalMargin,
				mainRect.width() - 2*m_horizontalMargin, mainRect.height() - 2*m_verticalMargin);

	int minPadding = 7;

	QStyleOptionButton button;
//	button.text = itemPtr->text();
	button.rect = drawArea;
	button.state = QStyle::State_Enabled;
	//button.icon = itemIcon;
	QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);

	

	int oneThirdthHeight = int(float(drawArea.height()) / 3 - minPadding);
	int widthAvailable = drawArea.width() - 2 * minPadding;

	int iconSize = qMin(2 * oneThirdthHeight, widthAvailable);
	int fontSize = qMin(oneThirdthHeight, 10);

	painter->setRenderHint(QPainter::Antialiasing);
	painter->setRenderHint(QPainter::TextAntialiasing);

/*	QColor backgroundColor(160, 70, 70);	//TODO
	
	int shadowOffset = 7;

	if (option.state & QStyle::State_Selected){
		drawArea.adjust(shadowOffset, -shadowOffset, 0, - 2 * shadowOffset);
		painter->fillRect(drawArea, backgroundColor);
	}
	else {
		QRectF shadowRect = drawArea;
		shadowRect.adjust(shadowOffset, -shadowOffset, 0, -2*shadowOffset);
		drawArea.adjust(0, 0, -shadowOffset, -shadowOffset);
		QColor shadowColor(18, 18, 18);

		painter->fillRect(shadowRect, shadowColor);
		painter->fillRect(drawArea, backgroundColor);
	}*/

	int iconRectLeft = drawArea.left() + (drawArea.width() - iconSize) / 2;
	QRect iconRect(iconRectLeft, drawArea.top() + minPadding, iconSize, iconSize);
	QPixmap itemPixmap = itemIcon.pixmap(QSize(iconRect.width(), iconRect.height()));
	painter->drawPixmap(iconRect, itemPixmap);

	QFont font;
	font.setPointSize(14);
	painter->setFont(font);

	const QColor& textColor = option.palette.text().color(); // QColor(230, 230, 230);	//TODO
	painter->setPen(textColor);

	int textRectTop = drawArea.top() + 2 * minPadding + iconSize;
	QRect textRect(drawArea.left() + minPadding, textRectTop, drawArea.width() - 2*minPadding, oneThirdthHeight);
	painter->drawText(
		textRect,
		Qt::AlignHCenter | Qt::AlignBottom,
		itemPtr->text()
	);

	if (!(option.state & QStyle::State_Selected)) {
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


