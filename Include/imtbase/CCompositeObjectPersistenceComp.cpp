#include <imtbase/CCompositeObjectPersistenceComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QUuid>

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
	imtbase::IObjectCollection* documentPtr = dynamic_cast<imtbase::IObjectCollection*>(&data);
	if (documentPtr == nullptr){
		return OS_FAILED;
	}

	if (!m_objectTypeIdsAttrPtr.IsValid() || !m_objectPresistencesCompPtr.IsValid()){
		return OS_FAILED;
	}

	QDir tempPath = QDir::temp();
	QString uuid = QUuid::createUuid().toString();

	if (!tempPath.mkpath(uuid)){
		return OS_FAILED;
	}

	tempPath.cd(uuid);

	if (m_fileCompressionCompPtr.IsValid()){
		if (!m_fileCompressionCompPtr->DecompressFolder(filePath, tempPath.path())){
			tempPath.removeRecursively();

			return OS_FAILED;
		}
	}

	const QString contentsFileName = tempPath.path() + QDir::separator() + "Contents.xml";
	ifile::CCompactXmlFileReadArchive xmlArchive;
	if (!xmlArchive.OpenFile(contentsFileName)){
		tempPath.removeRecursively();

		return OS_FAILED;
	}

	QVector<BundleElementInfo> contentMetaInfo;
	if (!SerializeBundleMetaInfo(contentMetaInfo, xmlArchive)){
		tempPath.removeRecursively();

		return OS_FAILED;
	}

	for (const BundleElementInfo& elementInfo : contentMetaInfo){
		const ifile::IFilePersistence* persistencePtr = GetFilePersistenceForTypeId(elementInfo.typeId);
		if (persistencePtr == nullptr){
			tempPath.removeRecursively();

			return OS_FAILED;
		}
		
		istd::IChangeable* objectPtr = const_cast<istd::IChangeable*>(documentPtr->GetObjectPtr(elementInfo.id));
		if (objectPtr == nullptr){
			if ((documentPtr->GetOperationFlags() & imtbase::IObjectCollection::OF_SUPPORT_INSERT) == 0){
				tempPath.removeRecursively();

				return OS_FAILED;
			}

			QByteArray id = documentPtr->InsertNewObject(elementInfo.typeId, elementInfo.name, elementInfo.description, nullptr, elementInfo.id);

			objectPtr = const_cast<istd::IChangeable*>(documentPtr->GetObjectPtr(id));
		}
	
		if (objectPtr == nullptr){
			tempPath.removeRecursively();

			return OS_FAILED;
		}

		int status = persistencePtr->LoadFromFile(*objectPtr, tempPath.path() + QDir::separator() + elementInfo.fileName);
		if (status != ifile::IFilePersistence::OS_OK){
			tempPath.removeRecursively();

			return OS_FAILED;
		}
	}

	if (!LoadAdditionalData(data, tempPath.path())) {
		tempPath.removeRecursively();

		return OS_FAILED;
	}

	tempPath.removeRecursively();

	return OS_OK;
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

	if (!m_objectTypeIdsAttrPtr.IsValid() || !m_objectPresistencesCompPtr.IsValid()){
		return OS_FAILED;
	}

	QDir tempPath = QDir::temp();
	QString uuid = QUuid::createUuid().toString();

	if (!tempPath.mkpath(uuid)){
		return OS_FAILED;
	}

	tempPath.cd(uuid);

	QString contentsFileName = tempPath.path() + QDir::separator() + "Contents.xml";
	ifile::CCompactXmlFileWriteArchive xmlArchive(contentsFileName);

	imtbase::ICollectionInfo::Ids ids = documentPtr->GetElementIds();

	QVector<BundleElementInfo> contentMetaInfo;
	
	for (const imtbase::ICollectionInfo::Id& objectId: ids){
		const istd::IChangeable *objectPtr = documentPtr->GetObjectPtr(objectId);
		if (objectPtr == nullptr){
			SendErrorMessage(0, QString("No object with the ID: '%1' was found").arg(qPrintable(objectId)));

			tempPath.removeRecursively();

			return OS_FAILED;
		}

		QByteArray typeId = documentPtr->GetElementInfo(objectId, imtbase::IObjectCollection::EIT_TYPE_ID).toByteArray();
		QString name = documentPtr->GetElementInfo(objectId, imtbase::IObjectCollection::EIT_NAME).toByteArray();

		const ifile::IFilePersistence* persistencePtr = GetFilePersistenceForTypeId(typeId);
		if (persistencePtr == nullptr){
			continue;

			SendErrorMessage(0, QString("No persistence was registered for type-ID: '%1'").arg(qPrintable(typeId)));

			tempPath.removeRecursively();

			return OS_FAILED;
		}

		QStringList extensions;
		persistencePtr->GetFileExtensions(extensions);

		QString objectFileName(name + "_" + objectId);
		if (extensions.count() > 0){
			objectFileName.append(QString(".%1").arg(extensions[0]));
		}
		
		QString objectFilePath = tempPath.path() + QDir::separator() + objectFileName;

		if (persistencePtr->SaveToFile(*objectPtr, objectFilePath) != OS_OK){
			continue;

			SendErrorMessage(0, QString("Object could not be saved to: '%1'").arg(objectFilePath));

			tempPath.removeRecursively();

			return OS_FAILED;
		}

		BundleElementInfo elementInfo;
		
		elementInfo.fileName = objectFileName;
		elementInfo.id = objectId;
		elementInfo.typeId = typeId;
		elementInfo.name = documentPtr->GetElementInfo(objectId, imtbase::IObjectCollection::EIT_NAME).toString();
		elementInfo.description = documentPtr->GetElementInfo(objectId, imtbase::IObjectCollection::EIT_DESCRIPTION).toString();

		contentMetaInfo.append(elementInfo);
	}

	bool bundleInfoWritten = SerializeBundleMetaInfo(contentMetaInfo, xmlArchive);

	xmlArchive.Flush();

	bundleInfoWritten &= SaveAdditionalData(data, tempPath.path());

	if (bundleInfoWritten){
		if (m_fileCompressionCompPtr.IsValid()){
			bool isCompressOk = m_fileCompressionCompPtr->CompressFolder(tempPath.path(), filePath, true);
		
			if (isCompressOk){
				tempPath.removeRecursively();

				return OS_OK;
			}
		}
	}

	tempPath.removeRecursively();

	return OS_FAILED;
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


bool CCompositeObjectPersistenceComp::LoadAdditionalData(istd::IChangeable& /*data*/, const QString& /*path*/) const
{
	return true;
}


bool CCompositeObjectPersistenceComp::SaveAdditionalData(const istd::IChangeable& /*data*/, const QString& /*path*/) const
{
	return true;
}


} // namespace imtbase


