#include <imtqml/CQuickObjectCompBase.h>


// Qt includes
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>

// ImtCore includes
#include <imtqml/CApplicationDataEnumProviderComp.h>
//#include <imtqml/CEnumPageDataProviderComp.h>
//#include <imtqml/CEnumCommandDataProviderComp.h>


namespace imtqml
{


// public methods

CQuickObjectCompBase::CQuickObjectCompBase()
	:m_quickItemPtr(nullptr),
	  QObject()
{
}


QString CQuickObjectCompBase::GetPathToQml()
{
	return "QmlFilePath";
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

	QQmlEngine *engine = qmlEngine(parentPtr);
	if (engine != nullptr){
		if (m_baseUrlAttrPtr.IsValid() && *m_baseUrlAttrPtr != ""){
			engine->setBaseUrl(*m_baseUrlAttrPtr);
		}
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


void CQuickObjectCompBase::SetBaseUrl(const QString &baseUrl) const
{
	if (m_quickItemPtr != nullptr){
		QQmlEngine *engine = qmlEngine(m_quickItemPtr);
		engine->setBaseUrl(baseUrl);
	}
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
	if (m_dataProviderCompPtr.IsValid()){
		imtrest::QueryParams params;
		for (int i = 0; i < m_modelIdsAttrPtr.GetCount(); ++i){
			QByteArray modelId = m_modelIdsAttrPtr[i];
//			QByteArray modelQuery = m_modelQueriesAttrPtr[i];
//			params.append(QPair<QByteArray,QByteArray>(modelId,modelName));
//			imtbase::CTreeItemModel *itemModelPtr = m_dataProviderCompPtr->GetTreeItemModel(params);
			imtbase::CTreeItemModel *modelPtr = new imtbase::CTreeItemModel(this);
			connect(modelPtr, SIGNAL(needsReload()), this, SLOT(OnModelNeedsReload()));
			m_models.append(modelPtr);

			m_quickItemPtr->setProperty(modelId, QVariant::fromValue(modelPtr));
		}
		QMetaObject::invokeMethod(m_quickItemPtr, "firstModelsInit");
	}
}


void CQuickObjectCompBase::OnItemDestroyed()
{
}


// reimplemented (icomp::CComponentBase)

void CQuickObjectCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


void CQuickObjectCompBase::OnModelNeedsReload(imtbase::CTreeItemModel *itemModelPtr)
{
	imtbase::CTreeItemModel *rootModelPtr = itemModelPtr;
	if (rootModelPtr == nullptr){
		rootModelPtr = qobject_cast<imtbase::CTreeItemModel*>(sender());
	}
	if (rootModelPtr != nullptr && m_dataProviderCompPtr.IsValid()){
		int index  = m_models.indexOf(rootModelPtr);
		if (m_modelQueriesAttrPtr.IsValid() && m_modelQueriesAttrPtr.GetCount() > index && index > -1){
			QByteArray modelQuery = m_modelQueriesAttrPtr[index];
			imtrest::QueryParams queryParams = rootModelPtr->GetQueryParams();
			QList<QByteArray> query = modelQuery.split('/');
			imtbase::CTreeItemModel *sourceModelPtr = m_dataProviderCompPtr->GetTreeItemModel(query,queryParams);
			if (sourceModelPtr != nullptr){
				QByteArray modelId = m_modelIdsAttrPtr[index];
//				rootModelPtr->SetState("Loading");
//				rootModelPtr->SetExternTreeModel(imtqml::CEnumApplicationDataProviderComp::DATA, sourceModelPtr);
//				rootModelPtr->SetState("Ready");
//				rootModelPtr->Refresh();
//				m_quickItemPtr->setProperty(modelId, QVariant::fromValue(0));
//				QVariant modelVariant;
//				modelVariant = QVariant::fromValue(rootModelPtr);
				m_quickItemPtr->setProperty(modelId, QVariant::fromValue(sourceModelPtr));
				m_models[index] = sourceModelPtr;
				connect(sourceModelPtr, SIGNAL(needsReload()), this, SLOT(OnModelNeedsReload()));
//				disconnect(rootModelPtr, SIGNAL(needsReload()), this, SLOT(OnModelNeedsReload()));
				rootModelPtr->disconnect();
				delete rootModelPtr;
			}

		}

	}
}


} // namespace imtqml



