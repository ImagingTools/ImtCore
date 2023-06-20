#include <imtqml/CGuiQuickWrapComp.h>


namespace imtqml
{

CGuiQuickWrapComp::CGuiQuickWrapComp()
	:m_quickWidget(nullptr)
{
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


} // namespace imtqml



