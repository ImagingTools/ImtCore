#include "CObserverQmlComp.h"

// Qt includes
#include <QtQml/QQmlEngine>


namespace imtqml
{


CObserverQmlComp::CObserverQmlComp()
	: m_settingsModelPtr(nullptr),
	  m_settingsObserver(*this)
{

}


//protected methods
void CObserverQmlComp::OnSettingsUpdated(const istd::IChangeable::ChangeSet &changeSet, const imtbase::CTreeItemModel *settingsModelPtr)
{
	imtbase::CTreeItemModel* model = const_cast<imtbase::CTreeItemModel*>(settingsModelPtr);
	QList<imtgql::CGqlObject> params;

	if (changeSet.GetChangeInfoMap().count() > 0){
		ApplyUrl(settingsModelPtr);
		m_mutationDataDelegateCompPtr->UpdateBaseModelFromRepresentation(params, model);
	}
}


void CObserverQmlComp::ApplyUrl(const imtbase::CTreeItemModel *settingsModelPtr)
{
	imtbase::CTreeItemModel* elementsModel = settingsModelPtr->GetTreeItemModel("Elements");
	if (elementsModel != nullptr && m_quickObjectComp.IsValid()){
		QString serverUrl = elementsModel->GetData("Value").toString();
		QQuickItem* quickItem = m_quickObjectComp->GetQuickItem();
		QQmlEngine* engine = qmlEngine(quickItem);
		engine->setBaseUrl(serverUrl + "/Lisa");
		QMetaObject::invokeMethod(quickItem, "updateModels");
	}
}


// reimplemented (ilog::CLoggerComponentBase)
void CObserverQmlComp::OnComponentCreated()
{
	ilog::CLoggerComponentBase::OnComponentCreated();
    if (m_quickObjectComp.IsValid()){

		QQuickItem* quickItem = m_quickObjectComp->GetQuickItem();
		connect(quickItem, SIGNAL(changeSourceItem(QString)), this, SLOT(OnChangeSourceItem(QString)));
		QMetaObject::invokeMethod(quickItem, "updateModels");

		if (m_pagesDataProviderCompPtr.IsValid()){
			QList<imtgql::CGqlObject> params;
			QByteArrayList fields;
			fields.append("NetworkSettings");
			m_settingsModelPtr = m_pagesDataProviderCompPtr->GetTreeItemModel(params, fields);
			QVariant data = QVariant::fromValue(m_settingsModelPtr);
			quickItem->setProperty("localSettings", data);
			m_settingsObserver.RegisterObject(m_settingsModelPtr, &CObserverQmlComp::OnSettingsUpdated);

			ApplyUrl(m_settingsModelPtr);
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


} // namespace imtqml

