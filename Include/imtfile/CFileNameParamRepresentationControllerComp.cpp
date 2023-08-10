#include <imtfile/CFileNameParamRepresentationControllerComp.h>


// ACF includes
#include <ifile/IFileNameParam.h>


namespace imtfile
{


// public methods

// reimplemented (IRepresentationController)

bool CFileNameParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const ifile::IFileNameParam* fileNameParamPtr = dynamic_cast<const ifile::IFileNameParam*>(&dataModel);
	if (fileNameParamPtr != nullptr){
		return true;
	}

	return false;
}


bool CFileNameParamRepresentationControllerComp::GetRepresentationFromDataModel(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	const ifile::IFileNameParam* fileNameParamPtr = dynamic_cast<const ifile::IFileNameParam*>(&dataModel);
	if (fileNameParamPtr != nullptr){
		representation.SetData("Id", *m_paramIdAttrPtr);

		if (m_paramNameAttrPtr.IsValid()){
			representation.SetData("Name", *m_paramNameAttrPtr);
		}

		QString path = fileNameParamPtr->GetPath();
		representation.SetData("Path", path);

		if (m_qmlPathAttrPtr.IsValid()){
			representation.SetData("Source", *m_qmlPathAttrPtr);
		}

		return true;
	}

	return false;
}


bool CFileNameParamRepresentationControllerComp::GetDataModelFromRepresentation(
			const imtbase::CTreeItemModel& representation,
			istd::IChangeable& dataModel) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	ifile::IFileNameParam* fileNameParamPtr = dynamic_cast<ifile::IFileNameParam*>(&dataModel);
	if (fileNameParamPtr != nullptr){
		QString path = representation.GetData("Path").toString();
		fileNameParamPtr->SetPath(path);

		return true;
	}

	return false;
}


} // namespace imtfile


