#include <imtqml/CQuickObjectCompBase.h>


// Qt includes
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>


namespace imtqml
{


// public methods

CQuickObjectCompBase::CQuickObjectCompBase()
	:m_quickItemPtr(nullptr)
{
}


// reimplemented (imtgui::IQuickObject)

bool CQuickObjectCompBase::IsItemCreated() const
{
	return (m_quickItemPtr != nullptr);
}


bool CQuickObjectCompBase::CreateQuickItem(QQuickItem* parentPtr)
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

		OnItemCreated();

		return true;
	}

	return false;
}


bool CQuickObjectCompBase::DestroyQuickItem()
{
	if (m_quickItemPtr != nullptr){
		OnItemDestroyed();

		m_quickItemPtr->deleteLater();

		m_quickItemPtr = nullptr;

		return true;
	}

	return false;
}


QQuickItem* CQuickObjectCompBase::GetQuickItem() const
{
	return m_quickItemPtr;
}


void CQuickObjectCompBase::OnTryClose(bool* /*ignoredPtr*/)
{
}


// protected methods

QQuickItem* CQuickObjectCompBase::CreateItem(QQmlEngine* enginePtr) const
{
	if (enginePtr != nullptr){
		enginePtr->addImportPath("qrc:/qml");

		QQmlComponent component(enginePtr, QUrl("qrc" + m_pathToQmlAttrPtr->GetValue()));

		return qobject_cast<QQuickItem*>(component.create(enginePtr->rootContext()));
	}

	return nullptr;
}


QQuickItem* CQuickObjectCompBase::CreateItem(QQmlEngine* enginePtr, const QVariantMap& initialProperties) const
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


void CQuickObjectCompBase::OnItemCreated()
{
}


void CQuickObjectCompBase::OnItemDestroyed()
{
}


// reimplemented (icomp::CComponentBase)

void CQuickObjectCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}



} // namespace imtqml



