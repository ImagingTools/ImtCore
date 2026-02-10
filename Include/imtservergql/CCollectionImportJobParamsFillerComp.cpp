// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CCollectionImportJobParamsFillerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IParamsManager.h>
#include <iprm/IIdParam.h>
#include <ifile/IFileNameParam.h>


namespace imtservergql
{


// public methods

// reimplemented (imtservergql::ICollectionImportJobParamsFiller)

bool CCollectionImportJobParamsFillerComp::FillCollectionImportJobParams(const imtservergql::ICollectionImportController::SessionInfo& sessionInfo, iprm::IParamsSet& jobParams) const
{
	iprm::TEditableParamsPtr<iprm::IIdParam> collectionIdPtr(&jobParams, "CollectionId");
	iprm::TEditableParamsPtr<iprm::IParamsManager> fileParamsListPtr(&jobParams, "FileParamsList");
	if (!collectionIdPtr.IsValid() || !fileParamsListPtr.IsValid()){
		return false;
	}

	collectionIdPtr->SetId(sessionInfo.collectionId);

	for (const ICollectionImportController::FileInfo& file : sessionInfo.files){
		int index = fileParamsListPtr->InsertParamsSet();
		if (index < 0){
			return false;
		}

		iprm::IParamsSet* fileParamsPtr = fileParamsListPtr->GetParamsSet(index);
		Q_ASSERT(fileParamsPtr != nullptr);

		iprm::TEditableParamsPtr<ifile::IFileNameParam> filePathPtr(fileParamsPtr, "FilePath");
		iprm::TEditableParamsPtr<iprm::IIdParam> objectTypeIdPtr(fileParamsPtr, "ObjectTypeId");
		iprm::TEditableParamsPtr<iprm::IIdParam> proposedIdPtr(fileParamsPtr, "ProposedId");

		Q_ASSERT(
			filePathPtr.IsValid() &&
			objectTypeIdPtr.IsValid() &&
			proposedIdPtr.IsValid());

		filePathPtr->SetPath(file.path);
		objectTypeIdPtr->SetId(file.objectTypeId);
		proposedIdPtr->SetId(file.id);
	}

	return true;
}


} // namespace imtservergql


