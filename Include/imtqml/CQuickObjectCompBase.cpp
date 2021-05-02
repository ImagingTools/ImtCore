#include <imtqml/CQuickObjectComp.h>


// Qt includes
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>


namespace imtqml
{


CQuickObjectComp::CQuickObjectComp()
	:m_quickItemPtr(nullptr)
{
}


bool CQuickObjectComp::IsItemCreated() const
{
	return (m_quickItemPtr != nullptr);
}


// reimplemented (imtgui::IQuickObject)

bool CQuickObjectComp::CreateItem(QQuickItem* parentPtr)
{
	if (parentPtr == nullptr){
		return false;
	}

	if ((*m_pathToQmlAttrPtr).isEmpty()){
		return false;
	}

	if (m_quickItemPtr != nullptr){
		Q_ASSERT(false);

		return false;
	}

	m_quickItemPtr = CreateItem(qmlEngine(parentPtr));
	if (m_quickItemPtr != nullptr){
		m_quickItemPtr->setParentItem(parentPtr);

		OnItemCreated(*m_quickItemPtr);

		return true;
	}

	return false;
}


QQuickItem* CQuickObjectComp::CreateItem(QQmlEngine* enginePtr)
{
	if (enginePtr != nullptr){
		enginePtr->addImportPath("qrc:/qml");

		QQmlComponent component(enginePtr, QUrl("qrc" + m_pathToQmlAttrPtr->GetValue()));

		return qobject_cast<QQuickItem*>(component.create(enginePtr->rootContext()));
	}

	return nullptr;
}


QQuickItem* CQuickObjectComp::CreateItem(QQmlEngine* enginePtr, const QVariantMap& initialProperties)
{
	Q_UNUSED(initialProperties);

	if ((*m_pathToQmlAttrPtr).isEmpty()){
		return nullptr;
	}

	QQmlComponent component(enginePtr, QUrl("qrc" + m_pathToQmlAttrPtr->GetValue()));
#if QT_VERSION > QT_VERSION_CHECK(5, 14, 0)
	return qobject_cast<QQuickItem*>(component.createWithInitialProperties(initialProperties, enginePtr->rootContext()));

#endif
	return nullptr;
}


bool CQuickObjectComp::DestroyItem()
{
	return false;
}


QQuickItem* CQuickObjectComp::GetItem() const
{
	return m_quickItemPtr;
}


void CQuickObjectComp::OnTryClose(bool* /*ignoredPtr*/)
{
}


// protected methods

void CQuickObjectComp::OnItemCreated(QQuickItem& /*item*/)
{
}


// reimplemented (icomp::CComponentBase)

void CQuickObjectComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}



} // namespace imtqml



