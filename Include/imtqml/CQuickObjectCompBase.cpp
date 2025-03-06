#include <imtqml/CQuickObjectCompBase.h>


// Qt includes
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlContext>


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

	QQmlEngine* engine = qmlEngine(parentPtr);
	if (engine != nullptr){
		InitializeEngine(*engine);

		m_quickItemPtr = CreateItem(engine);
		if (m_quickItemPtr != nullptr){
			m_quickItemPtr->setParentItem(parentPtr);

			OnItemCreated();

			return true;
		}
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

bool CQuickObjectCompBase::InitializeEngine(QQmlEngine& /*qmlEngine*/) const
{
	return true;
}


void CQuickObjectCompBase::OnItemCreated()
{
}


void CQuickObjectCompBase::OnItemDestroyed()
{
}


QQuickItem* CQuickObjectCompBase::CreateItem(QQmlEngine* enginePtr) const
{
	if (enginePtr != nullptr){
		enginePtr->addImportPath("qrc:/qml");

		QUrl componentUrl("qrc" + m_pathToQmlAttrPtr->GetValue());
		QQmlComponent component(enginePtr, componentUrl);

		QQmlContext* contextPtr = enginePtr->rootContext();
		contextPtr->setContextProperty("context", nullptr);

		QObject* createdComponentPtr = component.create(contextPtr);
		QQuickItem* quickItemPtr = qobject_cast<QQuickItem*>(createdComponentPtr);

		if (component.isError()){
			qWarning() << "UNABLE TO CREATE QML COMPONENT " << componentUrl;
			qWarning() << component.errors();
			qWarning() << "----------------";
		}

		return quickItemPtr;
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


} // namespace imtqml


