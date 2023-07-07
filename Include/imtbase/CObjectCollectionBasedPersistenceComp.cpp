#include <imtbase/CObjectCollectionBasedPersistenceComp.h>


// Qt includes
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>


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
			istd::IChangeable& /*data*/,
			const QString& /*filePath*/,
			ibase::IProgressManager* /*progressManagerPtr*/) const
{
	Q_ASSERT(false);

	return OS_FAILED;
}


int CObjectCollectionBasedPersistenceComp::SaveToFile(
			const istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* /*progressManagerPtr*/) const
{
	if (m_collectionCompPtr.IsValid()){
		QUrl uri(filePath);
		if (uri.scheme() == "collection"){
			QString path = uri.path();
			QStringList parts = path.split("/");
			if (parts.count() == 2){
				QByteArray parentId = parts[0].toUtf8();
				QByteArray proposedId = parts[1].toUtf8();

				QString objectName;
				if (uri.hasQuery()) {
					QUrlQuery query(uri.query());
					if (query.hasQueryItem("name")) {
						objectName = query.queryItemValue("name");
					}
				}

				imtbase::ICollectionInfo::Ids ids = m_collectionCompPtr->GetElementIds(0, -1, nullptr);

				if (!proposedId.isEmpty() && objectName.isEmpty()){
					if (ids.contains(proposedId)){
						return m_collectionCompPtr->SetObjectData(proposedId, data) ? OS_OK : OS_FAILED;
					}

					return OS_FAILED;
				}

				if (proposedId.isEmpty() && !objectName.isEmpty()){
					for (const QByteArray& id : ids){
						QString collectionObjctName = m_collectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString();
						if (collectionObjctName == objectName){
							return m_collectionCompPtr->SetObjectData(id, data) ? OS_OK : OS_FAILED;
						}
					}

					QByteArray objectId = m_collectionCompPtr->InsertNewObject(*m_objectTypeIdAttrPtr, objectName, "", &data, proposedId, nullptr, nullptr);

					return !objectId.isEmpty() ? OS_OK : OS_FAILED;
				}

				if (!proposedId.isEmpty() && !objectName.isEmpty()){
					// The collection contains element with id == proposedId and name == objectName
					if (ids.contains(proposedId)){
						QString collectionObjctName = m_collectionCompPtr->GetElementInfo(proposedId, imtbase::ICollectionInfo::EIT_NAME).toString();
						if (collectionObjctName == objectName){
							return m_collectionCompPtr->SetObjectData(proposedId, data) ? OS_OK : OS_FAILED;
						}

						return OS_FAILED;
					}

					// The collection contains no elements with either id == proposedId or name == objectName
					{
						for (const QByteArray& id : ids){
							QString collectionObjctName = m_collectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString();
							if (collectionObjctName == objectName){
								return OS_FAILED;
							}
						}

						QByteArray objectId = m_collectionCompPtr->InsertNewObject(*m_objectTypeIdAttrPtr, objectName, "", &data, proposedId, nullptr, nullptr);

						return !objectId.isEmpty() ? OS_OK : OS_FAILED;
					}
				}
			}
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


