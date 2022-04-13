#include "CObserverQmlComp.h"


namespace imtqml
{


// reimplemented (ilog::CLoggerComponentBase)
void CObserverQmlComp::OnComponentCreated()
{
	ilog::CLoggerComponentBase::OnComponentCreated();
    if (m_quickObjectComp.IsValid()){

		QQuickItem* quickItem = m_quickObjectComp->GetQuickItem();
		connect(quickItem, SIGNAL(changeSourceItem(QString)), this, SLOT(OnChangeSourceItem(QString)));
		QMetaObject::invokeMethod(quickItem, "updateModels");

		QList<imtgql::CGqlObject> params;
		QByteArrayList fields;
		fields.append("LocalSettings");
		imtbase::CTreeItemModel* rootModelPtr = m_pagesDataProviderCompPtr->GetTreeItemModel(params, fields);
		QVariant data = QVariant::fromValue(rootModelPtr);
		quickItem->setProperty("localSettings", data);
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

