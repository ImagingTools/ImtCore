#include <imtgui/CStandardPopupWidget.h>


// Qt includes
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtGui/QPainter>

// ACF includes
#include <iwidgets/iwidgets.h>


namespace imtgui
{


// public methods

void CStandardPopupWidget::SetContent(const ilog::IMessageConsumer::MessagePtr& messagePtr, QWidget* contentWidgetPtr)
{
	iwidgets::ClearLayout(layout());
	if (layout() == nullptr){
		QVBoxLayout* layoutPtr = new QVBoxLayout();
		layoutPtr->setContentsMargins(10, 10, 10, 10);
		setLayout(layoutPtr);
	}

	m_messagePtr = messagePtr;

	if (m_messagePtr.IsValid()){
		if (contentWidgetPtr != nullptr){
			contentWidgetPtr->setParent(this);
			layout()->addWidget(contentWidgetPtr);
		}
		else{
			QLabel* labelPtr = new QLabel(this);

			labelPtr->setText(messagePtr->GetInformationDescription());
			labelPtr->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			labelPtr->setStyleSheet(
						"QLabel { color : white; "
						"margin-top: 6px;"
						"margin-bottom: 6px;"
						"margin-left: 10px;"
						"margin-right: 10px; }");

			layout()->addWidget(labelPtr);
		}
	}

	adjustSize();
}


// protected methods

// reimplemented (QWidget)

void CStandardPopupWidget::paintEvent(QPaintEvent* /*event*/)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	static const std::map<istd::IInformationProvider::InformationCategory, QColor> backgroundColorLevels{
		{istd::IInformationProvider::InformationCategory::IC_NONE, QColor(0,0,0,192)},
		{istd::IInformationProvider::InformationCategory::IC_INFO, QColor(0,200,0,192)},
		{istd::IInformationProvider::InformationCategory::IC_WARNING, QColor(200,200,0,192)},
		{istd::IInformationProvider::InformationCategory::IC_ERROR, QColor(200,0,0,192)},
		{istd::IInformationProvider::InformationCategory::IC_CRITICAL, QColor(100,0,0,192)},
	};

	istd::IInformationProvider::InformationCategory category = istd::IInformationProvider::InformationCategory::IC_NONE;
	if (m_messagePtr.IsValid()){
		category = m_messagePtr->GetInformationCategory();
	}

	const QColor& backgroundColor = backgroundColorLevels.at(category);
	painter.setBrush(QBrush(backgroundColor));
	painter.setPen(Qt::NoPen);

	QRect widgetRect = rect();
	painter.drawRoundedRect(widgetRect, 10, 10);
}


} // namespace imtgui


