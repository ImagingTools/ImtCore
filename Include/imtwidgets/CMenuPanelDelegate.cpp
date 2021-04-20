#include <imtwidgets/CMenuPanelDelegate.h>


// Qt includes
#include <QtCore/QtMath>
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
	m_indent(0),
	m_height(0),
	m_iconSizeRatio(1),
	m_iconSizeHoverRatio(1),

	m_topPadding(0),
	m_leftPadding(0),
	m_rightPadding(0),
	m_iconToTextPadding(0)
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

void CMenuPanelDelegate::SetIndent(int indent)
{
	m_indent = indent;
}


void CMenuPanelDelegate::SetItemHeight(int heigth)
{
	m_height = heigth;

	SetIconSizeRatio(m_iconSizeRatio);
}


void CMenuPanelDelegate::SetIconSizeRatio(double ratio)
{
	m_iconSizeRatio = ratio;

	m_iconHeight = (m_height * m_iconSizeRatio ) / sqrt(2);

	if (m_height % 2 != 0 && m_iconHeight % 2 == 0){
		m_iconHeight--;
	}

	if (m_height % 2 == 0 && m_iconHeight % 2 != 0){
		m_iconHeight--;
	}

	SetIconSizeHoverRatio(m_iconSizeHoverRatio);
}


void CMenuPanelDelegate::SetIconSizeHoverRatio(double ratio)
{
	m_iconSizeHoverRatio = ratio;

	m_iconHeightHover = m_iconHeight * m_iconSizeHoverRatio;

	if (m_height % 2 != 0 && m_iconHeightHover % 2 == 0){
		m_iconHeightHover--;
	}

	if (m_height % 2 == 0 && m_iconHeightHover % 2 != 0){
		m_iconHeightHover--;
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

	QVariant isHidden = index.data(CMenuPanel::DR_PAGE_HIDDEN_WHILE_COLLAPSED);

	if (isHidden.isValid() && isHidden.type() == QVariant::Bool){
		if (isHidden.toBool()){
			size.setHeight(size.height() - m_height);
		}
	}

	int offset = -m_indent;
	QModelIndex currentIndex = index;
	while (currentIndex.isValid()){
		offset += m_indent;
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
	QPalette palette = qApp->palette();

	if (option.rect.height() < m_height){
		return;
	}

	int indent = property("indent").toInt();

	QColor backgroundColor = Qt::transparent;
	QColor textColor = palette.color(QPalette::Text);

	if (option.state & QStyle::State_Selected){
		backgroundColor = palette.color(QPalette::Highlight);
		textColor = palette.color(QPalette::HighlightedText);
	}

	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

	QRect backgroundSingleEllipse;
	backgroundSingleEllipse.setLeft(m_leftPadding + 1);
	backgroundSingleEllipse.setRight(m_leftPadding + m_height - 2);
	backgroundSingleEllipse.setTop(option.rect.bottom() - m_height + 2);
	backgroundSingleEllipse.setBottom(option.rect.bottom() - 1);

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

	int iconSize = m_iconHeight;
	if (option.state & QStyle::State_MouseOver){
		const QStandardItemModel* modelPtr = dynamic_cast<const QStandardItemModel*>(index.model());
		Q_ASSERT(modelPtr != nullptr);

		if (modelPtr->itemFromIndex(index)->isEnabled()){
			iconSize = m_iconHeightHover;
		}
	}

	QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
	QPixmap iconPixmap = icon.pixmap(
				iconSize,
				iconSize,
				iconMode);

	option.widget->style()->drawItemPixmap(
				painter, iconRect,
				Qt::AlignHCenter | Qt::AlignVCenter,
				iconPixmap);

	// Draw text:
	QString text = index.data(Qt::DisplayRole).toString();
	painter->setPen(textColor);
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


