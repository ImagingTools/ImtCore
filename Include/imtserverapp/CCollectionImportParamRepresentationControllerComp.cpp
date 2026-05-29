// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CCollectionImportParamRepresentationControllerComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings.h>


// ImtCore includes
#include <imtcol/ICollectionImportParam.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::V1_0::imtbase::CCollectionImportParam>)

QByteArray CCollectionImportParamRepresentationControllerComp::GetTypeId() const
{
	return sdl::V1_0::imtbase::CParamTypeIds::ParamTypeIdsFields::CollectionImportParam.toUtf8();
}


bool CCollectionImportParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtcol::ICollectionImportParam* collectionImportParamPtr = dynamic_cast<const imtcol::ICollectionImportParam*>(&dataModel);

	return collectionImportParamPtr != nullptr;
}


bool CCollectionImportParamRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::V1_0::imtbase::CCollectionImportParam& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtcol::ICollectionImportParam* collectionImportParamPtr = dynamic_cast<const imtcol::ICollectionImportParam*>(&dataModel);
	Q_ASSERT(collectionImportParamPtr != nullptr);
	if (collectionImportParamPtr == nullptr){
		return false;
	}

	sdlRepresentation.collectionId = collectionImportParamPtr->GetCollectionId();

	imtsdl::TElementList<sdl::V1_0::imtbase::CFileImportInfo> sdlFileImportInfos;
	int count = collectionImportParamPtr->GetFileCount();
	for (int i = 0; i < count; ++i){
		imtcol::ICollectionImportParam::FileImportInfo* fileImportInfoPtr = collectionImportParamPtr->GetFileImportInfo(i);
		if (fileImportInfoPtr != nullptr){
			sdl::V1_0::imtbase::CFileImportInfo sdlFileImportInfo;
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
			const sdl::V1_0::imtbase::CCollectionImportParam& sdlRepresentation) const
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
		const imtsdl::TElementList<sdl::V1_0::imtbase::CFileImportInfo>& sdlFileImportInfos = *sdlRepresentation.fileImportInfos;
		for (int i = 0; i < sdlFileImportInfos.size(); ++i){
			auto sdlFileImportInfo = sdlFileImportInfos[i];
			if (sdlFileImportInfo){
				imtcol::ICollectionImportParam::FileImportInfo fileImportInfo;
				if (sdlFileImportInfo->filePath){
					fileImportInfo.filePath = *sdlFileImportInfo->filePath;
				}
				if (sdlFileImportInfo->objectTypeId){
					fileImportInfo.objectTypeId = *sdlFileImportInfo->objectTypeId;
				}
				if (sdlFileImportInfo->proposedId){
					fileImportInfo.proposedId = *sdlFileImportInfo->proposedId;
				}
				collectionImportParamPtr->InsertFileImportInfo(fileImportInfo, i);
			}
		}
	}

	return true;
}


} // namespace imtserverapp


