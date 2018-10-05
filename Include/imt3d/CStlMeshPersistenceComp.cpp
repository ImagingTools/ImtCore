#include <imt3d/CStlMeshPersistenceComp.h>


// Qt includes
#include <QtCore/QFile>

// ImtCore includes
#include <imt3d/CMesh.h>


namespace imt3d
{


// reimplemented (ifile::IFilePersistence)

bool CStlMeshPersistenceComp::IsOperationSupported(
			const istd::IChangeable* dataObjectPtr,
			const QString* filePathPtr,
			int flags,
			bool /*beQuiet*/) const
{
	if ((filePathPtr != NULL) && (flags & QF_LOAD)){
		QFile stlFile(*filePathPtr);

		if (!stlFile.exists()){
			return false;
		}

		if (!stlFile.open(QIODevice::ReadOnly)){
			return false;
		}
	}

	if (dataObjectPtr != NULL){
		const CMesh* documentPtr = dynamic_cast<const CMesh*>(dataObjectPtr);
		if (documentPtr == NULL){
			return false;
		}
	}

	return true;
}


int CStlMeshPersistenceComp::LoadFromFile(
			istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* /*progressManagerPtr*/) const
{
	CMesh* documentPtr = dynamic_cast<CMesh*>(&data);
	if (documentPtr == NULL){
		return OS_FAILED;
	}

	bool loaded = documentPtr->LoadFromStlFile(filePath);
	if (!loaded){
		return OS_FAILED;
	}

	return OS_OK;
}


int CStlMeshPersistenceComp::SaveToFile(const istd::IChangeable& data, const QString& filePath, ibase::IProgressManager* /*progressManagerPtr*/) const
{
	const CMesh* documentPtr = dynamic_cast<const CMesh*>(&data);
	if (documentPtr == NULL){
		return OS_FAILED;
	}

	bool saved = documentPtr->SaveToStlFile(filePath);
	if (!saved){
		return OS_FAILED;
	}

	return OS_OK;
}


// reimplemented (ifile::IFileTypeInfo)

bool CStlMeshPersistenceComp::GetFileExtensions(QStringList& result, const istd::IChangeable* /*dataObjectPtr*/, int /*flags*/, bool doAppend) const
{
	if (!doAppend){
		result.clear();
	}

	result.push_back("stl");

	return true;
}


QString CStlMeshPersistenceComp::GetTypeDescription(const QString* /*extensionPtr*/) const
{
	return "STL files";
}


} // namespace imt3d


