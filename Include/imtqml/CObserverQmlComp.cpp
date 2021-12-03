#include "CObserverQmlComp.h"


namespace imtqml
{


// reimplemented (ilog::CLoggerComponentBase)
void CObserverQmlComp::OnComponentCreated()
{
	ilog::CLoggerComponentBase::OnComponentCreated();
    if (m_quickObjectComp.IsValid()){
        QQuickItem* quickItem = m_quickObjectComp->GetQuickItem();
//        quickItem->setProperty("sourceItem", "qrc:///qml/Pages/ClassifierObjectsWithConsumer.qml");
        //connect(quickItem, SIGNAL(registrationSignal(imtbase::CTreeItemModel*)), this, SLOT(OnRegistrationSignal(imtbase::CTreeItemModel*)));
        connect(quickItem, SIGNAL(changeSourceItem(QString)), this, SLOT(OnChangeSourceItem(QString)));
		QMetaObject::invokeMethod(quickItem, "updateModels");
    }
}

void CObserverQmlComp::OnComponentDestroyed()
{
    ilog::CLoggerComponentBase::OnComponentDestroyed();
}


void CObserverQmlComp::OnChangeSourceItem(QString src)
{
    if (m_quickObjectComp.IsValid()){
        QQuickItem* quickItem = m_quickObjectComp->GetQuickItem();
        quickItem->setProperty("sourceItem", "qrc:///qml/"+ src);
    }
}


} // namespace imtqml

