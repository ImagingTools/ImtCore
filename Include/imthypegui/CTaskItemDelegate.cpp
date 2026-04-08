// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthypegui/CTaskItemDelegate.h>


// Qt includes
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtCore/QDir>
#include <QtCore/QUuid>
#include <QtCore/QModelIndex>

// ACF includes
#include <istd/IInformationProvider.h>
#include <iqtgui/iqtgui.h>


namespace imthypegui
{


// public methods

CTaskItemDelegate::CTaskItemDelegate(QObject* parent)
	:BaseClass(parent)
{
}


// protected methods

// reimplemented (QItemDelegate)

QSize CTaskItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const
{
	return option.decorationSize;
}


void CTaskItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setRenderHint(QPainter::TextAntialiasing);
	
	// Setup fonts
	QFont taskTextFont;
	taskTextFont.setPointSize(11);
	taskTextFont.setBold(false);
	QFontMetrics taskTextFontMetrics(taskTextFont);

	QFont typeTextFont;
	typeTextFont.setPointSize(8);
	typeTextFont.setBold(true);
	QFontMetrics typeTextFontMetrics(typeTextFont);

	// Setup icon size
	int iconSize = taskTextFontMetrics.height() + typeTextFontMetrics.height();

	const int rowIndex = option.index.row();
	const int topPadding = (rowIndex == 0) ? 0 : 9;
	const int bottomPadding = (rowIndex == 0) ? 9 : 0;
	const int textPadding = 4;

	QPalette palette = qApp->palette();

	// Draw background, border and header
	QColor backgroundColor(palette.color(QPalette::Base));
	QRect mainRect = option.rect.adjusted(9, topPadding, -9, -bottomPadding);
	painter->fillRect(mainRect, backgroundColor);
	
	QColor headerColor = option.state & QStyle::State_Selected ? palette.color(QPalette::Highlight) : palette.color(QPalette::Midlight);
	painter->save();
	painter->setPen(QPen(headerColor, 2));
	painter->drawRoundedRect(mainRect, 2, 2);
	painter->restore();

	QRect headerRect(mainRect.left(), mainRect.top(), mainRect.width(), iconSize + 2 * textPadding);
	painter->fillRect(headerRect, headerColor);

	// Get task selected state
	int taskTextRectRight = mainRect.right() - textPadding;

	if (m_selectableItems)
	{
		bool isTaskSelected = index.data(DR_TASK_SELECTED).toBool();

		// Draw checkbox for selection state
		int checkboxSize = 16;
		int checkboxPadding = 4;
		QRect checkboxRect(mainRect.right() - checkboxSize - checkboxPadding,
			mainRect.top() + (headerRect.height() - checkboxSize) / 2,
			checkboxSize,
			checkboxSize);

		// Draw checkbox background
		painter->save();
		QColor checkboxBgColor = palette.color(QPalette::Base);
		painter->setBrush(checkboxBgColor);
		painter->setPen(QPen(palette.color(QPalette::Dark), 1));
		painter->drawRoundedRect(checkboxRect, 2, 2);

		// Draw checkmark if selected
		if (isTaskSelected)
		{
			painter->setPen(QPen(palette.color(QPalette::Highlight), 2));
			painter->setRenderHint(QPainter::Antialiasing, true);

			// Draw checkmark symbol
			QPoint checkmarkStart(checkboxRect.left() + 3, checkboxRect.top() + checkboxRect.height() / 2);
			QPoint checkmarkMiddle(checkboxRect.left() + checkboxRect.width() / 2 - 1, checkboxRect.bottom() - 3);
			QPoint checkmarkEnd(checkboxRect.right() - 3, checkboxRect.top() + 3);

			painter->drawLine(checkmarkStart, checkmarkMiddle);
			painter->drawLine(checkmarkMiddle, checkmarkEnd);
		}
		painter->restore();

		taskTextRectRight = checkboxRect.left() - textPadding;
	}

	// Adjust task text rect to account for checkbox
	int taskTextRectLeft = mainRect.left() + 2 * textPadding + iconSize;

	// Draw task name:
	QColor taskTextColor = option.state & QStyle::State_Selected ? palette.color(QPalette::HighlightedText) : palette.color(QPalette::Text);
	painter->setPen(taskTextColor);
	painter->setFont(taskTextFont);

	QRect taskTextRect(taskTextRectLeft, mainRect.top() + textPadding, taskTextRectRight - taskTextRectLeft, taskTextFontMetrics.height());
	QString taskText = index.data(DR_TASK_NAME).toString();

	taskText = taskTextFontMetrics.elidedText(taskText, Qt::ElideRight, taskTextRect.width() - textPadding);
	painter->drawText(
					taskTextRect,
					Qt::AlignLeft | Qt::AlignVCenter,
					taskText);

	// Draw task type:
	QColor typeTextColor = option.state & QStyle::State_Selected ? palette.color(QPalette::HighlightedText) : palette.color(QPalette::Text);
	painter->setPen(typeTextColor);
	painter->setFont(typeTextFont);

	QByteArray userId = index.data(DR_TASK_USERID).toByteArray();
	QByteArray inputId = index.data(DR_TASK_INPUTID).toByteArray();
	QString typeText = index.data(DR_TYPE_NAME).toString();

	if (!userId.isEmpty() || !inputId.isEmpty()){
		typeText += " | " + userId;
	}

	if (!inputId.isEmpty()){
		typeText += " | " + inputId;
	}

	QRect typeTextRect(taskTextRectLeft, mainRect.top() + textPadding + taskTextFontMetrics.height(), taskTextRectRight - taskTextRectLeft, typeTextFontMetrics.height());
	typeText = typeTextFontMetrics.elidedText(typeText, Qt::ElideRight, typeTextRect.width() - textPadding);
	painter->drawText(
					typeTextRect,
					Qt::AlignLeft | Qt::AlignVCenter,
					typeText);

	// Draw icon:
	bool isTaskEnabled = index.data(DR_TASK_ENABLED).toBool();

	QPixmap stateIcon = index.data(DR_TASK_PROCESSING_STATE_ICON).value<QPixmap>()
		.scaledToHeight(iconSize, Qt::SmoothTransformation);
	stateIcon = QIcon(stateIcon).pixmap(iconSize, iconSize, isTaskEnabled ? QIcon::Normal : QIcon::Disabled);

	QRect iconRect(mainRect.left() + textPadding, mainRect.top() + textPadding, iconSize, iconSize);
	painter->drawPixmap(iconRect, stateIcon);

	// Draw thumbnail
	QImage thmubnailImage = index.data(DR_TASK_PREVIEW_OBJECT).value<QImage>();
	QRect thumbRect(mainRect.left() + 1, headerRect.bottom() + 1, mainRect.width() - 2, mainRect.height() -1 - headerRect.height());
	
	double sourceAspectRatio = 1.0 * thmubnailImage.width() / thmubnailImage.height();
	double destinationAspectRatio = 1.0 * thumbRect.width() / thumbRect.height();

	int originalWidth = thumbRect.width();
	int originalHeight = thumbRect.height();

	if (destinationAspectRatio > sourceAspectRatio){
		thumbRect.setWidth(thumbRect.height() * sourceAspectRatio);

		int shift = (originalWidth - thumbRect.width()) / 2;

		thumbRect.setLeft(thumbRect.left() + shift);
		thumbRect.setRight(thumbRect.right() + shift);
	}
	else{
		thumbRect.setHeight(thumbRect.width() / sourceAspectRatio);

		int shift = (originalHeight - thumbRect.height()) / 2;

		thumbRect.setTop(thumbRect.top() + shift);
		thumbRect.setBottom(thumbRect.bottom() + shift);
	}

	thmubnailImage = thmubnailImage.scaled(thumbRect.size(), Qt::AspectRatioMode::KeepAspectRatioByExpanding, Qt::TransformationMode::SmoothTransformation);

	painter->drawImage(thumbRect, thmubnailImage);

	// Disabled task
	if (!isTaskEnabled){
		painter->save();

		QColor disableCurtainColor(200, 200, 200, 50);
		QBrush disableCurtainBrush(disableCurtainColor, Qt::BDiagPattern);
		painter->fillRect(mainRect, disableCurtainBrush);

		painter->setPen(QColor(255, 120, 0));
		painter->setBrush(Qt::SolidPattern);

		QFont boldFont;
		boldFont.setPointSize(10);
		boldFont.setBold(true);
		painter->setFont(boldFont);

		QRect disableTextRect(mainRect.left() + textPadding, headerRect.bottom() + textPadding, mainRect.right(), iconSize);
		painter->drawText(
					disableTextRect,
					Qt::AlignLeft | Qt::AlignVCenter,
					QObject::tr("Not active"));

		painter->restore();
	}

	if (!(option.state & QStyle::State_Selected)){
		QColor curtainColor(255, 255, 255, 70);
		painter->fillRect(mainRect, curtainColor);
	}
}


QWidget* CTaskItemDelegate::createEditor(QWidget* /*parent*/, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const
{
	return NULL;
}

// static
QRect CTaskItemDelegate::GetCheckboxRect(const QRect& itemRect)
{
	const int checkboxSize = 16;
	const int checkboxPadding = 8;

	const int offsetY = 10;

	return QRect(
		itemRect.right() - checkboxSize - checkboxPadding * 2,
		itemRect.top() + offsetY,
		checkboxSize + checkboxPadding * 2,
		checkboxSize + checkboxPadding * 2);
}

// Micro UI

CMicroTaskItemDelegate::CMicroTaskItemDelegate(QObject* parent)
	:BaseClass(parent)
{
}


void CMicroTaskItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	bool isTaskSelected = option.state & QStyle::State_Selected;
	bool isTaskEnabled = index.data(DR_TASK_ENABLED).toBool();
	bool isTaskChecked = index.data(DR_TASK_SELECTED).toBool();

	QPalette palette = qApp->palette();
	
	QRect mainRect = option.rect.adjusted(1, 1, -2, -2);

	painter->save();

	// background
	if (isTaskEnabled) {
		painter->setBrush(palette.color(QPalette::Base));
	}
	else {
		QColor disableCurtainColor(200, 200, 200, 50);
		QBrush disableCurtainBrush(disableCurtainColor, Qt::BDiagPattern);
		painter->setBrush(disableCurtainBrush);
	}

	painter->setPen(QPen(palette.color(isTaskSelected ? QPalette::Highlight : QPalette::Shadow), isTaskSelected ? 2 : 1));
	painter->drawRect(mainRect);

	// Draw checkbox for selection state
	const int checkboxSize = 12;
	const int checkboxPadding = 2;
	QRect checkboxRect(mainRect.right() - checkboxSize - checkboxPadding, 
	                   mainRect.top() + checkboxPadding, 
	                   checkboxSize, 
	                   checkboxSize);

	// Draw checkbox background
	QColor checkboxBgColor = palette.color(QPalette::Base);
	painter->setBrush(checkboxBgColor);
	painter->setPen(QPen(palette.color(QPalette::Dark), 1));
	painter->drawRoundedRect(checkboxRect, 1, 1);

	// Draw checkmark if checked
	if (isTaskChecked)
	{
		painter->setPen(QPen(palette.color(QPalette::Highlight), 2));
		painter->setRenderHint(QPainter::Antialiasing, true);
		
		// Draw checkmark symbol (scaled down for smaller size)
		QPoint checkmarkStart(checkboxRect.left() + 2, checkboxRect.top() + checkboxRect.height() / 2);
		QPoint checkmarkMiddle(checkboxRect.left() + checkboxRect.width() / 2 - 1, checkboxRect.bottom() - 2);
		QPoint checkmarkEnd(checkboxRect.right() - 2, checkboxRect.top() + 2);
		
		painter->drawLine(checkmarkStart, checkmarkMiddle);
		painter->drawLine(checkmarkMiddle, checkmarkEnd);
	}

	// Draw icon
	const int icon_size = 26;
	QPixmap stateIcon = index.data(DR_TASK_PROCESSING_STATE_ICON).value<QPixmap>()
		.scaledToHeight(icon_size, Qt::SmoothTransformation);
	stateIcon = QIcon(stateIcon).pixmap(icon_size, icon_size, isTaskEnabled ? QIcon::Normal : QIcon::Disabled);

	QRect iconRect(mainRect.left() + 2, mainRect.top() + 2, icon_size, icon_size);
	painter->drawPixmap(iconRect, stateIcon);

	// Draw text (adjusted to avoid checkbox)
	painter->setPen(palette.color(isTaskEnabled ? QPalette::Text : QPalette::Light));

	QString taskText = index.data(DR_TASK_NAME).toString();
	// Adjust text rect to leave space for checkbox
	QRect textRect = mainRect.adjusted(4, 36, -(checkboxSize + checkboxPadding + 2), -2);

	QFont smallFont;
	smallFont.setPointSize(8);
	painter->setFont(smallFont);

	QTextOption to;
	to.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	painter->drawText(textRect, taskText, to);

	painter->restore();
}

// static
QRect CMicroTaskItemDelegate::GetCheckboxRect(const QRect& itemRect)
{
	const int checkboxSize = 12;
	const int checkboxPadding = 2;

	const QRect mainRect = itemRect.adjusted(1, 1, -2, -2);

	return QRect(
		mainRect.right() - checkboxSize - checkboxPadding,
		mainRect.top() + checkboxPadding,
		checkboxSize,
		checkboxSize);
}


} // namespace imthypegui


