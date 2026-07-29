// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CDocumentRevisionControllerComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/DocumentRevision.h>


// ACF includes
#include <istd/TSingleFactory.h>
#include <iser/CJsonMemReadArchive.h>

// ImtCore includes
#include <imtbase/IRevisionController.h>
#include <imtbase/COperationDescription.h>
#include <imtbase/CObjectCollection.h>


namespace
{


/**
 * Case-insensitive "contains" over the fields a revision row actually shows,
 * so what the user types matches what the user sees. An empty filter matches
 * everything.
 */
bool MatchesRevisionFilter(const sdl::V1_0::imtbase::CRevisionItem& revisionItem, const QString& filterText)
{
	if (filterText.isEmpty()){
		return true;
	}

	if (revisionItem.user && revisionItem.user->contains(filterText, Qt::CaseInsensitive)){
		return true;
	}
	if (revisionItem.description && revisionItem.description->contains(filterText, Qt::CaseInsensitive)){
		return true;
	}
	if (revisionItem.timestamp && revisionItem.timestamp->contains(filterText, Qt::CaseInsensitive)){
		return true;
	}
	if (revisionItem.revision && QString::number(*revisionItem.revision).contains(filterText)){
		return true;
	}

	return false;
}


} // namespace


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


// reimplemented (sdl::V1_0::imtbase::CDocumentRevisionGqlHandlerCompBase)

sdl::V1_0::imtbase::CRevisionInfoList CDocumentRevisionControllerComp::OnGetRevisionInfoList(
	const sdl::V1_0::imtbase::CGetRevisionInfoListGqlRequest& getRevisionInfoListRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	sdl::V1_0::imtbase::CRevisionInfoList response;
	
	sdl::V1_0::imtbase::GetRevisionInfoListRequestArguments arguments = getRevisionInfoListRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		errorMessage = QString("Unable to get revision list. Error: Request invalid");
		return  sdl::V1_0::imtbase::CRevisionInfoList();
	}
	QByteArray documentId;
	if (arguments.input->documentId){
		documentId = *arguments.input->documentId;
	}

	QByteArray collectionId;
	if (arguments.input->collectionId){
		collectionId = *arguments.input->collectionId;
	}

	if (!IsCollectionSupported(collectionId)){
		return sdl::V1_0::imtbase::CRevisionInfoList();
	}

	const imtbase::IObjectCollection* objectCollectionPtr = FindObjectCollection(collectionId);
	Q_ASSERT(objectCollectionPtr != nullptr);

	response.documentId = (documentId);

	const imtbase::IRevisionController* revisionControllerPtr = objectCollectionPtr->GetRevisionController();
	if (revisionControllerPtr == nullptr){
		errorMessage = QString("Unable to get revision list for document '%1'. Error: revision controller is invalid").arg(qPrintable(documentId));
		return sdl::V1_0::imtbase::CRevisionInfoList();
	}

	imtbase::IDocumentChangeGenerator* documentChangeGeneratorPtr = FindDocumentChangeGenerator(collectionId);

	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}

	// Optional paging and text filtering. Without viewParams the whole list is
	// returned, which is what callers predating those fields expect.
	int offset = 0;
	int count = -1;
	QString filterText;
	if (arguments.input->viewParams){
		const auto& viewParams = *arguments.input->viewParams;
		if (viewParams.offset){
			offset = qMax(0, *viewParams.offset);
		}
		if (viewParams.count){
			count = qMax(0, *viewParams.count);
		}
		if (viewParams.filterModel){
			const auto& filterModel = *viewParams.filterModel;
			if (filterModel.textFilter && filterModel.textFilter->text){
				filterText = *filterModel.textFilter->text;
			}
		}
	}

	imtbase::IRevisionController::RevisionInfoList revisionInfoList = revisionControllerPtr->GetRevisionInfoList(*objectCollectionPtr, documentId);
	imtsdl::TElementList<sdl::V1_0::imtbase::CRevisionItem> revisionItemList;

	// The active revision and the total are properties of the whole list, not of
	// the requested page, so both are determined before paging is applied.
	int matchedCount = 0;
	for (const imtbase::IRevisionController::RevisionInfo& revisionInfo : revisionInfoList){
		sdl::V1_0::imtbase::CRevisionItem revisionItem;

		if (revisionInfo.isRevisionAvailable){
			response.activeRevision = revisionInfo.revision;
		}

		revisionItem.revision = revisionInfo.revision;
		revisionItem.user = revisionInfo.user;
		revisionItem.isActive = (revisionInfo.isRevisionAvailable);
		revisionItem.timestamp = revisionInfo.timestamp.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");

		if (documentChangeGeneratorPtr != nullptr){
			imtbase::CObjectCollection changeCollection;

			typedef istd::TSingleFactory<istd::IChangeable, imtbase::COperationDescription> FactoryOperationDescriptionImpl;
			changeCollection.RegisterFactory<FactoryOperationDescriptionImpl>("OperationInfo");

			iser::CJsonMemReadArchive archive(revisionInfo.comment.toUtf8());
			if (changeCollection.Serialize(archive)){
				QString operationDescription = documentChangeGeneratorPtr->GetOperationDescription(changeCollection, languageId);
				revisionItem.description = operationDescription;
			}
		}

		if (!MatchesRevisionFilter(revisionItem, filterText)){
			continue;
		}

		const int matchIndex = matchedCount;
		++matchedCount;

		if (matchIndex < offset){
			continue;
		}
		if (count >= 0 && revisionItemList.size() >= count){
			continue;
		}

		revisionItemList << revisionItem;
	}

	response.totalCount = matchedCount;
	response.revisions = std::move(revisionItemList);

	return response;
}


sdl::V1_0::imtbase::CBackupRevisionResponse CDocumentRevisionControllerComp::OnBackupRevision(
	const sdl::V1_0::imtbase::CBackupRevisionGqlRequest& /*backupRevisionRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return sdl::V1_0::imtbase::CBackupRevisionResponse();
}


sdl::V1_0::imtbase::CRestoreRevisionResponse CDocumentRevisionControllerComp::OnRestoreRevision(
	const sdl::V1_0::imtbase::CRestoreRevisionGqlRequest& restoreRevisionRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& errorMessage) const
{
	sdl::V1_0::imtbase::CRestoreRevisionResponse response;
	
	sdl::V1_0::imtbase::RestoreRevisionRequestArguments arguments = restoreRevisionRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		errorMessage = QString("Unable to restore revision. Error: Request invalid");
		return sdl::V1_0::imtbase::CRestoreRevisionResponse();
	}

	QByteArray documentId;
	if (arguments.input->objectId){
		documentId = *arguments.input->objectId;
	}
	
	int revisionNumber = -1;
	if (arguments.input->revision){
		revisionNumber = *arguments.input->revision;
	}
	
	QByteArray collectionId;
	if (arguments.input->collectionId){
		collectionId = *arguments.input->collectionId;
	}
	
	if (!IsCollectionSupported(collectionId)){
		return sdl::V1_0::imtbase::CRestoreRevisionResponse();
	}

	imtbase::IObjectCollection* objectCollectionPtr = FindObjectCollection(collectionId);
	Q_ASSERT(objectCollectionPtr != nullptr);

	const imtbase::IRevisionController* revisionControllerPtr = objectCollectionPtr->GetRevisionController();
	if (revisionControllerPtr == nullptr){
		errorMessage = QString("Unable to set revision '%1' for document '%2'. Error: revision controller is invalid").arg(revisionNumber).arg(qPrintable(documentId));
		return sdl::V1_0::imtbase::CRestoreRevisionResponse();
	}

	bool ok = revisionControllerPtr->RestoreRevision(*objectCollectionPtr, documentId, revisionNumber);
	if (!ok){
		errorMessage = QString("Unable to set revision '%1' for document '%2'. Error: Restoring object failed").arg(revisionNumber).arg(qPrintable(documentId));
		return sdl::V1_0::imtbase::CRestoreRevisionResponse();
	}

	response.result = (ok);

	return response;
}


sdl::V1_0::imtbase::CExportRevisionResponse CDocumentRevisionControllerComp::OnExportRevision(
	const sdl::V1_0::imtbase::CExportRevisionGqlRequest& /*exportRevisionRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return sdl::V1_0::imtbase::CExportRevisionResponse();
}


sdl::V1_0::imtbase::CDeleteRevisionResponse CDocumentRevisionControllerComp::OnDeleteRevision(
	const sdl::V1_0::imtbase::CDeleteRevisionGqlRequest& deleteRevisionRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& errorMessage) const
{
	sdl::V1_0::imtbase::CDeleteRevisionResponse response;

	const imtgql::IGqlContext* gqlContextPtr = deleteRevisionRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			if (!userInfoPtr->IsAdmin()){
				errorMessage = QString("Unable to delete revision. Error: Invalid permission for user '%1'").arg(userInfoPtr->GetName());

				return sdl::V1_0::imtbase::CDeleteRevisionResponse();
			}
		}
	}
	
	sdl::V1_0::imtbase::DeleteRevisionRequestArguments arguments = deleteRevisionRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		errorMessage = QString("Unable to delete revision. Error: Request invalid");
		return sdl::V1_0::imtbase::CDeleteRevisionResponse();
	}

	QByteArray documentId;
	if (arguments.input->objectId){
		documentId = *arguments.input->objectId;
	}
	
	int revisionNumber = -1;
	if (arguments.input->revision){
		revisionNumber = *arguments.input->revision;
	}
	

	QByteArray collectionId;
	if (arguments.input->collectionId){
		collectionId = *arguments.input->collectionId;
	}

	if (!IsCollectionSupported(collectionId)){
		return sdl::V1_0::imtbase::CDeleteRevisionResponse();
	}

	imtbase::IObjectCollection* objectCollectionPtr = FindObjectCollection(collectionId);
	Q_ASSERT(objectCollectionPtr != nullptr);

	const imtbase::IRevisionController* revisionControllerPtr = objectCollectionPtr->GetRevisionController();
	if (revisionControllerPtr == nullptr){
		errorMessage = QString("Unable to delete revision '%1' for document '%2'. Error: revision controller is invalid").arg(revisionNumber).arg(qPrintable(documentId));
		return sdl::V1_0::imtbase::CDeleteRevisionResponse();
	}

	bool ok = revisionControllerPtr->DeleteRevision(*objectCollectionPtr, documentId, revisionNumber);

	response.result = (ok);

	return response;
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
												sdl::V1_0::imtbase::CGetRevisionInfoListInput::GetRevisionInfoListInputFields::CollectionId.toUtf8()).toByteArray();
	if (collectionId.isEmpty()){
		return false;
	}

	return IsCollectionSupported(collectionId);
}


} // namespace imtservergql


