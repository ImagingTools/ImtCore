// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ifile/ITempFileManager.h>

// ImtCore includes
#include <imtservergql/ICollectionImportController.h>
#include <imthype/IJobQueueManager.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/CollectionImport_fwd.h>


namespace imtservergql
{


namespace collectionImport = sdl::V1_0::imtbase;


class CGqlCollectionImportControllerComp: public collectionImport::CCollectionImportGqlHandlerCompBase
{
	Q_DECLARE_TR_FUNCTIONS(CGqlCollectionImportControllerComp)
public:
	typedef collectionImport::CCollectionImportGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlCollectionImportControllerComp);
		I_ASSIGN(m_collectionImportControllerCompPtr, "CollectionImportController", "Collection import controller", false, "CollectionImportController");
	I_END_COMPONENT;

	template<typename GqlRequest>
	static bool FillSessionInfo(
		const GqlRequest& request,
		imtservergql::ICollectionImportController::SessionInfo& sessionInfo,
		QString& errorMessage);

	template<typename GqlRequest>
	static QByteArray GetCancelSessionId(const GqlRequest& request);

protected:
	// reimplemented (sdl::V1_0::imtbase::CCollectionImportGqlHandlerCompBase)
	virtual collectionImport::CSessionStatus OnBeginCollectionImportSession(
		const collectionImport::CBeginCollectionImportSessionGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual collectionImport::CSessionStatus OnCancelCollectionImportSession(
		const collectionImport::CCancelCollectionImportSessionGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;

private:
	I_REF(imtservergql::ICollectionImportController, m_collectionImportControllerCompPtr);
};


template<typename GqlRequest>
bool CGqlCollectionImportControllerComp::FillSessionInfo(
	const GqlRequest& request,
	imtservergql::ICollectionImportController::SessionInfo& sessionInfo,
	QString& errorMessage)
{
	auto sdlImportParams = request.GetRequestedArguments().input;
	auto sdlImportParamsV1_0 = sdlImportParams;

	if (sdlImportParamsV1_0.sessionId && sdlImportParamsV1_0.collectionId && sdlImportParamsV1_0.fileList && sdlImportParamsV1_0.fileList->count() > 0){
		sessionInfo.sessionId = *sdlImportParamsV1_0.sessionId;
		sessionInfo.collectionId = *sdlImportParamsV1_0.collectionId;

		for (const auto& fileInfo : *sdlImportParamsV1_0.fileList){
			if (!fileInfo.HasValue()){
				continue;
			}
			if (fileInfo->fileId && fileInfo->fileName && fileInfo->fileSize && fileInfo->objectTypeId){
				sessionInfo.files.emplace_back();
				sessionInfo.files.back().id = *fileInfo->fileId;
				sessionInfo.files.back().name = *fileInfo->fileName;
				sessionInfo.files.back().size = *fileInfo->fileSize;
				sessionInfo.files.back().objectTypeId = *fileInfo->objectTypeId;
			}
			else{
				errorMessage = tr("Invalid file info in the request");

				return false;
			}
		}

		return true;
	}
	else{
		errorMessage = tr("Invalid import request");
	}

	return false;
}


template<typename GqlRequest>
QByteArray CGqlCollectionImportControllerComp::GetCancelSessionId(
	const GqlRequest& request)
{
	auto sdlSessionId = request.GetRequestedArguments().input;
	if (sdlSessionId.sessionId){
		QByteArray sessionId = *sdlSessionId.sessionId;

		return sessionId;
	}

	return QByteArray();
}


} // namespace imtservergql


