#include <imtfile/CFileNameParamRepresentationControllerComp.h>


// ACF includes
#include <ifile/IFileNameParam.h>


namespace imtfile
{


// protected methods

// reimplemented (imtbase::CObjectRepresentationControllerCompBase)

bool CFileNameParamRepresentationControllerComp::GetRepresentationFromValue(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const ifile::IFileNameParam* fileNameParamPtr = dynamic_cast<const ifile::IFileNameParam*>(&dataModel);
	Q_ASSERT(fileNameParamPtr != nullptr);

	QString path = fileNameParamPtr->GetPath();
	representation.SetData("Path", path);

	return true;
}


// reimplemented (IRepresentationController)

bool CFileNameParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const ifile::IFileNameParam* fileNameParamPtr = dynamic_cast<const ifile::IFileNameParam*>(&dataModel);
	if (fileNameParamPtr != nullptr){
		return true;
	}

	return false;
}


bool CFileNameParamRepresentationControllerComp::GetDataModelFromRepresentation(
			const imtbase::CTreeItemModel& representation,
			istd::IChangeable& dataModel) const
{
	ifile::IFileNameParam* fileNameParamPtr = dynamic_cast<ifile::IFileNameParam*>(&dataModel);
	Q_ASSERT(fileNameParamPtr != nullptr);

	QString path = representation.GetData("Path").toString();
	fileNameParamPtr->SetPath(path);

	return true;
}


} // namespace imtfile


