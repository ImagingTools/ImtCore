#include "CObserverQmlComp.h"

// Qt includes
#include <QtQml/QQmlEngine>


namespace imtqml
{


CObserverQmlComp::CObserverQmlComp()
	:m_settingsModelPtr(nullptr),
	m_settingsObserver(*this)
{
}


//protected methods
void CObserverQmlComp::OnSettingsUpdated(const istd::IChangeable::ChangeSet &changeSet, const imtbase::CTreeItemModel *settingsModelPtr)
{
	imtbase::CTreeItemModel* model = const_cast<imtbase::CTreeItemModel*>(settingsModelPtr);
	QList<imtgql::CGqlObject> params;

	if (changeSet.GetChangeInfoMap().count() > 0){
		ApplyUrl();
		m_mutationDataDelegateCompPtr->UpdateBaseModelFromRepresentation(params, model);
	}
}


void CObserverQmlComp::ApplyUrl() const
{
	const imtbase::CTreeItemModel* pageModelPtr = GetPageModel("NetworkSettings");
	if (pageModelPtr != nullptr){
		imtbase::CTreeItemModel* elementsModelPtr = pageModelPtr->GetTreeItemModel("Elements");
		if (elementsModelPtr != nullptr && m_quickObjectComp.IsValid()){
			if (elementsModelPtr->ContainsKey("Value")){
				QString serverUrl = elementsModelPtr->GetData("Value").toString();
				QQuickItem* quickItem = m_quickObjectComp->GetQuickItem();
				QQmlEngine* engine = qmlEngine(quickItem);
				engine->setBaseUrl(serverUrl + *m_prefixServer);
			}
		}
	}
}


// reimplemented (ilog::CLoggerComponentBase)
void CObserverQmlComp::OnComponentCreated()
{
	ilog::CLoggerComponentBase::OnComponentCreated();
	if (m_quickObjectComp.IsValid()){

		QQuickItem* quickItem = m_quickObjectComp->GetQuickItem();

		if (quickItem != nullptr){
//			connect(quickItem, SIGNAL(changeSourceItem(QString)), this, SLOT(OnChangeSourceItem(QString)));

			if (m_pagesDataProviderCompPtr.IsValid()){
				QList<imtgql::CGqlObject> params;
				imtgql::CGqlObject *inputParams = new imtgql::CGqlObject("input");
				inputParams->InsertField("LanguageId", "ru_RU");
				params.append(*inputParams);
				QByteArrayList fields;
				fields.append("NetworkSettings");
				m_settingsModelPtr = m_pagesDataProviderCompPtr->GetTreeItemModel(params, fields);
				if (m_settingsModelPtr != nullptr){
					QVariant data = QVariant::fromValue(m_settingsModelPtr);
					quickItem->setProperty("localSettings", data);
				}
//				m_settingsObserver.RegisterObject(m_settingsModelPtr, &CObserverQmlComp::OnSettingsUpdated);
//				connect(m_settingsModelPtr, SIGNAL(modelChanged), this, SLOT(OnModelChanged));

				connect(quickItem, SIGNAL(settingsUpdate(QString)), this, SLOT(OnModelChanged(QString)));

				ApplyUrl();
				QMetaObject::invokeMethod(quickItem, "updateModels");
			}
		}
	}
}


void CObserverQmlComp::OnComponentDestroyed()
{
	ilog::CLoggerComponentBase::OnComponentDestroyed();
	m_settingsObserver.UnregisterAllObjects();
	if (m_settingsModelPtr != nullptr){
		delete m_settingsModelPtr;
		m_settingsModelPtr = nullptr;
	}
}


void CObserverQmlComp::OnChangeSourceItem(QString src)
{
	if (m_quickObjectComp.IsValid()){
		QQuickItem* quickItem = m_quickObjectComp->GetQuickItem();
		quickItem->setProperty("sourceItem", "qrc:///qml/"+ src);
	}
}


void CObserverQmlComp::OnModelChanged(const QString& pageId)
{
	qDebug() << "ObserverQml OnModelChanged";
	QList<imtgql::CGqlObject> params;

	imtgql::CGqlObject gqlObject;
	gqlObject.InsertField("Id", pageId);
	params << gqlObject;

	const imtbase::CTreeItemModel* pageModelPtr = GetPageModel(pageId.toUtf8());
	if (pageModelPtr != nullptr){
		ApplyUrl();
		m_mutationDataDelegateCompPtr->UpdateBaseModelFromRepresentation(params, const_cast<imtbase::CTreeItemModel*>(pageModelPtr));
	}
}


const imtbase::CTreeItemModel* CObserverQmlComp::GetPageModel(const QByteArray &pageId) const
{
	imtbase::CTreeItemModel* pageModelPtr = nullptr;

	for (int i = 0; i < m_settingsModelPtr->GetItemsCount(); i++){
		if (m_settingsModelPtr->ContainsKey("Id", i)){
			QByteArray currentPageId = m_settingsModelPtr->GetData("Id", i).toByteArray();
			if (currentPageId == pageId){
				pageModelPtr = new imtbase::CTreeItemModel();
				int index = pageModelPtr->InsertNewItem();
				m_settingsModelPtr->CopyItemDataToModel(i, pageModelPtr, index);
				break;
			}
		}
	}

	return pageModelPtr;
}


} // namespace imtqml

