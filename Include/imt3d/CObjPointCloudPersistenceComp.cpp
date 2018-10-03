#include <imt3d/CObjPointCloudPersistenceComp.h>


// Qt includes
#include <QtCore/QFile>

// ImtCore includes
#include <imt3d/CPointCloud3d.h>


namespace imt3d
{


// reimplemented (ifile::IFilePersistence)

bool CObjPointCloudPersistenceComp::IsOperationSupported(
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
		const CPointCloud3d* documentPtr = dynamic_cast<const CPointCloud3d*>(dataObjectPtr);
		if (documentPtr == NULL){
			return false;
		}
	}

	return true;
}


int CObjPointCloudPersistenceComp::LoadFromFile(
			istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* /*progressManagerPtr*/) const
{
	CPointCloud3d* documentPtr = dynamic_cast<CPointCloud3d*>(&data);
	if (documentPtr == NULL){
		return OS_FAILED;
	}

	return OS_OK;
}


int CObjPointCloudPersistenceComp::SaveToFile(const istd::IChangeable& data, const QString& filePath, ibase::IProgressManager* /*progressManagerPtr*/) const
{
	const CPointCloud3d* documentPtr = dynamic_cast<const CPointCloud3d*>(&data);
	if (documentPtr == NULL){
		return OS_FAILED;
	}

	return OS_OK;
}


// reimplemented (ifile::IFileTypeInfo)

bool CObjPointCloudPersistenceComp::GetFileExtensions(QStringList& result, const istd::IChangeable* /*dataObjectPtr*/, int /*flags*/, bool doAppend) const
{
	if (!doAppend){
		result.clear();
	}

	result.push_back("obj");

	return true;
}


QString CObjPointCloudPersistenceComp::GetTypeDescription(const QString* /*extensionPtr*/) const
{
	return "3D-object files";
}


} // namespace imt3d


