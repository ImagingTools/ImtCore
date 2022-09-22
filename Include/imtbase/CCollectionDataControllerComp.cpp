#include <imtbase/CCollectionDataControllerComp.h>


// Qt includes
#include <QtCore/QFileInfo>

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
	const ifile::IFilePersistence* persistencePtr = GetPersistenceForObjectType(typeId);
	if (persistencePtr != nullptr){
		imtbase::IObjectCollection::DataPtr dataPtr(CreateObjectInstance(typeId));
		if (dataPtr.IsValid()) {
			int state= persistencePtr->LoadFromFile(*dataPtr, sourceFilePath);
			if (state == ifile::IFilePersistence::OS_OK){
				QFileInfo fileInfo(sourceFilePath);

				return collection.InsertNewObject(typeId, fileInfo.baseName(), QString(QObject::tr("Import from %1").arg(sourceFilePath)), dataPtr);
			}
		}
	}

	return QByteArray();
}


// protected methods

istd::IChangeable* CCollectionDataControllerComp::CreateObjectInstance(const QByteArray& typeId) const
{
	int factoryIndex = -1;

	if (m_resourceTypesCompPtr.IsValid()){
		for (int i = 0; i < m_resourceTypesCompPtr->GetOptionsCount(); ++i){
			if (typeId == m_resourceTypesCompPtr->GetOptionId(i)){
				factoryIndex = i;
				break;
			}
		}
	}

	if ((factoryIndex >= 0) && factoryIndex < m_objectFactoryListCompPtr.GetCount()){
		return m_objectFactoryListCompPtr.CreateInstance(factoryIndex);
	}

	return nullptr;
}


} // namespace imtbase


