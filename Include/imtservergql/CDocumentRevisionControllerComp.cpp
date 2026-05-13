// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CDocumentRevisionControllerComp.h>


// ACF includes
#include <istd/TSingleFactory.h>
#include <iser/CJsonMemReadArchive.h>

// ImtCore includes
#include <imtbase/IRevisionController.h>
#include <imtbase/COperationDescription.h>
#include <imtbase/CObjectCollection.h>


namespace imtservergql
{


// protected methods

bool CDocumentRevisionControllerComp::IsCollectionSupported(const QByteArray& typeId) const
{
	return m_collectionIdsAttrPtr.FindValue(typeId) >= 0;
}


imtbase::IObjectCollection* CDocumentRevisionControllerComp::FindObjectCollection(const QByteArray& typeId) const
{
	if (!m_collectionIdsAttrPtr.IsValid()){
		return nullptr;
	}

	const int indexOfCollection = m_collectionIdsAttrPtr.FindValue(typeId);
	if (indexOfCollection < 0){
		return nullptr;
	}

	const int collectionsCount = m_objectCollectionsCompPtr.GetCount();
	if (indexOfCollection >= collectionsCount){
		SendCriticalMessage(0, QString("Collection with id '%1' doesn't have a collection controller").arg(m_collectionIdsAttrPtr[indexOfCollection]));

		return nullptr;
	}
	
	return m_objectCollectionsCompPtr[indexOfCollection];
}


imtbase::IDocumentChangeGenerator* CDocumentRevisionControllerComp::FindDocumentChangeGenerator(const QByteArray& typeId) const
{
	int index = m_collectionIdsAttrPtr.FindValue(typeId);
	if (index < 0 || index >= m_documentChangeGeneratorsCompPtr.GetCount() ){
		return nullptr;
	}

	return m_documentChangeGeneratorsCompPtr[index];
}


// reimplemented (sdl::imtbase::DocumentRevision::CGraphQlHandlerCompBase)

sdl::imtbase::DocumentRevision::CRevisionInfoList CDocumentRevisionControllerComp::OnGetRevisionInfoList(
	const sdl::imtbase::DocumentRevision::CGetRevisionInfoListGqlRequest& getRevisionInfoListRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	sdl::imtbase::DocumentRevision::CRevisionInfoList::V1_0 response;
	
	sdl::imtbase::DocumentRevision::GetRevisionInfoListRequestArguments arguments = getRevisionInfoListRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		errorMessage = QString("Unable to get revision list. Error: Request invalid");
		return sdl::imtbase::DocumentRevision::CRevisionInfoList();
	}
	
	QByteArray documentId;
	if (arguments.input.Version_1_0->documentId){
		documentId = *arguments.input.Version_1_0->documentId;
	}

	QByteArray collectionId;
	if (arguments.input.Version_1_0->collectionId){
		collectionId = *arguments.input.Version_1_0->collectionId;
	}

	if (!IsCollectionSupported(collectionId)){
		return sdl::imtbase::DocumentRevision::CRevisionInfoList();
	}

	const imtbase::IObjectCollection* objectCollectionPtr = FindObjectCollection(collectionId);
	Q_ASSERT(objectCollectionPtr != nullptr);

	response.documentId = (documentId);

	const imtbase::IRevisionController* revisionControllerPtr = objectCollectionPtr->GetRevisionController();
	if (revisionControllerPtr == nullptr){
		errorMessage = QString("Unable to get revision list for document '%1'. Error: revision controller is invalid").arg(qPrintable(documentId));
		return sdl::imtbase::DocumentRevision::CRevisionInfoList();
	}

	imtbase::IDocumentChangeGenerator* documentChangeGeneratorPtr = FindDocumentChangeGenerator(collectionId);

	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}

	imtbase::IRevisionController::RevisionInfoList revisionInfoList = revisionControllerPtr->GetRevisionInfoList(*objectCollectionPtr, documentId);
	imtsdl::TElementList<sdl::imtbase::DocumentRevision::CRevisionItem::V1_0> revisionItemList;

	for (const imtbase::IRevisionController::RevisionInfo& revisionInfo : revisionInfoList){
		sdl::imtbase::DocumentRevision::CRevisionItem::V1_0 revisionItem;

		if (revisionInfo.isRevisionAvailable){
			response.activeRevision = std::move(revisionInfo.revision);
		}

		revisionItem.revision = std::move(revisionInfo.revision);
		revisionItem.user = std::move(revisionInfo.user);
		revisionItem.isActive = (revisionInfo.isRevisionAvailable);
		revisionItem.timestamp = std::move(revisionInfo.timestamp.toLocalTime().toString("dd.MM.yyyy hh:mm:ss"));

		if (documentChangeGeneratorPtr != nullptr){
			imtbase::CObjectCollection changeCollection;

			typedef istd::TSingleFactory<istd::IChangeable, imtbase::COperationDescription> FactoryOperationDescriptionImpl;
			changeCollection.RegisterFactory<FactoryOperationDescriptionImpl>("OperationInfo");

			iser::CJsonMemReadArchive archive(revisionInfo.comment.toUtf8());
			if (changeCollection.Serialize(archive)){
				QString operationDescription = documentChangeGeneratorPtr->GetOperationDescription(changeCollection, languageId);
				revisionItem.description = std::move(operationDescription);
			}
		}

		revisionItemList << revisionItem;
	}

	response.revisions = std::move(revisionItemList);

	sdl::imtbase::DocumentRevision::CRevisionInfoList retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::DocumentRevision::CBackupRevisionResponse CDocumentRevisionControllerComp::OnBackupRevision(
	const sdl::imtbase::DocumentRevision::CBackupRevisionGqlRequest& /*backupRevisionRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return sdl::imtbase::DocumentRevision::CBackupRevisionResponse();
}


sdl::imtbase::DocumentRevision::CRestoreRevisionResponse CDocumentRevisionControllerComp::OnRestoreRevision(
	const sdl::imtbase::DocumentRevision::CRestoreRevisionGqlRequest& restoreRevisionRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& errorMessage) const
{
	sdl::imtbase::DocumentRevision::CRestoreRevisionResponse::V1_0 response;
	
	sdl::imtbase::DocumentRevision::RestoreRevisionRequestArguments arguments = restoreRevisionRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		errorMessage = QString("Unable to restore revision. Error: Request invalid");
		return sdl::imtbase::DocumentRevision::CRestoreRevisionResponse();
	}
	
	QByteArray documentId;
	if (arguments.input.Version_1_0->objectId){
		documentId = *arguments.input.Version_1_0->objectId;
	}
	
	int revisionNumber = -1;
	if (arguments.input.Version_1_0->revision){
		revisionNumber = *arguments.input.Version_1_0->revision;
	}
	
	QByteArray collectionId;
	if (arguments.input.Version_1_0->collectionId){
		collectionId = *arguments.input.Version_1_0->collectionId;
	}
	
	if (!IsCollectionSupported(collectionId)){
		return sdl::imtbase::DocumentRevision::CRestoreRevisionResponse();
	}

	imtbase::IObjectCollection* objectCollectionPtr = FindObjectCollection(collectionId);
	Q_ASSERT(objectCollectionPtr != nullptr);

	const imtbase::IRevisionController* revisionControllerPtr = objectCollectionPtr->GetRevisionController();
	if (revisionControllerPtr == nullptr){
		errorMessage = QString("Unable to set revision '%1' for document '%2'. Error: revision controller is invalid").arg(revisionNumber).arg(qPrintable(documentId));
		return sdl::imtbase::DocumentRevision::CRestoreRevisionResponse();
	}

	bool ok = revisionControllerPtr->RestoreRevision(*objectCollectionPtr, documentId, revisionNumber);
	if (!ok){
		errorMessage = QString("Unable to set revision '%1' for document '%2'. Error: Restoring object failed").arg(revisionNumber).arg(qPrintable(documentId));
		return sdl::imtbase::DocumentRevision::CRestoreRevisionResponse();
	}

	response.result = (ok);

	sdl::imtbase::DocumentRevision::CRestoreRevisionResponse retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtbase::DocumentRevision::CExportRevisionResponse CDocumentRevisionControllerComp::OnExportRevision(
	const sdl::imtbase::DocumentRevision::CExportRevisionGqlRequest& /*exportRevisionRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return sdl::imtbase::DocumentRevision::CExportRevisionResponse();
}


sdl::imtbase::DocumentRevision::CDeleteRevisionResponse CDocumentRevisionControllerComp::OnDeleteRevision(
	const sdl::imtbase::DocumentRevision::CDeleteRevisionGqlRequest& deleteRevisionRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& errorMessage) const
{
	sdl::imtbase::DocumentRevision::CDeleteRevisionResponse::V1_0 response;

	const imtgql::IGqlContext* gqlContextPtr = deleteRevisionRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			if (!userInfoPtr->IsAdmin()){
				errorMessage = QString("Unable to delete revision. Error: Invalid permission for user '%1'").arg(userInfoPtr->GetName());

				return sdl::imtbase::DocumentRevision::CDeleteRevisionResponse();
			}
		}
	}
	
	sdl::imtbase::DocumentRevision::DeleteRevisionRequestArguments arguments = deleteRevisionRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		errorMessage = QString("Unable to delete revision. Error: Request invalid");
		return sdl::imtbase::DocumentRevision::CDeleteRevisionResponse();
	}
	
	QByteArray documentId;
	if (arguments.input.Version_1_0->objectId){
		documentId = *arguments.input.Version_1_0->objectId;
	}
	
	int revisionNumber = -1;
	if (arguments.input.Version_1_0->revision){
		revisionNumber = *arguments.input.Version_1_0->revision;
	}
	
	QByteArray collectionId;
	if (arguments.input.Version_1_0->collectionId){
		collectionId = *arguments.input.Version_1_0->collectionId;
	}

	if (!IsCollectionSupported(collectionId)){
		return sdl::imtbase::DocumentRevision::CDeleteRevisionResponse();
	}

	imtbase::IObjectCollection* objectCollectionPtr = FindObjectCollection(collectionId);
	Q_ASSERT(objectCollectionPtr != nullptr);

	const imtbase::IRevisionController* revisionControllerPtr = objectCollectionPtr->GetRevisionController();
	if (revisionControllerPtr == nullptr){
		errorMessage = QString("Unable to delete revision '%1' for document '%2'. Error: revision controller is invalid").arg(revisionNumber).arg(qPrintable(documentId));
		return sdl::imtbase::DocumentRevision::CDeleteRevisionResponse();
	}

	bool ok = revisionControllerPtr->DeleteRevision(*objectCollectionPtr, documentId, revisionNumber);

	response.result = (ok);

	sdl::imtbase::DocumentRevision::CDeleteRevisionResponse retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


// reimplemented (imtgql::IGqlRequestHandler)

bool CDocumentRevisionControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
	if (!isSupported){
		return false;
	}

	const imtgql::CGqlParamObject* inputObjectPtr = gqlRequest.GetParamObject("input");
	if (inputObjectPtr == nullptr){
		return false;
	}
	
	QByteArray collectionId = inputObjectPtr->GetParamArgumentValue(
												sdl::imtbase::DocumentRevision::CGetRevisionInfoListInput::V1_0::GetRevisionInfoListInputFields::CollectionId.toUtf8()).toByteArray();
	if (collectionId.isEmpty()){
		return false;
	}

	return IsCollectionSupported(collectionId);
}


} // namespace imtservergql


