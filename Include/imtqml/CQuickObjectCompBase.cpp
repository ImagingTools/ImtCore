// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CQuickObjectCompBase.h>


// Qt includes
#include <QtCore/QFile>
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

		QString qmlFilePath = m_pathToQmlAttrPtr->GetValue();
		if (!qmlFilePath.startsWith(QStringLiteral("qrc"))){
			qmlFilePath.prepend(QStringLiteral("qrc"));
		}

		// endure, the file is existing. Remove first 'qml', because file access from resource MUST start from '/:'
		QFile qmlFile((QString(QString(qmlFilePath.data() + 3, qmlFilePath.size() - 3))));		
		if (!qmlFile.exists()){
			Q_ASSERT_X(false, __func__, QString("Unable to find file '%1'").arg(qmlFilePath + ' ' + qmlFile.fileName()).toLocal8Bit());

			return nullptr;
		}

		QUrl componentUrl(qmlFilePath);
		QQmlComponent component(enginePtr, componentUrl);

		QQmlContext* contextPtr = enginePtr->rootContext();
		contextPtr->setContextProperty("context", nullptr);

		QObject* createdComponentPtr = component.create(contextPtr);
		QQuickItem* quickItemPtr = qobject_cast<QQuickItem*>(createdComponentPtr);

		if (component.isError() || createdComponentPtr == nullptr || quickItemPtr == nullptr){
			qWarning() << "UNABLE TO CREATE QML COMPONENT " << componentUrl;
			qWarning() << component.errorString();
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


