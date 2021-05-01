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

	if (CreateItem(qmlEngine(parentPtr))){
		if (m_quickItemPtr != nullptr){
			m_quickItemPtr->setParentItem(parentPtr);
		}

		return (m_quickItemPtr != nullptr);
	}

	return false;
}


bool CQuickObjectComp::CreateItem(QQmlEngine* enginePtr)
{
	if (enginePtr != nullptr){
		QQmlComponent component(enginePtr, QUrl("qrc" + m_pathToQmlAttrPtr->GetValue()));

		m_quickItemPtr = qobject_cast<QQuickItem*>(component.create(enginePtr->rootContext()));

		return (m_quickItemPtr != nullptr);
	}

	return false;
}


bool CQuickObjectComp::CreateItem(QQmlEngine* enginePtr, const QVariantMap& initialProperties)
{
	Q_UNUSED(initialProperties);

	if ((*m_pathToQmlAttrPtr).isEmpty()){
		return false;
	}

	QQmlComponent component(enginePtr, QUrl("qrc" + m_pathToQmlAttrPtr->GetValue()));
#if QT_VERSION > QT_VERSION_CHECK(5, 14, 0)
	m_quickItemPtr = qobject_cast<QQuickItem*>(component.createWithInitialProperties(initialProperties, enginePtr->rootContext()));

	return (m_quickItemPtr != nullptr);
#endif
	return false;
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


} // namespace imtqml



