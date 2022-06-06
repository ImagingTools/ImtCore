#include <imtgui/CPopupTestGuiComp.h>


// Qt includes
#include <QtGui/QResizeEvent>
#include <QtGui/QScreen>
#include <QtWidgets/QSlider>
#include <QtWidgets/QLabel>

// ACF includes
#include <ilog/CMessage.h>


namespace imtgui
{


// public methods

CPopupTestGuiComp::CPopupTestGuiComp()
	:m_messageCounter(0)
{
}


// protected methods

// reimplemented (iqtgui::CGuiComponentBase)

void CPopupTestGuiComp::OnGuiCreated()
{
	EnableDesignHandler(true);

	BaseClass::OnGuiCreated();

	QWidget* widgetPtr = GetWidget()->window();
	if (widgetPtr != nullptr){
		widgetPtr->installEventFilter(this);
	}

	if (!m_popupControllerCompPtr.IsValid()){
		None->setEnabled(false);
		Info->setEnabled(false);
		Warning->setEnabled(false);
		Error->setEnabled(false);
		Critical->setEnabled(false);
		Timeout5->setEnabled(false);
		Timeout10->setEnabled(false);
		Timeout15->setEnabled(false);
		Timeout20->setEnabled(false);
		TimeoutUser->setEnabled(false);
		MessageButton->setEnabled(false);
		WidgetButton->setEnabled(false);
		WidgetControlsButton->setEnabled(false);
		AllowClosing->setEnabled(false);
	}
}


// reimplemented (iqtgui::TDesignSchemaHandlerWrap)

void CPopupTestGuiComp::OnDesignSchemaChanged()
{
	SetStatusIcon(GetIcon(":/Icons/Log"));
}


// reimplemented (QObject)

bool CPopupTestGuiComp::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::Move || event->type() == QEvent::Resize){
		UpdateClientArea();
	}

	return false;
}


// private slots

void CPopupTestGuiComp::on_AnchorTL_clicked()
{
	m_clientAreaProvider.SetCorner(Qt::TopLeftCorner);
}


void CPopupTestGuiComp::on_AnchorTR_clicked()
{
	m_clientAreaProvider.SetCorner(Qt::TopRightCorner);
}


void CPopupTestGuiComp::on_AnchorBL_clicked()
{
	m_clientAreaProvider.SetCorner(Qt::BottomLeftCorner);
}


void CPopupTestGuiComp::on_AnchorBR_clicked()
{
	m_clientAreaProvider.SetCorner(Qt::BottomRightCorner);
}


void CPopupTestGuiComp::on_InfoButton_clicked()
{
	SendInfoMessage(0, QString("Message %1").arg(m_messageCounter));
	m_messageCounter++;
}


void CPopupTestGuiComp::on_WarningButton_clicked()
{
	SendWarningMessage(0, QString("Message %1").arg(m_messageCounter));
	m_messageCounter++;
}


void CPopupTestGuiComp::on_ErrorButton_clicked()
{
	SendErrorMessage(0, QString("Message %1").arg(m_messageCounter));
	m_messageCounter++;
}


void CPopupTestGuiComp::on_CriticalButton_clicked()
{
	SendCriticalMessage(0, QString("Message %1").arg(m_messageCounter));
	m_messageCounter++;
}


void CPopupTestGuiComp::on_MessageButton_clicked()
{
	ilog::IMessageConsumer::MessagePtr messagePtr(new ilog::CMessage(GetCategory(), 0, "Popup message", ""));

	m_popupControllerCompPtr->AddPopup(messagePtr, GetTimeout(), AllowClosing->isChecked(), nullptr);
}


void CPopupTestGuiComp::on_WidgetButton_clicked()
{
	ilog::IMessageConsumer::MessagePtr messagePtr(new ilog::CMessage(GetCategory(), 0, "", ""));
	QWidget* widgetPtr = new QWidget();
	widgetPtr->setLayout(new QHBoxLayout());

	QSlider* sliderPtr = new QSlider(widgetPtr);
	sliderPtr->resize(50,50);
	sliderPtr->setMinimum(0);
	sliderPtr->setMaximum(100);
	sliderPtr->setValue(0);

	QLabel* textPtr = new QLabel(widgetPtr);
	textPtr->setText("100");

	connect(sliderPtr, &QAbstractSlider::sliderMoved, [textPtr](int value)
				{
					QString text = QString::number(value);
					textPtr->setText(text);
				});

	widgetPtr->layout()->addWidget(sliderPtr);
	widgetPtr->layout()->addWidget(textPtr);

	widgetPtr->adjustSize();
	textPtr->setText("0");

	m_popupControllerCompPtr->AddPopup(messagePtr, GetTimeout(), AllowClosing->isChecked(), widgetPtr);
}


void CPopupTestGuiComp::on_WidgetControlsButton_clicked()
{
	ilog::IMessageConsumer::MessagePtr messagePtr(new ilog::CMessage(GetCategory(), 0, "", ""));
	QWidget* widgetPtr = new QWidget();
	widgetPtr->setLayout(new QVBoxLayout());

	QLabel* textPtr = new QLabel(widgetPtr);
	textPtr->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	textPtr->setText("Internet connection lost. Switch to offline mode?");

	QHBoxLayout* hLayoutPtr = new QHBoxLayout();
	QPushButton* yesPtr = new QPushButton(widgetPtr);
	QPushButton* noPtr = new QPushButton(widgetPtr);
	yesPtr->setText("Yes");
	noPtr->setText("No");

	widgetPtr->layout()->addWidget(textPtr);
	widgetPtr->layout()->addItem(hLayoutPtr);
	hLayoutPtr->addStretch();
	hLayoutPtr->addWidget(yesPtr);
	hLayoutPtr->addWidget(noPtr);
	hLayoutPtr->addStretch();

	widgetPtr->adjustSize();

	QByteArray id = m_popupControllerCompPtr->AddPopup(messagePtr, GetTimeout(), AllowClosing->isChecked(), widgetPtr);

	IPopupController* controllerCompPtr = m_popupControllerCompPtr.GetPtr();
	connect(yesPtr, &QAbstractButton::clicked, [controllerCompPtr, id](){
		controllerCompPtr->ClosePopup(id);
	});

	connect(noPtr, &QAbstractButton::clicked, [controllerCompPtr, id](){
		controllerCompPtr->ClosePopup(id);
	});
}


void CPopupTestGuiComp::on_TimeoutUser_toggled(bool checked)
{
	if (checked){
		MessageButton->setEnabled(AllowClosing->isChecked());
		WidgetButton->setEnabled(AllowClosing->isChecked());
	}
	else{
		MessageButton->setEnabled(true);
		WidgetButton->setEnabled(true);
	}
}


void CPopupTestGuiComp::on_AllowClosing_toggled(bool checked)
{
	if (!checked){
		MessageButton->setEnabled(!TimeoutUser->isChecked());
		WidgetButton->setEnabled(!TimeoutUser->isChecked());
	}
	else{
		MessageButton->setEnabled(true);
		WidgetButton->setEnabled(true);
	}
}


void CPopupTestGuiComp::on_UseAreaProvider_toggled(bool checked)
{
	UpdateClientArea();
}


// public methods of the embedded class ClientAreaProvider

CPopupTestGuiComp::ClientAreaProvider::ClientAreaProvider()
	:m_corner(Qt::BottomRightCorner)
{
}


void CPopupTestGuiComp::ClientAreaProvider::SetCorner(Qt::Corner corner)
{
	if (m_corner != corner){
		istd::CChangeNotifier notifier(this);

		m_corner = corner;

		QPoint pnt = m_anchor;
		if (m_size.width() < 0){
			pnt.rx() += m_size.width() + 1;
		}

		if (m_size.height() < 0){
			pnt.ry() += m_size.height() + 1;
		}

		QSize size;
		size.setWidth(qAbs(m_size.width()));
		size.setHeight(qAbs(m_size.height()));

		QRect rect(pnt, size);
		SetRect(rect/*, false*/);
	}
}


void CPopupTestGuiComp::ClientAreaProvider::SetRect(const QRect& rect/*, bool isMoved*/)
{
	QPoint anchor;
	QSize size;
	switch (m_corner){
	case Qt::TopLeftCorner:
		anchor = rect.topLeft();
		size.setWidth(rect.width());
		size.setHeight(rect.height());
		break;
	case Qt::TopRightCorner:
		anchor = rect.topRight();
		size.setWidth(-rect.width());
		size.setHeight(rect.height());
		break;
	case Qt::BottomLeftCorner:
		anchor = rect.bottomLeft();
		size.setWidth(rect.width());
		size.setHeight(-rect.height());
		break;
	case Qt::BottomRightCorner:
		anchor = rect.bottomRight();
		size.setWidth(-rect.width());
		size.setHeight(-rect.height());
		break;
	}

	if (m_anchor != anchor || m_size != size){
		istd::CChangeNotifier notifier(this);

		m_anchor = anchor;
		m_size = size;
	}
}


// reimplemented (IClientAreaProvider)

QPoint CPopupTestGuiComp::ClientAreaProvider::GetAnchor() const
{
	return m_anchor;
}


QSize CPopupTestGuiComp::ClientAreaProvider::GetSize() const
{
	return m_size;
}

// private methods

QRect CPopupTestGuiComp::CalculatePopupAreaFromScreen()
{
	QGuiApplication* appPtr = dynamic_cast<QGuiApplication*>(qApp);
	if (appPtr != nullptr){
		QScreen* screenPtr = appPtr->primaryScreen();
		if (screenPtr != nullptr){
			QRect rect = screenPtr->geometry();

			return rect;
		}
	}

	return QRect();
}


void CPopupTestGuiComp::UpdateClientArea()
{
	if (UseAreaProvider->isChecked()){
		QWidget* widgetPtr = GetWidget()->window();
		m_clientAreaProvider.SetRect(widgetPtr->geometry()/*, true*/);
	}
	else{
		m_clientAreaProvider.SetRect(CalculatePopupAreaFromScreen());
	}
}


istd::IInformationProvider::InformationCategory CPopupTestGuiComp::GetCategory()
{
	istd::IInformationProvider::InformationCategory category = istd::IInformationProvider::IC_NONE;
	if (None->isChecked()){
		category = istd::IInformationProvider::IC_NONE;
	}
	else if (Info->isChecked()){
		category = istd::IInformationProvider::IC_INFO;
	}
	else if (Warning->isChecked()){
		category = istd::IInformationProvider::IC_WARNING;
	}
	else if (Error->isChecked()){
		category = istd::IInformationProvider::IC_ERROR;
	}
	else if (Critical->isChecked()){
		category = istd::IInformationProvider::IC_CRITICAL;
	}

	return category;
}


int CPopupTestGuiComp::GetTimeout()
{
	int timeout = -1;
	if (Timeout5->isChecked()){
		timeout = 5000;
	}
	else if (Timeout10->isChecked()){
		timeout = 10000;
	}
	else if (Timeout15->isChecked()){
		timeout = 15000;
	}
	else if (Timeout20->isChecked()){
		timeout = 20000;
	}

	return timeout;
}


QRect CPopupTestGuiComp::Normalize(const QRect& rect)
{
	QRect retVal;
	int left = rect.left();
	int right = rect.right();
	int top = rect.top();
	int bottom = rect.bottom();
	retVal.setLeft(left);
	retVal.setRight(right);
	retVal.setTop(top);
	retVal.setBottom(bottom);

	return retVal;
}


} // namespace imtgui


