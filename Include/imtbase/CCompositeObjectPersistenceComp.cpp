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
	imtbase::IObjectCollection* documentPtr = const_cast<imtbase::IObjectCollection*>(dynamic_cast<const imtbase::IObjectCollection*>(&data));
	if (documentPtr == nullptr){
		return OS_FAILED;
	}

	if (!m_objectCollectionCompPtr.IsValid() || !m_objectTypeIdsAttrPtr.IsValid() || !m_objectPresistencesCompPtr.IsValid()){
		return OS_FAILED;
	}

	const QString contentsFileName = filePath + "/Contents.xml";
	ifile::CCompactXmlFileReadArchive xmlArchive;
	if (!xmlArchive.OpenFile(contentsFileName)){
		return OS_FAILED;
	}

	QVector<BundleElementInfo> contentMetaInfo;
	if (!SerializeBundleMetaInfo(contentMetaInfo, xmlArchive)){
		return OS_FAILED;
	}

	for (BundleElementInfo elementInfo : contentMetaInfo){
		const ifile::IFilePersistence* persistencePtr = GetFilePersistenceForTypeId(elementInfo.typeId);
		if (persistencePtr == nullptr){
			return OS_FAILED;
		}
		
		istd::IChangeable* objectPtr = const_cast<istd::IChangeable*>(documentPtr->GetObjectPtr(elementInfo.id));
		if (objectPtr == nullptr){
			elementInfo.id = documentPtr->InsertNewObject(elementInfo.typeId, elementInfo.name, elementInfo.description);

			objectPtr = const_cast<istd::IChangeable*>(documentPtr->GetObjectPtr(elementInfo.id));
		}
	
		if (objectPtr == nullptr){
			return OS_FAILED;
		}

		int status = persistencePtr->LoadFromFile(*objectPtr, filePath + "/" + elementInfo.fileName);
		if (status != ifile::IFilePersistence::OS_OK){
			return OS_FAILED;
		}
	}

	return OS_OK;
}


int CCompositeObjectPersistenceComp::SaveToFile(
			const istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* /*progressManagerPtr*/) const
{
	imtbase::IObjectCollection* documentPtr = const_cast<imtbase::IObjectCollection*>(dynamic_cast<const imtbase::IObjectCollection*>(&data));
	if (documentPtr == nullptr){
		return OS_FAILED;
	}

	if (!m_objectCollectionCompPtr.IsValid() || !m_objectTypeIdsAttrPtr.IsValid() || !m_objectPresistencesCompPtr.IsValid()){
		return OS_FAILED;
	}

	QDir dir(filePath);
	if (dir.exists()){
		if (!dir.removeRecursively()){
			return OS_FAILED;
		}
	}

	if (!dir.mkpath(".")){
		return OS_FAILED;
	}

	const QString contentsFileName = filePath + "/Contents.xml";
	ifile::CCompactXmlFileWriteArchive xmlArchive(contentsFileName);

	imtbase::ICollectionInfo::Ids ids = documentPtr->GetElementIds();
	int objectCounter = 0;
	
	QVector<BundleElementInfo> contentMetaInfo;
	
	for (imtbase::ICollectionInfo::Id objectId: ids){
		const istd::IChangeable *objectPtr = documentPtr->GetObjectPtr(objectId);
		if (objectPtr == nullptr){
			return OS_FAILED;
		}

		QByteArray typeId = documentPtr->GetElementInfo(objectId, imtbase::IObjectCollection::EIT_TYPE_ID).toByteArray();

		const ifile::IFilePersistence* persistencePtr = GetFilePersistenceForTypeId(typeId);
		if (persistencePtr == nullptr){
			return OS_FAILED;
		}

		QString objectFile;
		objectFile = QString("object%1").arg(objectCounter++);
		if (persistencePtr->SaveToFile(*objectPtr, filePath + "/" + objectFile) != OS_OK){
			return OS_FAILED;
		}
			
		BundleElementInfo elementInfo;
		
		elementInfo.fileName = objectFile;
		elementInfo.id = objectId;
		elementInfo.typeId = typeId;
		elementInfo.name = documentPtr->GetElementInfo(objectId, imtbase::IObjectCollection::EIT_NAME).toString();
		elementInfo.description = documentPtr->GetElementInfo(objectId, imtbase::IObjectCollection::EIT_DESCRIPTION).toString();

		contentMetaInfo.append(elementInfo);
	}

	return SerializeBundleMetaInfo(contentMetaInfo, xmlArchive);
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


// protected methods

const ifile::IFilePersistence* CCompositeObjectPersistenceComp::GetFilePersistenceForTypeId(const QByteArray& typeId) const
{
	if (!m_objectTypeIdsAttrPtr.IsValid() || !m_objectPresistencesCompPtr.IsValid()){
		return nullptr;
	}

	int presistencesCount = qMin(m_objectTypeIdsAttrPtr.GetCount(), m_objectPresistencesCompPtr.GetCount());
	for (int i = 0; i < presistencesCount; i++){
		if (m_objectTypeIdsAttrPtr[i] == typeId){
			return m_objectPresistencesCompPtr[i];
		}
	}

	return nullptr;
}


bool CCompositeObjectPersistenceComp::SerializeBundleMetaInfo(QVector<BundleElementInfo>& contentMetaInfo, iser::IArchive& archive) const
{
	int objectCount = contentMetaInfo.count();

	if (!archive.IsStoring()){
		objectCount = 0;

		contentMetaInfo.clear();
	}

	static iser::CArchiveTag objectListTag("ObjectsList", "List of objects", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag objectTag("Object", "Object item", iser::CArchiveTag::TT_GROUP, &objectListTag);

	bool retVal = true;

	retVal = retVal && archive.BeginMultiTag(objectListTag, objectTag, objectCount);
	
	for (int i = 0; i < objectCount; i++){
		retVal = retVal && archive.BeginTag(objectTag);

		BundleElementInfo elementInfo;
		if (archive.IsStoring()){
			elementInfo = contentMetaInfo[i];
		}

		static iser::CArchiveTag fileNameTag("FileName", "Object file name", iser::CArchiveTag::TT_LEAF, &objectTag);
		retVal = retVal && archive.BeginTag(fileNameTag);
		retVal = retVal && archive.Process(elementInfo.fileName);
		retVal = retVal && archive.EndTag(fileNameTag);

		static iser::CArchiveTag objectIdTag("Id", "Object ID", iser::CArchiveTag::TT_LEAF, &objectTag);
		retVal = retVal && archive.BeginTag(objectIdTag);
		retVal = retVal && archive.Process(elementInfo.id);
		retVal = retVal && archive.EndTag(objectIdTag);

		static iser::CArchiveTag typeIdTag("TypeId", "Object type-ID", iser::CArchiveTag::TT_LEAF, &objectTag);
		retVal = retVal && archive.BeginTag(typeIdTag);
		retVal = retVal && archive.Process(elementInfo.typeId);
		retVal = retVal && archive.EndTag(typeIdTag);

		static iser::CArchiveTag objectNameTag("Name", "Object name", iser::CArchiveTag::TT_LEAF, &objectTag);
		retVal = retVal && archive.BeginTag(objectNameTag);
		retVal = retVal && archive.Process(elementInfo.name);
		retVal = retVal && archive.EndTag(objectNameTag);

		static iser::CArchiveTag objectDescriptionTag("Description", "Object description", iser::CArchiveTag::TT_LEAF, &objectTag);
		retVal = retVal && archive.BeginTag(objectDescriptionTag);
		retVal = retVal && archive.Process(elementInfo.description);
		retVal = retVal && archive.EndTag(objectDescriptionTag);

		if (retVal && !archive.IsStoring()){
			contentMetaInfo.append(elementInfo);
		}

		retVal = retVal && archive.EndTag(objectTag);
	}

	retVal = retVal && archive.EndTag(objectListTag);

	return retVal;
}


} // namespace imtbase


