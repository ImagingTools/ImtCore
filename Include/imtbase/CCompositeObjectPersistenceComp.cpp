#include <imtbase/CCompositeObjectPersistenceComp.h>


// Qt includes
#include <QFile>
#include <QDir>
#include <QFileInfo>

// ACF includes
#include <ifile/CCompactXmlFileReadArchive.h>
#include <ifile/CCompactXmlFileWriteArchive.h>


namespace imtbase
{


// public methods



// reimplemented (ifile::IFilePersistence)

bool CCompositeObjectPersistenceComp::IsOperationSupported(
			const istd::IChangeable* dataObjectPtr,
			const QString* filePathPtr,
			int flags,
			bool /*beQuiet*/) const
{
	if ((filePathPtr == NULL) || (flags & QF_LOAD)){
		return false;
	}

	if (dataObjectPtr != NULL){
		const imtbase::IObjectCollection* documentPtr = dynamic_cast<const imtbase::IObjectCollection*>(dataObjectPtr);
		if (documentPtr == NULL){
			return false;
		}
	}

	return true;
}


int CCompositeObjectPersistenceComp::LoadFromFile(
			istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* /*progressManagerPtr*/) const
{
	const imtbase::IObjectCollection* documentPtr = dynamic_cast<const imtbase::IObjectCollection*>(&data);
	if (documentPtr == nullptr){
		return OS_FAILED;
	}

	if (!m_objectCollectionCompPtr.IsValid() || !m_objectPresistencesCompPtr.IsValid() || !m_objectTypeIds.IsValid()){
		return OS_FAILED;
	}

	if (m_objectPresistencesCompPtr.GetCount() != m_objectTypeIds.GetCount()){
		return OS_FAILED;
	}

	QDir dir(filePath);
	if (dir.exists()){
		if (dir.removeRecursively()){
			return OS_FAILED;
		}
	}

	int objectCounter = 0;

	const QString contentsFileName = dir.path() + "/contents.xml";
	ifile::CCompactXmlFileReadArchive xmlArchive(contentsFileName);

	static iser::CArchiveTag objectListTag("ObjectsList", "List of objects", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag objectTag("Object", "Object item", iser::CArchiveTag::TT_GROUP, &objectListTag);

	int objectCount;

	bool retVal = true;
	retVal = retVal && xmlArchive.BeginMultiTag(objectListTag, objectTag, objectCount);
	for (int objectCounter = 0; objectCounter < objectCount; objectCounter++){
		QString fileName;
		static iser::CArchiveTag fileNameTag("FileName", "Object file name", iser::CArchiveTag::TT_GROUP, &objectTag);
		retVal = retVal && xmlArchive.BeginTag(fileNameTag);
		retVal = retVal && xmlArchive.Process(fileName);
		retVal = retVal && xmlArchive.EndTag(fileNameTag);

		QByteArray typeId;
		static iser::CArchiveTag typeIdTag("TypeId", "Object type id", iser::CArchiveTag::TT_GROUP, &objectTag);
		retVal = retVal && xmlArchive.BeginTag(typeIdTag);
		retVal = retVal && xmlArchive.Process(typeId);
		retVal = retVal && xmlArchive.EndTag(typeIdTag);

		QByteArray objectId;
		static iser::CArchiveTag objectIdTag("ObjectId", "Object id", iser::CArchiveTag::TT_GROUP, &objectTag);
		retVal = retVal && xmlArchive.BeginTag(objectIdTag);
		retVal = retVal && xmlArchive.Process(objectId);
		retVal = retVal && xmlArchive.EndTag(objectIdTag);

		QString objectName;
		static iser::CArchiveTag objectNameTag("ObjectName", "Object name", iser::CArchiveTag::TT_GROUP, &objectTag);
		retVal = retVal && xmlArchive.BeginTag(objectNameTag);
		retVal = retVal && xmlArchive.Process(objectName);
		retVal = retVal && xmlArchive.EndTag(objectNameTag);

		if (!retVal){
			return OS_FAILED;
		}

		for (int i = 0; i < m_objectTypeIds.GetCount(); i++){
			if (m_objectTypeIds[i] == typeId){
				QFile objectFile;
				objectFile.setFileName(dir.path() + QString("/%1").arg(fileName));

				QString desc;
				QByteArray objectId = const_cast<imtbase::IObjectCollection*>(documentPtr)->InsertNewObject(typeId, objectName, desc);

				if (!objectId.isNull()){
					const istd::IChangeable* objectPtr = documentPtr->GetObjectPtr(objectId);
					if (objectPtr != nullptr){
						int status = m_objectPresistencesCompPtr[i]->LoadFromFile(*const_cast<istd::IChangeable*>(objectPtr), objectFile.fileName());
						if (status != ifile::IFilePersistence::OS_OK){
							const_cast<imtbase::IObjectCollection*>(documentPtr)->RemoveObject(objectId);
						}
					}
				}

				break;
			}
		}
	}
	retVal = retVal && xmlArchive.EndTag(objectListTag);

	if (retVal){
		return OS_OK;
	}
	else{
		return OS_FAILED;
	}
}


int CCompositeObjectPersistenceComp::SaveToFile(
			const istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* /*progressManagerPtr*/) const
{
	const imtbase::IObjectCollection* documentPtr = dynamic_cast<const imtbase::IObjectCollection*>(&data);
	if (documentPtr == nullptr){
		return OS_FAILED;
	}

	if (!m_objectCollectionCompPtr.IsValid() ||	!m_objectPresistencesCompPtr.IsValid() || !m_objectTypeIds.IsValid()){
		return OS_FAILED;
	}

	if (m_objectPresistencesCompPtr.GetCount() != m_objectTypeIds.GetCount()){
		return OS_FAILED;
	}

	QDir dir(filePath);
	if (dir.exists()){
		if (dir.removeRecursively()){
			return OS_FAILED;
		}
	}

	int objectCounter = 0;

	const QString contentsFileName = dir.path() + "/contents.xml";
	ifile::CCompactXmlFileWriteArchive xmlArchive(contentsFileName);

	static iser::CArchiveTag objectListTag("ObjectsList", "List of objects", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag objectTag("Object", "Object item", iser::CArchiveTag::TT_GROUP, &objectListTag);

	imtbase::ICollectionInfo::Ids ids = documentPtr->GetElementIds();
	int objectCount = ids.count();

	bool retVal = true;
	retVal = retVal && xmlArchive.BeginMultiTag(objectListTag, objectTag, objectCount);
	for (imtbase::ICollectionInfo::Id objectId: ids){
		const istd::IChangeable *objectPtr = documentPtr->GetObjectPtr(objectId);
		QByteArray typeId = documentPtr->GetElementInfo(objectId, imtbase::IObjectCollection::EIT_TYPE_ID).toByteArray();

		for (int i = 0; i < m_objectTypeIds.GetCount(); i++){
			if (m_objectTypeIds[i] == typeId){
				QFile objectFile;
				objectFile.setFileName(dir.path() + QString("/object%1").arg(objectCounter));
				m_objectPresistencesCompPtr[i]->SaveToFile(*objectPtr, objectFile.fileName());

				static iser::CArchiveTag fileNameTag("FileName", "Object file name", iser::CArchiveTag::TT_GROUP, &objectTag);
				retVal = retVal && xmlArchive.BeginTag(fileNameTag);
				QString fileName = objectFile.fileName();
				retVal = retVal && xmlArchive.Process(fileName);
				retVal = retVal && xmlArchive.EndTag(fileNameTag);
				
				static iser::CArchiveTag typeIdTag("TypeId", "Object type id", iser::CArchiveTag::TT_GROUP, &objectTag);
				retVal = retVal && xmlArchive.BeginTag(typeIdTag);
				retVal = retVal && xmlArchive.Process(typeId);
				retVal = retVal && xmlArchive.EndTag(typeIdTag);

				static iser::CArchiveTag objectIdTag("ObjectId", "Object id", iser::CArchiveTag::TT_GROUP, &objectTag);
				retVal = retVal && xmlArchive.BeginTag(objectIdTag);
				retVal = retVal && xmlArchive.Process(objectId);
				retVal = retVal && xmlArchive.EndTag(objectIdTag);

				static iser::CArchiveTag objectNameTag("ObjectName", "Object name", iser::CArchiveTag::TT_GROUP, &objectTag);
				retVal = retVal && xmlArchive.BeginTag(objectNameTag);
				QString objectName = documentPtr->GetElementInfo(objectId, imtbase::IObjectCollection::EIT_NAME).toString();
				retVal = retVal && xmlArchive.Process(objectName);
				retVal = retVal && xmlArchive.EndTag(objectNameTag);

				break;
			}
		}
	}
	retVal = retVal && xmlArchive.EndTag(objectListTag);

	if (retVal){
		return OS_OK;
	}
	else{
		return OS_FAILED;
	}
}


// reimplemented (ifile::IFileTypeInfo)

bool CCompositeObjectPersistenceComp::GetFileExtensions(QStringList& result, const istd::IChangeable* /*dataObjectPtr*/, int /*flags*/, bool doAppend) const
{
	if (!doAppend){
		result.clear();
	}

	result.push_back("zip");

	return true;
}


QString CCompositeObjectPersistenceComp::GetTypeDescription(const QString* /*extensionPtr*/) const
{
	return "Bundle files";
}


} // namespace imtbase


