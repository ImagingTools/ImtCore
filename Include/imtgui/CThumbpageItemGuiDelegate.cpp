#include <imtgui/CThumbpageItemGuiDelegate.h>


// Qt includes
#include <QtCore/QDir>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>



namespace imtgui
{

	static int maxCharacterCount = 12;
	static int fontSize = 24;
	static int minIconSize = 24;
	static const int minPadding = 7;


// public methods

CThumbpageItemGuiDelegate::CThumbpageItemGuiDelegate(const QStandardItemModel& itemModel, int horizontalSpacing, int verticalSpacing, QObject* parent)
	:BaseClass(parent),
	m_itemModel(itemModel)
{
	m_verticalMargin = int(qMax(verticalSpacing, 6) / 2);
	m_horizontalMargin = int(qMax(horizontalSpacing, 6) / 2);
}


void CThumbpageItemGuiDelegate::SetMargins(int horizontal, int vertical)
{
	m_horizontalMargin = horizontal > 0 ? horizontal : m_horizontalMargin;
	m_verticalMargin = vertical > 0 ? vertical : m_verticalMargin;
}


// reimplemented (QItemDelegate/)

QSize CThumbpageItemGuiDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const
{
	QFont font = option.font;
	font.setPointSize(fontSize);
	QFontMetrics fm(font);
	int minHeight = fm.height() + fm.lineSpacing() + minIconSize + 2* minPadding;
	int minWidth = fm.averageCharWidth() * (maxCharacterCount+1);
	if (option.rect.width() < minWidth || option.rect.height() < minHeight){
		return QSize(minWidth, minHeight);
	}
	else{
		return option.rect.size();
	}
}


void CThumbpageItemGuiDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStandardItem* itemPtr = m_itemModel.itemFromIndex(index);
	Q_ASSERT(itemPtr != NULL);

	QRect mainRect = option.rect;
	const QColor& color = option.palette.background().color();
	painter->fillRect(mainRect, color);

	if (!(itemPtr->data(DR_PAGE_ID).isValid())){
		return;
	}

	QIcon itemIcon = itemPtr->icon();

	painter->setRenderHint(QPainter::Antialiasing);
	painter->setRenderHint(QPainter::TextAntialiasing);
	QFont font;
	font.setPointSize(24);
	painter->setFont(font);

	QColor textColor = Qt::gray;
	painter->setPen(textColor);

	QRect drawArea = QRect(mainRect.left() + m_horizontalMargin, mainRect.top() + m_verticalMargin, mainRect.width() - 2*m_horizontalMargin, mainRect.height() - 2*m_verticalMargin);

	int iconPosVCenter = int(float(drawArea.height()) / 3 - minPadding);
	int iconSize = int(float(drawArea.height()) / 2 - minPadding);
	int textAreaTop = 2 * iconPosVCenter;
	int textAreaHeight = (drawArea.height() - 2*minPadding) - textAreaTop;
	int widthAvailable = drawArea.width() - 2 * minPadding;

	iconSize = qMin(iconSize, widthAvailable);

	QStyleOptionButton button;
	button.rect = drawArea;
	button.state = QStyle::State_Enabled;
	button.palette = option.palette;

	int iconRectLeft = drawArea.left() + (drawArea.width() - iconSize) / 2;
	QRect iconRect(iconRectLeft, drawArea.top() + minPadding + (textAreaTop - iconSize)/2, iconSize, iconSize);
	QPixmap itemPixmap = itemIcon.pixmap(QSize(iconRect.width(), iconRect.height()));

	int textRectTop = drawArea.top() + textAreaTop + minPadding;
	QRect textRect(drawArea.left() + minPadding, textRectTop, drawArea.width() - 2 * minPadding, textAreaHeight);

	if (option.state & QStyle::State_Selected){
		// move textRect & iconRect right and down as if pressed
		iconRect.adjust(minPadding, minPadding, 0, 0);
		textRect.adjust(minPadding, minPadding, 0, 0);
	}

	QStyle* appStylePtr = QApplication::style();
	Q_ASSERT(appStylePtr != nullptr);

	appStylePtr->drawControl(QStyle::CE_PushButton, &button, painter);
	painter->drawPixmap(iconRect, itemPixmap);
	painter->drawText(textRect, Qt::AlignCenter /* | Qt::AlignVCenter*/, itemPtr->text());

	if ((option.state & QStyle::State_MouseOver)){
		QColor curtainColor(255, 255, 255, 70);

		painter->fillRect(option.rect, curtainColor);
	}
}


QWidget* CThumbpageItemGuiDelegate::createEditor(QWidget* /*parent*/, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const
{
	return NULL;
}


} // namespace imtgui


