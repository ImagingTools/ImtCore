#include <imthypegui/CTaskItemDelegate.h>


// Qt includes
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtCore/QDir>
#include <QtCore/QUuid>
#include <QtCore/QModelIndex>

// ACF includes
#include <istd/IInformationProvider.h>


namespace imthypegui
{


// public methods

CTaskItemDelegate::CTaskItemDelegate(QObject* parent)
{
	BaseClass::setParent(parent);
}


// protected methods

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

	int rowIndex = option.index.row();
	int topPadding = (rowIndex == 0) ? 0 : 9;
	int bottomPadding = (rowIndex == 0) ? 9 : 0;

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

	const int textPadding = 4;
	QRect headerRect(mainRect.left(), mainRect.top(), mainRect.width(), iconSize + 2 * textPadding);
	painter->fillRect(headerRect, headerColor);

	// Draw task name:
	QColor taskTextColor = option.state & QStyle::State_Selected ? palette.color(QPalette::HighlightedText) : palette.color(QPalette::Text);
	painter->setPen(taskTextColor);
	painter->setFont(taskTextFont);

	int taskTextRectLeft = mainRect.left() + 2 * textPadding + iconSize;
	QRect taskTextRect(taskTextRectLeft, mainRect.top() + textPadding, mainRect.right() - taskTextRectLeft, taskTextFontMetrics.height());
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

	QRect typeTextRect(taskTextRectLeft, mainRect.top() + textPadding + taskTextFontMetrics.height(), mainRect.right() - taskTextRectLeft, typeTextFontMetrics.height());
	typeText = typeTextFontMetrics.elidedText(typeText, Qt::ElideRight, typeTextRect.width() - textPadding);
	painter->drawText(
					typeTextRect,
					Qt::AlignLeft | Qt::AlignVCenter,
					typeText);

	// Draw icon:
	QPixmap stateIcon = index.data(DR_TASK_PROCESSING_STATE_ICON).value<QPixmap>();
	if (stateIcon.isNull()){
		istd::IInformationProvider::InformationCategory inspectionState = istd::IInformationProvider::InformationCategory(index.data(DR_TASK_PROCESSING_STATE).toInt());
		switch (inspectionState){
		case istd::IInformationProvider::IC_NONE:
			stateIcon.load(GetIconPath(":/Icons/StateUnknown"));
			break;

		case istd::IInformationProvider::IC_INFO:
			stateIcon.load(GetIconPath(":/Icons/StateOk"));
			break;

		case istd::IInformationProvider::IC_WARNING:
			stateIcon.load(GetIconPath(":/Icons/StateWarning"));
			break;

		case istd::IInformationProvider::IC_ERROR:
			stateIcon.load(GetIconPath(":/Icons/Error"));
			break;

		case istd::IInformationProvider::IC_CRITICAL:
			stateIcon.load(GetIconPath(":/Icons/StateInvalid"));
			break;

		default:
			stateIcon.load(GetIconPath(":/Icons/StateUnknown"));
			break;
		}
	}

	bool isTaskEnabled = index.data(DR_TASK_ENABLED).toBool();

	stateIcon.scaledToHeight(iconSize, Qt::SmoothTransformation);
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


} // namespace imthypegui


