// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CCollectionImportJobParamsFillerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtcol/ICollectionImportParam.h>
#include <imtservergql/ICollectionImportController.h>


namespace imtservergql
{


// public methods

// reimplemented (imtservergql::ICollectionImportJobParamsFiller)

bool CCollectionImportJobParamsFillerComp::FillCollectionImportJobParams(const imtservergql::ICollectionImportController::SessionInfo& sessionInfo, iprm::IParamsSet& jobParams) const
{
	iprm::TEditableParamsPtr<imtcol::ICollectionImportParam> collectionImportParamPtr(&jobParams, "CollectionImportParam");
	if (!collectionImportParamPtr.IsValid()){
		return false;
	}

	collectionImportParamPtr->SetCollectionId(sessionInfo.collectionId);

	for (const ICollectionImportController::FileInfo& file : sessionInfo.files){
		imtcol::ICollectionImportParam::FileImportInfo info;

		info.filePath = file.path;
		info.objectTypeId = file.objectTypeId;
		info.proposedId = file.id;

		collectionImportParamPtr->InsertFileImportInfo(info);
	}

	return true;
}


} // namespace imtservergql


