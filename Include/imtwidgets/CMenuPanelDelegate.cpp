#include <imtwidgets/CMenuPanelDelegate.h>


// Qt includes
#include <QtGui/QPainter>

// ImtCore includes
#include <imtwidgets/CMenuPanel.h>

#include <QDebug>

namespace imtwidgets
{


// public methods

CMenuPanelDelegate::CMenuPanelDelegate(QTreeView* menuPanelPtr)
	:QStyledItemDelegate(menuPanelPtr),
	m_fontMetrics(QFontMetrics(QFont())),
	m_maxIndent(20),
	m_iconSize(16),
	m_selectionSizeRatio(1),
	
	m_verticalPadding(0),
	m_leftPadding(0),
	m_rightPadding(0),
	m_iconToTextPadding(0),
	
	m_selectedColor(QColor(0, 0, 0)),
	m_mouseOverColor(QColor(0, 0, 0)),
	m_mouseOverSelectedColor(QColor(0, 0, 0)),
	m_selectedContourColor(QColor(0, 0, 0)),
	m_textColor(QColor(0, 0, 0))
{
}


int CMenuPanelDelegate::GetMinimumWidth()
{
	return 2 * m_leftPadding + m_selectionBoxHeight;
}



void CMenuPanelDelegate::SetFontMetrics(const QFontMetrics& fontMetrics)
{
	m_fontMetrics = fontMetrics;
}

void CMenuPanelDelegate::SetMaxIndent(int indent)
{
	m_maxIndent = indent;
}


void CMenuPanelDelegate::SetIconSize(int size)
{
	m_iconSize = size;

	m_selectionBoxHeight = sqrt(2 * m_iconSize * m_iconSize) * m_selectionSizeRatio;
	if ((m_selectionBoxHeight - m_iconSize) % 2){
		m_selectionBoxHeight++;
	}
}


void CMenuPanelDelegate::SetSelectionSizeRatio(double ratio)
{
	m_selectionSizeRatio = ratio;

	m_selectionBoxHeight = sqrt(2 * m_iconSize * m_iconSize) * m_selectionSizeRatio;
	if ((m_selectionBoxHeight - m_iconSize) % 2){
		m_selectionBoxHeight++;
	}
}


void CMenuPanelDelegate::SetVerticalPadding(int padding)
{
	m_verticalPadding = padding;
}


void CMenuPanelDelegate::SetLeftPadding(int padding)
{
	m_leftPadding = padding;
}


void CMenuPanelDelegate::SetRightPadding(int padding)
{
	m_rightPadding = padding;
}


void CMenuPanelDelegate::SetIconToTextPadding(int padding)
{
	m_iconToTextPadding = padding;
}


void CMenuPanelDelegate::SetSelectedColor(const QColor& color)
{
	m_selectedColor = color;
}


void CMenuPanelDelegate::SetMouserOverColor(const QColor& color)
{
	m_mouseOverColor = color;
}


void CMenuPanelDelegate::SetMouserOverSelectedColor(const QColor& color)
{
	m_mouseOverSelectedColor = color;
}


void CMenuPanelDelegate::SetSelectedContourColor(const QColor& color)
{
	m_selectedContourColor = color;
}


void CMenuPanelDelegate::SetTextColor(const QColor& color)
{
	m_textColor = color;
}


// protected methods

// reimplemented (QItemDelegate)

QSize CMenuPanelDelegate::sizeHint(const QStyleOptionViewItem& /*option*/, const QModelIndex& index) const
{
	QSize size;
	size.setHeight(m_selectionBoxHeight + m_verticalPadding);

	int offset = -m_maxIndent;
	QModelIndex currentIndex = index;
	while (currentIndex.isValid()){
		offset += m_maxIndent;
		currentIndex = currentIndex.parent();
	}

	QString text = index.data(Qt::DisplayRole).toString();

	int width = m_leftPadding + offset + m_selectionBoxHeight + m_iconToTextPadding;
	width += m_fontMetrics.boundingRect(text).width() + m_rightPadding;

	size.setWidth(width);

	return size;
}


void CMenuPanelDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	int indent = property("indent").toInt();

	QColor backgroundColor = Qt::transparent;

	if (option.state & QStyle::State_Selected){
		backgroundColor = m_selectedColor;
	}

	if (option.state & QStyle::State_MouseOver){
		const QStandardItemModel* modelPtr = dynamic_cast<const QStandardItemModel*>(index.model());
		Q_ASSERT(modelPtr != nullptr);

		if (modelPtr->itemFromIndex(index)->isEnabled()){
			backgroundColor = m_mouseOverColor;

			if (option.state & QStyle::State_Selected){
				backgroundColor = m_mouseOverSelectedColor;
			}
		}
	}

	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);

	QRect backgroundSingleEllipse;
	backgroundSingleEllipse.setLeft(m_leftPadding);
	backgroundSingleEllipse.setRight(m_leftPadding + m_selectionBoxHeight - 1);
	backgroundSingleEllipse.setTop(option.rect.bottom() - m_selectionBoxHeight + 1);
	backgroundSingleEllipse.setBottom(option.rect.bottom());

	// Draw background:
	if (option.state & QStyle::State_Selected || option.state & QStyle::State_MouseOver){
		painter->setBrush(QBrush(backgroundColor));

		QRect backgroundRect = option.rect;
		if (indent != 0){
			painter->setPen(Qt::transparent);

			backgroundRect.setLeft(0);
			backgroundRect.setRight(option.rect.right() - m_selectionBoxHeight / 2 - m_leftPadding);
			backgroundRect.setTop(option.rect.bottom() - m_selectionBoxHeight + 1);
			backgroundRect.setBottom(option.rect.bottom());
			painter->fillRect(backgroundRect, backgroundColor);

			QRect backgroundRightEllipse;
			backgroundRightEllipse.setRight(option.rect.right() - m_leftPadding);
			backgroundRightEllipse.setLeft(backgroundRightEllipse.right() - m_selectionBoxHeight + 1);
			backgroundRightEllipse.setTop(option.rect.bottom() - m_selectionBoxHeight + 1);
			backgroundRightEllipse.setBottom(option.rect.bottom());
			painter->drawEllipse(backgroundRightEllipse);
		}
		else {
			painter->setPen(m_selectedContourColor);
			painter->drawEllipse(backgroundSingleEllipse);
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
	iconRect = backgroundSingleEllipse;
	if (indent != 0){
		iconRect.setLeft(backgroundSingleEllipse.left() + offset);
		iconRect.setRight(backgroundSingleEllipse.right() + offset);
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
				m_iconSize,
				m_iconSize,
				iconMode);

	option.widget->style()->drawItemPixmap(
				painter, iconRect,
				Qt::AlignHCenter | Qt::AlignVCenter,
				iconPixmap);

	// Draw text:

	QString text = index.data(Qt::DisplayRole).toString();
	painter->setPen(m_textColor);
	if (indent > 0){
		QRect textRect = option.rect;
		textRect.adjust(0, m_verticalPadding, 0, 0);
		textRect.setLeft(iconRect.right() + 1 + m_iconToTextPadding);

		option.widget->style()->drawItemText(
					painter,
					textRect,
					Qt::AlignLeft | Qt::AlignVCenter,
					option.palette,
					true,
					text);
	}

	painter->restore();
}


} // namespace imtwidgets


