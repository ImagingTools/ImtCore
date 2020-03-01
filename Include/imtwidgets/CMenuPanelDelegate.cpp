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
	m_height(16),
	m_iconSizeRatio(1),
	
	m_topPadding(0),
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
	return 2 * m_leftPadding + m_height;
}



void CMenuPanelDelegate::SetFontMetrics(const QFontMetrics& fontMetrics)
{
	m_fontMetrics = fontMetrics;
}

void CMenuPanelDelegate::SetMaxIndent(int indent)
{
	m_maxIndent = indent;
}


void CMenuPanelDelegate::SetItemHeight(int heigth)
{
	m_height = heigth;

	m_iconHeight = sqrt((m_height * m_height / m_iconSizeRatio) / 2);

	if (m_height % 2 != 0 && m_iconHeight % 2 == 0){
		m_iconHeight--;
	}

	if (m_height % 2 == 0 && m_iconHeight % 2 != 0){
		m_iconHeight--;
	}
}


void CMenuPanelDelegate::SetIconSizeRatio(double ratio)
{
	m_iconSizeRatio = ratio;

	m_iconHeight = sqrt((m_height * m_height / m_iconSizeRatio) / 2);

	if (m_height % 2 != 0 && m_iconHeight % 2 == 0){
		m_iconHeight--;
	}

	if (m_height % 2 == 0 && m_iconHeight % 2 != 0){
		m_iconHeight--;
	}
}


void CMenuPanelDelegate::SetTopPadding(int padding)
{
	m_topPadding = padding;
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

	if (index.row() == 0 && !index.parent().isValid()) {
		size.setHeight(m_height + m_topPadding);
	}
	else{
		size.setHeight(m_height);
	}

	int offset = -m_maxIndent;
	QModelIndex currentIndex = index;
	while (currentIndex.isValid()){
		offset += m_maxIndent;
		currentIndex = currentIndex.parent();
	}

	QString text = index.data(Qt::DisplayRole).toString();

	int width = m_leftPadding + offset + m_height + m_iconToTextPadding;
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
	painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

	QRect backgroundSingleEllipse;
	backgroundSingleEllipse.setLeft(m_leftPadding);
	backgroundSingleEllipse.setRight(m_leftPadding + m_height - 1);
	backgroundSingleEllipse.setTop(option.rect.bottom() - m_height + 1);
	backgroundSingleEllipse.setBottom(option.rect.bottom());

	// Draw background:
	if (option.state & QStyle::State_Selected || option.state & QStyle::State_MouseOver){
		painter->setBrush(QBrush(backgroundColor));

		QRect backgroundRect = option.rect;
		if (indent != 0){
			painter->setPen(Qt::transparent);

			backgroundRect.setLeft(0);
			backgroundRect.setRight(option.rect.right() - m_height / 2 - m_leftPadding);
			backgroundRect.setTop(option.rect.bottom() - m_height + 1);
			backgroundRect.setBottom(option.rect.bottom());
			painter->fillRect(backgroundRect, backgroundColor);

			QRect backgroundRightEllipse;
			backgroundRightEllipse.setRight(option.rect.right() - m_leftPadding);
			backgroundRightEllipse.setLeft(backgroundRightEllipse.right() - m_height + 1);
			backgroundRightEllipse.setTop(option.rect.bottom() - m_height + 1);
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

	QRect iconRect;
	iconRect = backgroundSingleEllipse;
	if (indent != 0){
		iconRect.setLeft(backgroundSingleEllipse.left() + offset);
		iconRect.setRight(backgroundSingleEllipse.right() + offset);
	}

	// Draw icon:
	QIcon::Mode iconMode = QIcon::Mode::Normal;
	if (!index.data(imtwidgets::CMenuPanel::DR_PAGE_ENABLED).toBool()){
		iconMode = QIcon::Mode::Disabled;
	}

	QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
	QPixmap iconPixmap = icon.pixmap(
				m_iconHeight,
				m_iconHeight,
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
		textRect.setLeft(iconRect.right() + 1 + m_iconToTextPadding);
		textRect.setTop(option.rect.bottom() - m_height + 1);

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


