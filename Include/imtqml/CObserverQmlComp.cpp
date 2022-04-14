#include "CObserverQmlComp.h"


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
			fields.append("LocalSettings");
			m_settingsModelPtr = m_pagesDataProviderCompPtr->GetTreeItemModel(params, fields);
			QVariant data = QVariant::fromValue(m_settingsModelPtr);
			quickItem->setProperty("localSettings", data);
			m_settingsObserver.RegisterObject(m_settingsModelPtr, &CObserverQmlComp::OnSettingsUpdated);
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

