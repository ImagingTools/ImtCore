#include <imtbase/CObjectCollectionBasedPersistenceComp.h>


// Qt includes
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>


namespace imtbase
{


// reimplemented (ifile::IFilePersistence)

bool CObjectCollectionBasedPersistenceComp::IsOperationSupported(
			const istd::IChangeable* dataObjectPtr,
			const QString* filePathPtr,
			int flags,
			bool /*beQuiet*/) const
{
	if (filePathPtr != nullptr){
		QFileInfo fileInfo(*filePathPtr);

		QStringList extensions;
		GetFileExtensions(extensions, dataObjectPtr, flags);

		if (!extensions.contains(fileInfo.suffix())){
			return false;
		}
	}

	return true;
}


int CObjectCollectionBasedPersistenceComp::LoadFromFile(
			istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* /*progressManagerPtr*/) const
{
	Q_ASSERT(false);
	// TODO: check load from collection

	if (m_collectionCompPtr.IsValid()){
		QByteArray documentId;
		for (const QByteArray& id : m_collectionCompPtr->GetElementIds()){
			if (m_collectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString() == filePath){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_collectionCompPtr->GetObjectData(id, dataPtr)){
					data.CopyFrom(*dataPtr.GetPtr());
					return OS_OK;
				}
			}
		}
	}

	return OS_FAILED;
}


int CObjectCollectionBasedPersistenceComp::SaveToFile(
			const istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* /*progressManagerPtr*/) const
{
	if (m_collectionCompPtr.IsValid()){
		QByteArray documentId;
		for (const QByteArray& id : m_collectionCompPtr->GetElementIds()){
			if (m_collectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString() == filePath){
				documentId = id;
				break;
			}
		}

		bool result = false;
		if (documentId.isEmpty()){
			documentId = m_collectionCompPtr->InsertNewObject(*m_objectTypeIdAttrPtr, filePath, "", &data);
			result = !documentId.isEmpty();
		}
		else{
			result = m_collectionCompPtr->SetObjectData(documentId, data);
		}

		if (result){
			idoc::IDocumentMetaInfo* documentMetaInfoPtr = dynamic_cast<idoc::IDocumentMetaInfo*>(const_cast<istd::IChangeable*>(&data));
			if (documentMetaInfoPtr != nullptr){
				documentMetaInfoPtr->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_TITLE, filePath);
			}

			return OS_OK;
		}
	}

	return OS_FAILED;
}


// reimplemented (ifile::IFileTypeInfo)

bool CObjectCollectionBasedPersistenceComp::GetFileExtensions(QStringList& result, const istd::IChangeable* dataObjectPtr, int flags, bool doAppend) const
{
	if (m_fileTypeInfoCompPtr.IsValid()){
		return m_fileTypeInfoCompPtr->GetFileExtensions(result, dataObjectPtr, flags, doAppend);
	}

	if (!doAppend){
		result.clear();
	}

	int count = GetExtensionCount();
	for (int i = 0; i < count; i++){
		result.push_back(m_extensionAttrPtr[i]);
	}

	return true;
}


QString CObjectCollectionBasedPersistenceComp::GetTypeDescription(const QString* extensionPtr) const
{
	if (m_fileTypeInfoCompPtr.IsValid()){
		return m_fileTypeInfoCompPtr->GetTypeDescription(extensionPtr);
	}

	if (extensionPtr == nullptr){
		return *m_commonDescriptionAttrPtr;
	}

	int count = GetExtensionCount();
	for (int i = 0; i < count; i++){
		if (m_extensionAttrPtr[i] == *extensionPtr){
			return m_extensionDescAttrPtr[i];
		}
	}

	return QString();
}


// private methods

int CObjectCollectionBasedPersistenceComp::GetExtensionCount() const
{
	return qMin(m_extensionAttrPtr.GetCount(), m_extensionDescAttrPtr.GetCount());
}


} // namespace imtbase


