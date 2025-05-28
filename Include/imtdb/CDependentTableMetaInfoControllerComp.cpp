#include <imtdb/CDependentTableMetaInfoControllerComp.h>


namespace imtdb
{


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CDependentTableMetaInfoControllerComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!m_dependentMetaInfoControllerCompPtr.IsValid()){
		return;
	}
	
	if (!changeSet.Contains(imtbase::ICollectionInfo::CF_ADDED) &&
		!changeSet.Contains(imtbase::ICollectionInfo::CF_ELEMENT_DESCRIPTION_CHANGED) &&
		!changeSet.Contains(imtbase::ICollectionInfo::CF_ELEMENT_RENAMED) &&
		!changeSet.Contains(imtbase::ICollectionInfo::CF_ELEMENT_STATE) &&
		!changeSet.Contains(imtbase::IObjectCollection::CF_OBJECT_DATA_CHANGED) &&
		!changeSet.Contains(imtbase::ICollectionInfo::CF_REMOVED)){
		return;
	}
	
	bool isRemoved = false;
	QByteArray elementId;
	if (changeSet.Contains(imtbase::ICollectionInfo::CF_ADDED)){
		elementId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED).toByteArray();
	}
	else if (changeSet.Contains(imtbase::ICollectionInfo::CF_ELEMENT_DESCRIPTION_CHANGED)){
		elementId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_DESCRIPTION_CHANGED).toByteArray();
	}
	else if (changeSet.Contains(imtbase::ICollectionInfo::CF_ELEMENT_RENAMED)){
		elementId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_RENAMED).toByteArray();
	}
	else if (changeSet.Contains(imtbase::ICollectionInfo::CF_ELEMENT_STATE)){
		elementId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_STATE).toByteArray();
	}
	else if (changeSet.Contains(imtbase::IObjectCollection::CF_OBJECT_DATA_CHANGED)){
		elementId = changeSet.GetChangeInfo(imtbase::IObjectCollection::CN_OBJECT_DATA_CHANGED).toByteArray();
	}
	else if (changeSet.Contains(imtbase::ICollectionInfo::CF_REMOVED)){
		elementId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENTS_REMOVED).toByteArray();
		
		isRemoved = true;
	}
	
	if (elementId.isEmpty()){
		return;
	}
	
	imtdb::IDependentMetaInfoController::DependentMetaInfo dependentMetaInfo;
	
	dependentMetaInfo.objectId = elementId;
	dependentMetaInfo.dependentKey = *m_metaInfoIdAttrPtr;
	
	bool isDocumentSource = m_isDocumentSourceAttrPtr.IsValid() ? *m_isDocumentSourceAttrPtr : true;
	
	for (int i = 0; i < m_metaInfoNameAttrPtr.GetCount(); i++){
		QString metaInfoName = m_metaInfoNameAttrPtr[i];
		QString dependentMetaInfoName = m_dependentMetaInfoNameAttrPtr[i];
		dependentMetaInfo.metaInfoIds << metaInfoName;

		if (!isRemoved){
			QString selectValue =
				QString(R"((SELECT "%0"->>'%1' FROM "%2" WHERE "State" = 'Active' AND "DocumentId" = '%3' LIMIT 1)
						)").arg(isDocumentSource ? "Document" : "DataMetaInfo",
								dependentMetaInfoName,
								*m_dependentTableNameAttrPtr,
								qPrintable(elementId));
			
			dependentMetaInfo.metaInfoValues << selectValue;
		}
	}
	
	if (isRemoved){
		if (!m_dependentMetaInfoControllerCompPtr->ClearDependentMetaInfo(dependentMetaInfo)){
			SendErrorMessage(0, QString("Unable to clear dependent meta info"), "CDependentTableMetaInfoControllerComp");
		}
	}
	else{
		if (!m_dependentMetaInfoControllerCompPtr->UpdateDependentMetaInfo(dependentMetaInfo)){
			SendErrorMessage(0, QString("Unable to update dependent meta info"), "CDependentTableMetaInfoControllerComp");
		}
	}
}


// reimplemented (icomp::CComponentBase)

void CDependentTableMetaInfoControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
	
	if (m_objectCollectionModelCompPtr.IsValid()){
		m_objectCollectionModelCompPtr->AttachObserver(this);
	}
}


void CDependentTableMetaInfoControllerComp::OnComponentDestroyed()
{
	EnsureModelDetached();
	
	BaseClass::OnComponentDestroyed();
}


} // namespace imtdb


