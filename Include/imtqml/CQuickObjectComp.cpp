#include <imtqml/CQuickObjectComp.h>


// Qt includes
#include <QtCore/QString>
#include <QtCore/QTimer>
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


bool CQuickObjectComp::CreateItem(QQuickItem *parentPtr)
{
	if (parentPtr == nullptr || m_pathToQmlPtr.IsValid() == false){
		return false;
	}

	QQmlEngine *engine = qmlEngine(parentPtr);

	CreateItem(engine);

	if (m_quickItemPtr != nullptr){
		m_quickItemPtr->setParentItem(parentPtr);
	}

	return  IsItemCreated();
}


bool CQuickObjectComp::CreateItem(QQmlEngine *enginePtr)
{
	if (m_pathToQmlPtr.IsValid() == false){
		return false;
	}

//	QQmlComponent component(enginePtr, QUrl::fromLocalFile(m_pathToQmlPtr->GetValue()));
	QQmlComponent component(enginePtr, QUrl("qrc" + m_pathToQmlPtr->GetValue()));
	m_quickItemPtr = qobject_cast<QQuickItem*>(component.create(enginePtr->rootContext()));

	return  IsItemCreated();
}

bool CQuickObjectComp::CreateItem(QQmlEngine *enginePtr, const QVariantMap &initialProperties)
{
	if (m_pathToQmlPtr.IsValid() == false){
		return false;
	}

//	QQmlComponent component(enginePtr, QUrl::fromLocalFile(m_pathToQmlPtr->GetValue()));
	QQmlComponent component(enginePtr, QUrl("qrc" + m_pathToQmlPtr->GetValue()));
	m_quickItemPtr = qobject_cast<QQuickItem*>(component.createWithInitialProperties(initialProperties, enginePtr->rootContext()));

	return  IsItemCreated();
}

bool CQuickObjectComp::DestroyItem()
{
	return false;
}


QQuickItem *CQuickObjectComp::GetItem() const
{
	return m_quickItemPtr;
}


void CQuickObjectComp::OnTryClose(bool* /*ignoredPtr*/)
{
}




} // namespace imtqml



