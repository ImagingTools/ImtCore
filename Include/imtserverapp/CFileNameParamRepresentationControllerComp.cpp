// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CFileNameParamRepresentationControllerComp.h>


// ACF includes
#include <ifile/IFileNameParam.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CFileNameParam::V1_0>)

QByteArray CFileNameParamRepresentationControllerComp::GetTypeId() const
{
	return QByteArray("FileNameParam");
}


bool CFileNameParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const ifile::IFileNameParam* fileNameParamPtr = dynamic_cast<const ifile::IFileNameParam*>(&dataModel);

	return fileNameParamPtr != nullptr;
}


bool CFileNameParamRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::imtbase::ImtBaseTypes::CFileNameParam::V1_0& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const ifile::IFileNameParam* fileNameParamPtr = dynamic_cast<const ifile::IFileNameParam*>(&dataModel);
	Q_ASSERT(fileNameParamPtr != nullptr);
	if (fileNameParamPtr == nullptr){
		return false;
	}

	QString path = fileNameParamPtr->GetPath();
	sdlRepresentation.path = path;

	int type = fileNameParamPtr->GetPathType();
	sdlRepresentation.pathType = type;

	return true;
}


bool CFileNameParamRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::imtbase::ImtBaseTypes::CFileNameParam::V1_0& sdlRepresentation) const
{
	ifile::IFileNameParam* fileNameParamPtr = dynamic_cast<ifile::IFileNameParam*>(&dataModel);
	Q_ASSERT(fileNameParamPtr != nullptr);
	if (fileNameParamPtr == nullptr){
		return false;
	}

	if (sdlRepresentation.path){
		fileNameParamPtr->SetPath(*sdlRepresentation.path);
	}

	return true;
}


} // namespace imtserverapp


