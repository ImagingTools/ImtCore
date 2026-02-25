// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CGqlCollectionImportControllerComp.h>


namespace imtservergql
{


// protected methods

// reimplemented (sdl::imtbase::CollectionImport::CGraphQlHandlerCompBase)

collectionImport::CSessionStatus CGqlCollectionImportControllerComp::OnBeginCollectionImportSession(
	const collectionImport::CBeginCollectionImportSessionGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& errorMessage) const
{
	collectionImport::CSessionStatus retVal;

	Q_ASSERT(m_collectionImportControllerCompPtr.IsValid());
	if (!m_collectionImportControllerCompPtr.IsValid()){
		errorMessage = tr("Collection import controller unavailable");

		return retVal;
	}

	retVal.Version_1_0.emplace();
	retVal.Version_1_0->status = collectionImport::Status::Failed;

	imtservergql::ICollectionImportController::SessionInfo sessionInfo;
	if (FillSessionInfo(request, sessionInfo, errorMessage)){
		if (m_collectionImportControllerCompPtr->BeginCollectionImportSession(sessionInfo, errorMessage)){
			retVal.Version_1_0->status = collectionImport::Status::Success;
		}
	}

	return retVal;
}


collectionImport::CSessionStatus CGqlCollectionImportControllerComp::OnCancelCollectionImportSession(
	const collectionImport::CCancelCollectionImportSessionGqlRequest& request,
	const::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& errorMessage) const
{
	collectionImport::CSessionStatus retVal;

	Q_ASSERT(m_collectionImportControllerCompPtr.IsValid());
	if (!m_collectionImportControllerCompPtr.IsValid()){
		errorMessage = tr("Collection import controller unavailable");

		return retVal;
	}

	retVal.Version_1_0.emplace();
	retVal.Version_1_0->status = collectionImport::Status::Failed;

	QByteArray sessionId = GetCancelSessionId(request);
	if (!sessionId.isEmpty()){
		if (m_collectionImportControllerCompPtr->CancelCollectionImportSession(sessionId, errorMessage)){
			retVal.Version_1_0->status = collectionImport::Status::Success;
		}
	}
	else{
		errorMessage = tr("Invalid import request");
	}

	return retVal;
}


} // namespace imtservergql


