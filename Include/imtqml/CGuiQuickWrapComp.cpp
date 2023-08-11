#include <imtqml/CGuiQuickWrapComp.h>


namespace imtqml
{


CGuiQuickWrapComp::CGuiQuickWrapComp()
	:m_quickWidget(nullptr)
{
	EnableDesignHandler(true);
}


// reimplemented (iqtgui::IGuiObject)

bool CGuiQuickWrapComp::IsGuiCreated() const
{
	return  m_quickWidget != nullptr;
}


bool CGuiQuickWrapComp::CreateGui(QWidget *parentPtr)
{
		
	if (m_quickCompPtr.IsValid()){
		imtqml::IQuickObject *quickObjectPtr = m_quickCompPtr.GetPtr();
		if (m_quickWidget == nullptr){
			m_quickWidget = new QQuickWidget(parentPtr);
			m_quickWidget->setContentsMargins(0, 0, 0, 0);
			m_quickWidget->setSource(QUrl("qrc:/qml/WrapItem.qml"));

			quickObjectPtr->CreateQuickItem(m_quickWidget->rootObject());

			m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
		}
	}

	if (parentPtr != nullptr) {
		QLayout* layoutPtr = parentPtr->layout();

		if (layoutPtr != nullptr) {
			layoutPtr->addWidget(m_quickWidget);
		}
	}

	return  m_quickWidget != nullptr;
}


bool CGuiQuickWrapComp::DestroyGui()
{
	if (m_quickWidget != nullptr){
		imtqml::IQuickObject* quickObjectPtr = m_quickCompPtr.GetPtr();
		if (quickObjectPtr != nullptr){
			quickObjectPtr->DestroyQuickItem();
		}

		m_quickWidget->deleteLater();
	}

	return true;
}


QWidget* CGuiQuickWrapComp::GetWidget() const
{
	return m_quickWidget;
}


void CGuiQuickWrapComp::OnTryClose(bool* /*ignoredPtr*/)
{
	if (m_quickWidget != nullptr){
		m_quickWidget->close();
	}
}


// protected methods

// reimplemented (ibase::TDesignSchemaHandlerWrap)

void CGuiQuickWrapComp::OnDesignSchemaChanged(const QByteArray & themeId)
{
	BaseClass::OnDesignSchemaChanged(themeId);

	if (m_defaultStatusIconPathAttrPtr.IsValid()){
		istd::CChangeNotifier changeNotifier(&m_visualStatus);

		m_visualStatus.m_statusIcon = GetIcon(*m_defaultStatusIconPathAttrPtr);
	}
}

// reimplemented (icomp::CComponentBase)

void CGuiQuickWrapComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_defaultStatusIconPathAttrPtr.IsValid()){
		m_visualStatus.m_statusIcon = GetIcon(*m_defaultStatusIconPathAttrPtr);
	}

	m_visualStatus.m_statusText = *m_defaultStatusTextAttrPtr;
}


// public methods of embedded class VisualStatus

// reimplemented (iqtgui::IVisualStatus)

QIcon CGuiQuickWrapComp::VisualStatus::GetStatusIcon() const
{
	return m_statusIcon;
}


QString CGuiQuickWrapComp::VisualStatus::GetStatusText() const
{
	return m_statusText;
}


} // namespace imtqml



