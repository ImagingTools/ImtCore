#include <imtbase/CCollectionDataControllerComp.h>


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


// reimplemented (ICollectionDataController)

const ifile::IFilePersistence* CCollectionDataControllerComp::GetPersistenceForObjectType(const QByteArray& typeId) const
{
	int persistenceIndex = -1;

	if (m_resourceTypesCompPtr.IsValid()){
		for (int i = 0; i < m_resourceTypesCompPtr->GetOptionsCount(); ++i){
			if (typeId == m_resourceTypesCompPtr->GetOptionId(i)){
				persistenceIndex = i;
				break;
			}
		}
	}

	if ((persistenceIndex >= 0) && persistenceIndex < m_objectPersistenceListCompPtr.GetCount()){
		return m_objectPersistenceListCompPtr[persistenceIndex];
	}

	return nullptr;
}


bool CCollectionDataControllerComp::ExportFile(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& targetFilePath) const
{
	QByteArray typeId = collection.GetObjectTypeId(objectId);

	const ifile::IFilePersistence* persistencePtr = GetPersistenceForObjectType(typeId);
	if (persistencePtr != nullptr){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (collection.GetObjectData(objectId, dataPtr)){
			return persistencePtr->SaveToFile(*dataPtr, targetFilePath) == ifile::IFilePersistence::OS_OK;
		}
	}

	return false;
}


QByteArray CCollectionDataControllerComp::ImportFile(
			imtbase::IObjectCollection& collection,
			const QByteArray& typeId,
			const QString& sourceFilePath,
			const ICollectionInfo::Id& /*parentId*/) const
{
	return QByteArray();
}


} // namespace imtbase


