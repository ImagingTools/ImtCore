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
	int index = m_collectionIdsAttrPtr.FindValue(typeId);
	Q_ASSERT(index >= 0);

	return m_objectCollectionsCompPtr[index];
}


imtbase::IDocumentChangeGenerator* CDocumentRevisionControllerComp::FindDocumentChangeGenerator(const QByteArray& typeId) const
{
	int index = m_collectionIdsAttrPtr.FindValue(typeId);
	if (index < 0 || index >= m_documentChangeGeneratorsCompPtr.GetCount() ){
		return nullptr;
	}

	return m_documentChangeGeneratorsCompPtr[index];
}


// reimplemented (sdl::imtbase::DocumentRevision::V1_0::CGraphQlHandlerCompBase)

sdl::imtbase::DocumentRevision::CRevisionInfoList::V1_0 CDocumentRevisionControllerComp::OnGetRevisionInfoList(
	const sdl::imtbase::DocumentRevision::V1_0::CGetRevisionInfoListGqlRequest& getRevisionInfoListRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	sdl::imtbase::DocumentRevision::CRevisionInfoList::V1_0 response;

	QByteArray documentId = *getRevisionInfoListRequest.GetRequestedArguments().input.DocumentId;
	QByteArray collectionId = *getRevisionInfoListRequest.GetRequestedArguments().input.CollectionId;

	if (!IsCollectionSupported(collectionId)){
		return response;
	}

	const imtbase::IObjectCollection* objectCollectionPtr = FindObjectCollection(collectionId);
	Q_ASSERT(objectCollectionPtr != nullptr);

	response.DocumentId = std::make_optional<QByteArray>(documentId);

	const imtbase::IRevisionController* revisionControllerPtr = objectCollectionPtr->GetRevisionController();
	if (revisionControllerPtr == nullptr){
		errorMessage = QString("Unable to get revision list for document '%1'. Error: Revision controller is invalid").arg(qPrintable(documentId));
		return response;
	}

	imtbase::IDocumentChangeGenerator* documentChangeGeneratorPtr = FindDocumentChangeGenerator(collectionId);

	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}

	imtbase::IRevisionController::RevisionInfoList revisionInfoList = revisionControllerPtr->GetRevisionInfoList(*objectCollectionPtr, documentId);
	QList<sdl::imtbase::DocumentRevision::CRevisionItem::V1_0> revisionItemList;

	for (const imtbase::IRevisionController::RevisionInfo& revisionInfo : revisionInfoList){
		sdl::imtbase::DocumentRevision::CRevisionItem::V1_0 revisionItem;

		if (revisionInfo.isRevisionAvailable){
			response.ActiveRevision = std::make_optional<int>(revisionInfo.revision);
		}

		revisionItem.Revision = std::make_optional<int>(revisionInfo.revision);
		revisionItem.User = std::make_optional<QString>(revisionInfo.user);
		revisionItem.IsActive = std::make_optional<bool>(revisionInfo.isRevisionAvailable);
		revisionItem.Timestamp = std::make_optional<QString>(revisionInfo.timestamp.toLocalTime().toString("dd.MM.yyyy hh:mm:ss"));

		if (documentChangeGeneratorPtr != nullptr){
			imtbase::CObjectCollection changeCollection;

			typedef istd::TSingleFactory<istd::IChangeable, imtbase::COperationDescription> FactoryOperationDescriptionImpl;
			changeCollection.RegisterFactory<FactoryOperationDescriptionImpl>("OperationInfo");

			iser::CJsonMemReadArchive archive(revisionInfo.comment.toUtf8());
			if (changeCollection.Serialize(archive)){
				QString operationDescription = documentChangeGeneratorPtr->GetOperationDescription(changeCollection, languageId);
				revisionItem.Description = std::make_optional<QString>(operationDescription);
			}
		}

		revisionItemList << revisionItem;
	}

	response.Revisions = std::make_optional<QList<sdl::imtbase::DocumentRevision::CRevisionItem::V1_0>>(revisionItemList);

	return response;
}


sdl::imtbase::DocumentRevision::CBackupRevisionResponse::V1_0 CDocumentRevisionControllerComp::OnBackupRevision(
	const sdl::imtbase::DocumentRevision::V1_0::CBackupRevisionGqlRequest& /*backupRevisionRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return sdl::imtbase::DocumentRevision::CBackupRevisionResponse::V1_0();
}


sdl::imtbase::DocumentRevision::CRestoreRevisionResponse::V1_0 CDocumentRevisionControllerComp::OnRestoreRevision(
	const sdl::imtbase::DocumentRevision::V1_0::CRestoreRevisionGqlRequest& restoreRevisionRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& errorMessage) const
{
	sdl::imtbase::DocumentRevision::CRestoreRevisionResponse::V1_0 response;

	QByteArray collectionId = *restoreRevisionRequest.GetRequestedArguments().input.CollectionId;
	QByteArray documentId = *restoreRevisionRequest.GetRequestedArguments().input.ObjectId;
	int revisionNumber = *restoreRevisionRequest.GetRequestedArguments().input.Revision;

	if (!IsCollectionSupported(collectionId)){
		return response;
	}

	imtbase::IObjectCollection* objectCollectionPtr = FindObjectCollection(collectionId);
	Q_ASSERT(objectCollectionPtr != nullptr);

	const imtbase::IRevisionController* revisionControllerPtr = objectCollectionPtr->GetRevisionController();
	if (revisionControllerPtr == nullptr){
		errorMessage = QString("Unable to set revision '%1' for document '%2'. Error: Revision controller is invalid").arg(revisionNumber).arg(qPrintable(documentId));
		return response;
	}

	bool ok = revisionControllerPtr->RestoreRevision(*objectCollectionPtr, documentId, revisionNumber);
	if (!ok){
		errorMessage = QString("Unable to set revision '%1' for document '%2'. Error: Restoring object failed").arg(revisionNumber).arg(qPrintable(documentId));
		return response;
	}

	response.Result = std::make_optional<bool>(ok);

	return response;
}


sdl::imtbase::DocumentRevision::CExportRevisionResponse::V1_0 CDocumentRevisionControllerComp::OnExportRevision(
	const sdl::imtbase::DocumentRevision::V1_0::CExportRevisionGqlRequest& /*exportRevisionRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return sdl::imtbase::DocumentRevision::CExportRevisionResponse::V1_0();
}


sdl::imtbase::DocumentRevision::CDeleteRevisionResponse::V1_0 CDocumentRevisionControllerComp::OnDeleteRevision(
	const sdl::imtbase::DocumentRevision::V1_0::CDeleteRevisionGqlRequest& deleteRevisionRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& errorMessage) const
{
	sdl::imtbase::DocumentRevision::CDeleteRevisionResponse::V1_0 response;

	const imtgql::IGqlContext* gqlContextPtr = deleteRevisionRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			if (!userInfoPtr->IsAdmin()){
				errorMessage = QString("Unable to delete revision. Error: Invalid permission for user '%1'").arg(userInfoPtr->GetName());

				return response;
			}
		}
	}

	QByteArray documentId = *deleteRevisionRequest.GetRequestedArguments().input.ObjectId;
	int revisionNumber = *deleteRevisionRequest.GetRequestedArguments().input.Revision;
	QByteArray collectionId = *deleteRevisionRequest.GetRequestedArguments().input.CollectionId;

	if (!IsCollectionSupported(collectionId)){
		return response;
	}

	imtbase::IObjectCollection* objectCollectionPtr = FindObjectCollection(collectionId);
	Q_ASSERT(objectCollectionPtr != nullptr);

	const imtbase::IRevisionController* revisionControllerPtr = objectCollectionPtr->GetRevisionController();
	if (revisionControllerPtr == nullptr){
		errorMessage = QString("Unable to delete revision '%1' for document '%2'. Error: Revision controller is invalid").arg(revisionNumber).arg(qPrintable(documentId));
		return response;
	}

	bool ok = revisionControllerPtr->DeleteRevision(*objectCollectionPtr, documentId, revisionNumber);

	response.Result = std::make_optional<bool>(ok);

	return response;
}


// reimplemented (imtgql::IGqlRequestHandler)

bool CDocumentRevisionControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
	if (!isSupported){
		return false;
	}

	const imtgql::CGqlObject* inputObjectPtr = gqlRequest.GetParamObject("input");
	if (inputObjectPtr == nullptr){
		return false;
	}

	QByteArray collectionId = inputObjectPtr->GetFieldArgumentValue("CollectionId").toByteArray();
	if (collectionId.isEmpty()){
		return false;
	}

	return IsCollectionSupported(collectionId);
}


} // namespace imtservergql


