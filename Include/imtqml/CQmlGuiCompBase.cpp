#include <imtqml/CQmlGuiCompBase.h>


namespace imtqml
{


CQmlGuiCompBase::CQmlGuiCompBase()
	:m_quickWidget(nullptr)
{
	EnableDesignHandler(true);
}


// reimplemented (iqtgui::IGuiObject)

bool CQmlGuiCompBase::IsGuiCreated() const
{
	return  m_quickWidget != nullptr;
}


bool CQmlGuiCompBase::CreateGui(QWidget *parentPtr)
{
	if (m_quickWidget == nullptr){
#if QT_VERSION > 0x060000
		QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
#endif
		m_quickWidget = new QQuickWidget(parentPtr);
		m_quickWidget->setContentsMargins(0, 0, 0, 0);
		m_quickWidget->setSource(QUrl("qrc:/qml/WrapItem.qml"));

		BaseClass::CreateQuickItem(m_quickWidget->rootObject());

		m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
	}

	if (parentPtr != nullptr) {
		QLayout* layoutPtr = parentPtr->layout();
		if (layoutPtr != nullptr) {
			layoutPtr->addWidget(m_quickWidget);
		}
	}

	return  m_quickWidget != nullptr;
}


bool CQmlGuiCompBase::DestroyGui()
{
	if (m_quickWidget != nullptr){
		BaseClass::DestroyQuickItem();

		m_quickWidget->deleteLater();
	}

	return true;
}


QWidget* CQmlGuiCompBase::GetWidget() const
{
	return m_quickWidget;
}


void CQmlGuiCompBase::OnTryClose(bool* ignoredPtr)
{
	if (m_quickWidget != nullptr){
		m_quickWidget->close();

		if (ignoredPtr != nullptr) {
			*ignoredPtr = false;
		}
	}
}


// protected methods

// reimplemented (ibase::TDesignSchemaHandlerWrap)

void CQmlGuiCompBase::OnDesignSchemaChanged(const QByteArray & themeId)
{
	BaseClass::OnDesignSchemaChanged(themeId);

	if (m_defaultStatusIconPathAttrPtr.IsValid()){
		istd::CChangeNotifier changeNotifier(&m_visualStatus);

		m_visualStatus.m_statusIcon = GetIcon(*m_defaultStatusIconPathAttrPtr);
	}
}

// reimplemented (icomp::CComponentBase)

void CQmlGuiCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_defaultStatusIconPathAttrPtr.IsValid()){
		m_visualStatus.m_statusIcon = GetIcon(*m_defaultStatusIconPathAttrPtr);
	}

	m_visualStatus.m_statusText = *m_defaultStatusTextAttrPtr;
}


// public methods of embedded class VisualStatus

// reimplemented (iqtgui::IVisualStatus)

QIcon CQmlGuiCompBase::VisualStatus::GetStatusIcon() const
{
	return m_statusIcon;
}


QString CQmlGuiCompBase::VisualStatus::GetStatusText() const
{
	return m_statusText;
}


} // namespace imtqml



