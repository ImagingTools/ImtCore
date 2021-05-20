#include <imtwidgets/CMenuPanelDelegateMin.h>


// Qt includes
#include <QtCore/QtMath>
#include <QtCore/QDebug>
#include <QtGui/QPainter>

// ImtCore includes
#include <imtwidgets/CMenuPanel.h>


namespace imtwidgets
{


// public methods

CMenuPanelDelegateMin::CMenuPanelDelegateMin(QTreeView* menuPanelPtr)
	:CMenuPanelDelegate(menuPanelPtr)
{
}


// reimplemented (IMenuPanelDelegate)

int CMenuPanelDelegateMin::GetMinimumWidth()
{
	return 2 * m_leftPadding + m_height;
}


// protected methods

// reimplemented (QItemDelegate)

void CMenuPanelDelegateMin::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QPalette palette = option.palette;

	if (option.rect.height() < m_height){
		return;
	}

	int indent = property("indent").toInt();

	QColor backgroundColor = Qt::transparent;
	QColor textColor = palette.color(QPalette::Text);

	if (option.state & QStyle::State_Selected){
		backgroundColor = palette.color(QPalette::WindowText);
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
	if (option.state & QStyle::State_Selected){
		painter->setBrush(QBrush(backgroundColor));

		QRect backgroundRect = option.rect;
		painter->setPen(Qt::transparent);

		backgroundRect.setLeft(0);
		backgroundRect.setRight(3);
		backgroundRect.setTop(option.rect.bottom() - m_height + 1);
		backgroundRect.setBottom(option.rect.bottom());
		painter->fillRect(backgroundRect, QColor(25, 200, 60));
	}

	// Calculate offset from treeview's left side:
	int offset = -indent;
	QModelIndex currentIndex = index;
	while (currentIndex.isValid()){
		offset += indent;
		currentIndex = currentIndex.parent();
	}

	QRect iconRect;
	iconRect.setLeft(3);
	iconRect.setRight(option.rect.right());
	iconRect.setTop(option.rect.bottom() - m_height + 1);
	iconRect.setBottom(option.rect.bottom() - m_height * 0.21);

	// Draw icon:
	QIcon::Mode iconMode = QIcon::Mode::Normal;
	if (!index.data(imtwidgets::CMenuPanel::DR_PAGE_ENABLED).toBool()){
		iconMode = QIcon::Mode::Disabled;
	}

	int iconSize = m_iconHeight;
	if ((option.state & QStyle::State_Selected) || (option.state & QStyle::State_MouseOver)){
		const QStandardItemModel* modelPtr = dynamic_cast<const QStandardItemModel*>(index.model());
		Q_ASSERT(modelPtr != nullptr);

		if (modelPtr->itemFromIndex(index)->isEnabled()){
			iconSize = m_iconHeightHover;
			iconMode = QIcon::Mode::Selected;
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
	QFont font = painter->font();
	font.setPointSize(8);
	painter->setFont(font);
	QFontMetrics fontMetrics(font);
	QRect textRect = option.rect;
	textRect.setLeft(4);
	textRect.setTop(option.rect.bottom() - m_height * 0.21 );
	text = fontMetrics.elidedText(text,Qt::ElideRight,textRect.width());

	option.widget->style()->drawItemText(
				painter,
				textRect,
				Qt::AlignHCenter | Qt::AlignBottom,
				palette,
				true,
				text);

	painter->restore();
}


} // namespace imtwidgets


