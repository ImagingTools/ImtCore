// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CCollectionImportParamRepresentationControllerComp.h>


// ImtCore includes
#include <imtcol/ICollectionImportParam.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CCollectionImportParam::V1_0>)

QByteArray CCollectionImportParamRepresentationControllerComp::GetTypeId() const
{
	return sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0::ParamTypeIdsFields::CollectionImportParam.toUtf8();
}


bool CCollectionImportParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtcol::ICollectionImportParam* collectionImportParamPtr = dynamic_cast<const imtcol::ICollectionImportParam*>(&dataModel);

	return collectionImportParamPtr != nullptr;
}


bool CCollectionImportParamRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::imtbase::ImtBaseTypes::CCollectionImportParam::V1_0& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtcol::ICollectionImportParam* collectionImportParamPtr = dynamic_cast<const imtcol::ICollectionImportParam*>(&dataModel);
	Q_ASSERT(collectionImportParamPtr != nullptr);
	if (collectionImportParamPtr == nullptr){
		return false;
	}

	sdlRepresentation.collectionId = collectionImportParamPtr->GetCollectionId();

	imtsdl::TElementList<sdl::imtbase::ImtBaseTypes::CFileImportInfo::V1_0> sdlFileImportInfos;
	int count = collectionImportParamPtr->GetFileCount();
	for (int i = 0; i < count; ++i){
		imtcol::ICollectionImportParam::FileImportInfo* fileImportInfoPtr = collectionImportParamPtr->GetFileImportInfo(i);
		if (fileImportInfoPtr != nullptr){
			sdl::imtbase::ImtBaseTypes::CFileImportInfo::V1_0 sdlFileImportInfo;
			sdlFileImportInfo.filePath = fileImportInfoPtr->filePath;
			sdlFileImportInfo.objectTypeId = fileImportInfoPtr->objectTypeId;
			sdlFileImportInfo.proposedId = fileImportInfoPtr->proposedId;
			sdlFileImportInfos << sdlFileImportInfo;
		}
	}

	sdlRepresentation.fileImportInfos = sdlFileImportInfos;

	return true;
}


bool CCollectionImportParamRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::imtbase::ImtBaseTypes::CCollectionImportParam::V1_0& sdlRepresentation) const
{
	imtcol::ICollectionImportParam* collectionImportParamPtr = dynamic_cast<imtcol::ICollectionImportParam*>(&dataModel);
	Q_ASSERT(collectionImportParamPtr != nullptr);
	if (collectionImportParamPtr == nullptr){
		return false;
	}

	if (sdlRepresentation.collectionId){
		collectionImportParamPtr->SetCollectionId(*sdlRepresentation.collectionId);
	}

	if (sdlRepresentation.fileImportInfos){
		const imtsdl::TElementList<sdl::imtbase::ImtBaseTypes::CFileImportInfo::V1_0>& sdlFileImportInfos = *sdlRepresentation.fileImportInfos;
		for (int i = 0; i < sdlFileImportInfos.size(); ++i){
			const sdl::imtbase::ImtBaseTypes::CFileImportInfo::V1_0& sdlFileImportInfo = sdlFileImportInfos[i];
			imtcol::ICollectionImportParam::FileImportInfo fileImportInfo;
			if (sdlFileImportInfo.filePath){
				fileImportInfo.filePath = *sdlFileImportInfo.filePath;
			}
			if (sdlFileImportInfo.objectTypeId){
				fileImportInfo.objectTypeId = *sdlFileImportInfo.objectTypeId;
			}
			if (sdlFileImportInfo.proposedId){
				fileImportInfo.proposedId = *sdlFileImportInfo.proposedId;
			}
			collectionImportParamPtr->InsertFileImportInfo(fileImportInfo, i);
		}
	}

	return true;
}


} // namespace imtserverapp


