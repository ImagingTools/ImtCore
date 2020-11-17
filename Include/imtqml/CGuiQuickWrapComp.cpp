#include <imtqml/CGuiQuickWrapComp.h>


// Qt includes
#include <QtGui/QGuiApplication>
#include <QtWidgets/QVBoxLayout>


namespace imtqml
{

CGuiQuickWrapComp::CGuiQuickWrapComp()
	:m_quickWidget(nullptr),
	m_layout(nullptr)
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
		imtqml::IQuickObject *quickObject = m_quickCompPtr.GetPtr();
		if (m_quickWidget == nullptr){
			m_quickWidget = new QQuickWidget(parentPtr);
			m_quickWidget->setSource(QUrl::fromLocalFile((":/qml/WrapItem.qml")));

			quickObject->CreateItem(m_quickWidget->rootObject());

			m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
		}
	}

	if (parentPtr != nullptr){
		m_layout = new QVBoxLayout(parentPtr);

		m_layout->addWidget(m_quickWidget);
	}

	return  m_quickWidget != nullptr;
}


bool CGuiQuickWrapComp::DestroyGui()
{
	if (m_quickWidget != nullptr){
		m_quickWidget->deleteLater();
	}

	if (m_layout != nullptr){
		m_layout->deleteLater();
	}

	return true;
}


QWidget *CGuiQuickWrapComp::GetWidget() const
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



