#include <imtbase/CCompositeObjectPersistenceComp.h>


// ImtCore includes
#include <imtbase/IObjectCollection.h>


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
	if (documentPtr == NULL){
		return OS_FAILED;
	}

	return OS_FAILED;
}


int CCompositeObjectPersistenceComp::SaveToFile(
			const istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* /*progressManagerPtr*/) const
{
	const imtbase::IObjectCollection* documentPtr = dynamic_cast<const imtbase::IObjectCollection*>(&data);
	if (documentPtr == NULL){
		return OS_FAILED;
	}

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


} // namespace imtbase


