// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CCollectionDocumentManagerControllerComp.h>


namespace imtservergql
{


// protected methods

// reimplemented (CGraphQlHandlerCompBase)

CDM::CDocumentList CCollectionDocumentManagerControllerComp::OnGetOpenedDocumentList(
			const CDM::CGetOpenedDocumentListGqlRequest& /*getOpenedDocumentListRequest*/,
			const::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	CDM::CDocumentList retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->documentList.emplace();

	if (m_documentManagerCompPtr.IsValid()){
		QByteArray userId = GetUserId(gqlRequest);

		imtdoc::IDocumentManager::DocumentList list = m_documentManagerCompPtr->GetOpenedDocumentList(userId);
		for (const imtdoc::IDocumentManager::DocumentListItem& info : list){
			CDM::CDocumentInfo sdlInfo;
			sdlInfo.Version_1_0.emplace();

			QString path = info.url.path();
			QStringList parts = path.split('/', Qt::SkipEmptyParts);
			QByteArray objectId = parts.count() > 0 ? parts.last().toUtf8() : QByteArray();

			sdlInfo.Version_1_0->documentId = info.documentId;
			sdlInfo.Version_1_0->documentName = info.name;
			sdlInfo.Version_1_0->objectId = objectId;
			sdlInfo.Version_1_0->objectTypeId = info.typeId;
			sdlInfo.Version_1_0->isDirty = info.isDirty;

			retVal.Version_1_0->documentList->append(sdlInfo.Version_1_0);
		}
	}

	return retVal;
}


CDM::CDocumentId CCollectionDocumentManagerControllerComp::OnCreateNewDocument(
			const CDM::CCreateNewDocumentGqlRequest& createNewDocumentRequest,
			const::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	CDM::CDocumentId retVal;

	const auto& arguments = createNewDocumentRequest.GetRequestedArguments();

	auto documentTypeId = arguments.input.Version_1_0;
	if (!documentTypeId || !documentTypeId->typeId){
		errorMessage = "Invalid GraphQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	if (m_documentManagerCompPtr.IsValid()) {
		QByteArray documentId = m_documentManagerCompPtr->CreateNewDocument(userId, *documentTypeId->typeId);
		if (documentId.isEmpty()){
			errorMessage = "Unable to create document or undo manager";

			return retVal;
		}

		retVal.Version_1_0.emplace().id = documentId;
	}

	return retVal;
}


CDM::CDocumentId CCollectionDocumentManagerControllerComp::OnOpenDocument(
			const CDM::COpenDocumentGqlRequest& openDocumentRequest,
			const::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	CDM::CDocumentId retVal;

	const auto& arguments = openDocumentRequest.GetRequestedArguments();

	auto objectId = arguments.input.Version_1_0;
	if (!objectId || !objectId->id){
		errorMessage = "Invalid GraphQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	QUrl url(QString("collection:///%1").arg(*objectId->id));

	if (m_documentManagerCompPtr.IsValid()) {
		QByteArray documentId = m_documentManagerCompPtr->OpenDocument(userId, url);
		if (documentId.isEmpty()){
			errorMessage = "Unable to open document or create undo manager";

			return retVal;
		}

		retVal.Version_1_0.emplace().id = documentId;
	}

	return retVal;
}


CDM::CDocumentInfo CCollectionDocumentManagerControllerComp::OnGetDocumentName(
			const CDM::CGetDocumentNameGqlRequest& getDocumentNameRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	CDM::CDocumentInfo retVal;

	const auto& arguments = getDocumentNameRequest.GetRequestedArguments();

	auto documentId = arguments.input.Version_1_0;
	if (!documentId || !documentId->id){
		errorMessage = "Invalid GraphQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	if (m_documentManagerCompPtr.IsValid()) {
		QString name;
		imtdoc::IDocumentManager::OperationStatus status = m_documentManagerCompPtr->GetDocumentName(userId, *documentId->id, name);

		switch (status){
		case imtdoc::IDocumentManager::OS_OK:{
			retVal.Version_1_0.emplace();
			retVal.Version_1_0->documentId = *documentId->id;
			retVal.Version_1_0->documentName = name;
			break;
		}

		case imtdoc::IDocumentManager::OS_INVALID_USER_ID:
			errorMessage = "Invalid user-ID";
			break;

		case imtdoc::IDocumentManager::OS_INVALID_DOCUMENT_ID:
			errorMessage = "Invalid document ID";
			break;
		default:
			Q_ASSERT(false);
			break;
		}
	}

	return retVal;
}


CDM::CDocumentOperationStatus CCollectionDocumentManagerControllerComp::OnSetDocumentName(
			const CDM::CSetDocumentNameGqlRequest& setDocumentNameRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	CDM::CDocumentOperationStatus retVal;

	const auto& arguments = setDocumentNameRequest.GetRequestedArguments();

	auto documentId = arguments.input.Version_1_0;
	if (!documentId || !documentId->documentId || !documentId->documentName){
		errorMessage = "Invalid GraphQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	retVal.Version_1_0.emplace();

	if (m_documentManagerCompPtr.IsValid()) {
		imtdoc::IDocumentManager::OperationStatus status = m_documentManagerCompPtr->SetDocumentName(userId, *documentId->documentId, *documentId->documentName);
		switch (status){
		case imtdoc::IDocumentManager::OS_OK:
			retVal.Version_1_0->status = CDM::EDocumentOperationStatus::Success;
			break;
		case imtdoc::IDocumentManager::OS_INVALID_USER_ID:
			errorMessage = "Invalid user-ID";
			retVal.Version_1_0->status = CDM::EDocumentOperationStatus::InvalidUserId;
			break;
		case imtdoc::IDocumentManager::OS_INVALID_DOCUMENT_ID:
			retVal.Version_1_0->status = CDM::EDocumentOperationStatus::InvalidDocumentId;
			errorMessage = "Invalid document ID";
			break;
		case imtdoc::IDocumentManager::OS_FAILED:
			retVal.Version_1_0->status = CDM::EDocumentOperationStatus::Failed;
			errorMessage = "Failed to set document name";
			break;
		default:
			Q_ASSERT(false);
			break;
		}
	}

	return retVal;
}


CDM::CDocumentOperationStatus CCollectionDocumentManagerControllerComp::OnSaveDocument(
			const CDM::CSaveDocumentGqlRequest& saveDocumentRequest,
			const::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	CDM::CDocumentOperationStatus retVal;

	const auto& arguments = saveDocumentRequest.GetRequestedArguments();

	auto saveDocumentInput = arguments.input.Version_1_0;
	if (!saveDocumentInput || !saveDocumentInput->documentId || !saveDocumentInput->documentName){
		errorMessage = "Invalid GraphQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	retVal.Version_1_0.emplace();

	if (m_documentManagerCompPtr.IsValid()) {
		imtdoc::IDocumentManager::OperationStatus status = m_documentManagerCompPtr->SaveDocument(userId, *saveDocumentInput->documentId, *saveDocumentInput->documentName);
		switch (status){
		case imtdoc::IDocumentManager::OS_OK:
			retVal.Version_1_0->status = CDM::EDocumentOperationStatus::Success;
			break;
		case imtdoc::IDocumentManager::OS_INVALID_USER_ID:
			retVal.Version_1_0->status = CDM::EDocumentOperationStatus::InvalidUserId;
			break;
		case imtdoc::IDocumentManager::OS_INVALID_DOCUMENT_ID:
			retVal.Version_1_0->status = CDM::EDocumentOperationStatus::InvalidDocumentId;
			break;
		case imtdoc::IDocumentManager::OS_FAILED:
			retVal.Version_1_0->status = CDM::EDocumentOperationStatus::Failed;
			break;
		default:
			break;
		}

		if (status != imtdoc::IDocumentManager::OperationStatus::OS_OK){
			errorMessage = "Unable to open document or create undo manager";
		}
	}

	return retVal;
}


CDM::CDocumentOperationStatus CCollectionDocumentManagerControllerComp::OnCloseDocument(
			const CDM::CCloseDocumentGqlRequest& closeDocumentRequest,
			const::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	CDM::CDocumentOperationStatus retVal;

	const auto& arguments = closeDocumentRequest.GetRequestedArguments();

	auto documentId = arguments.input.Version_1_0;
	if (!documentId || !documentId->id){
		errorMessage = "Invalid GraphQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	retVal.Version_1_0.emplace();

	if (m_documentManagerCompPtr.IsValid()) {
		imtdoc::IDocumentManager::OperationStatus status = m_documentManagerCompPtr->CloseDocument(userId, *documentId->id);
		switch (status){
		case imtdoc::IDocumentManager::OS_OK:
			retVal.Version_1_0->status = CDM::EDocumentOperationStatus::Success;
			break;
		case imtdoc::IDocumentManager::OS_INVALID_USER_ID:
			retVal.Version_1_0->status = CDM::EDocumentOperationStatus::InvalidUserId;
			break;
		case imtdoc::IDocumentManager::OS_INVALID_DOCUMENT_ID:
			retVal.Version_1_0->status = CDM::EDocumentOperationStatus::InvalidDocumentId;
			break;
		case imtdoc::IDocumentManager::OS_FAILED:
			retVal.Version_1_0->status = CDM::EDocumentOperationStatus::Failed;
			break;
		default:
			break;
		}
	}

	return retVal;
}


sdl::imtbase::UndoManager::CUndoInfo CCollectionDocumentManagerControllerComp::OnGetUndoInfo(
			const CDM::CGetUndoInfoGqlRequest& getUndoInfoRequest,
			const::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::UndoManager::CUndoInfo retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->status.emplace();

	const auto& arguments = getUndoInfoRequest.GetRequestedArguments();

	auto documentId = arguments.input.Version_1_0;
	if (!documentId || !documentId->id){
		errorMessage = "Invalid GraphQL request params";

		retVal.Version_1_0->status->status = sdl::imtbase::UndoManager::EUndoStatus::Failed;

		return retVal;
	}

	retVal.Version_1_0->documentId = documentId->id;

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	if (m_documentManagerCompPtr.IsValid()) {
		idoc::IUndoManager* undoManagerPtr = nullptr;
		if (m_documentManagerCompPtr-> GetDocumentUndoManager(userId, *documentId->id, undoManagerPtr) != imtdoc::IDocumentManager::OS_OK){
			errorMessage = "Undo manager not available";

			retVal.Version_1_0->status->status = sdl::imtbase::UndoManager::EUndoStatus::Failed;

			return retVal;
		}

		retVal.Version_1_0->isDirty = undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL;
		retVal.Version_1_0->status.emplace().status = sdl::imtbase::UndoManager::EUndoStatus::Success;

		int count = undoManagerPtr->GetAvailableUndoSteps();
		retVal.Version_1_0->availableUndoSteps = count;
		retVal.Version_1_0->undoLevelDescriptions.emplace();
		for (int i = 0; i < count; i++){
			QString description = undoManagerPtr->GetUndoLevelDescription(i);
			retVal.Version_1_0->undoLevelDescriptions->append(description);
		}

		count = undoManagerPtr->GetAvailableRedoSteps();
		retVal.Version_1_0->availableRedoSteps = count;
		retVal.Version_1_0->redoLevelDescriptions.emplace();
		for (int i = 0; i < count; i++){
			QString description = undoManagerPtr->GetRedoLevelDescription(i);
			retVal.Version_1_0->redoLevelDescriptions->append(description);
		}

		retVal.Version_1_0->status->status = sdl::imtbase::UndoManager::EUndoStatus::Success;
	}

	return retVal;
}


sdl::imtbase::UndoManager::CUndoStatus CCollectionDocumentManagerControllerComp::OnDoUndo(
			const CDM::CDoUndoGqlRequest& doUndoRequest,
			const::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::UndoManager::CUndoStatus retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->status.emplace();

	const auto& arguments = doUndoRequest.GetRequestedArguments();

	istd::TSharedNullable<CDM::CCollectionUndoRedoInput::V1_0> collectionUndoRedoInput = arguments.input.Version_1_0;

	if (!collectionUndoRedoInput || !collectionUndoRedoInput->undoRedoInput){
		errorMessage = "Invalid GraphQL request params";
		
		return retVal;
	}

	istd::TSharedNullable<sdl::imtbase::UndoManager::CUndoRedoInput::V1_0> undoRedoInput = collectionUndoRedoInput->undoRedoInput;

	if (!undoRedoInput || !undoRedoInput->documentId || !undoRedoInput->steps){
		errorMessage = "Invalid GraphQL request params";

		retVal.Version_1_0->status = sdl::imtbase::UndoManager::EUndoStatus::InvalidDocumentId;

		return retVal;
	}

	QByteArray documentId = *undoRedoInput->documentId;
	QByteArray userId = GetUserId(gqlRequest);

	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	if (m_documentManagerCompPtr.IsValid()) {
		idoc::IUndoManager* undoManagerPtr = nullptr;
		if (m_documentManagerCompPtr->GetDocumentUndoManager(userId, documentId, undoManagerPtr) != imtdoc::IDocumentManager::OS_OK) {
			errorMessage = "Undo manager not available";

			retVal.Version_1_0->status = sdl::imtbase::UndoManager::EUndoStatus::Failed;

			return retVal;
		}

		if (undoManagerPtr->GetAvailableUndoSteps() < *undoRedoInput->steps){
			errorMessage = "The number of available undo steps is less than requested";

			retVal.Version_1_0->status = sdl::imtbase::UndoManager::EUndoStatus::InvalidStepCount;

			return retVal;
		}

		if (!undoManagerPtr->DoUndo(*undoRedoInput->steps)){
			errorMessage = "Undo operation failed";

			retVal.Version_1_0->status = sdl::imtbase::UndoManager::EUndoStatus::Failed;

			return retVal;
		}

		retVal.Version_1_0->status = sdl::imtbase::UndoManager::EUndoStatus::Success;
	}

	return retVal;
}


sdl::imtbase::UndoManager::CUndoStatus CCollectionDocumentManagerControllerComp::OnDoRedo(
			const CDM::CDoRedoGqlRequest& doRedoRequest,
			const::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::UndoManager::CUndoStatus retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->status.emplace();

	const auto& arguments = doRedoRequest.GetRequestedArguments();

	istd::TSharedNullable<CDM::CCollectionUndoRedoInput::V1_0> collectionUndoRedoInput = arguments.input.Version_1_0;

	if (!collectionUndoRedoInput || !collectionUndoRedoInput->undoRedoInput){
		errorMessage = "Invalid GraphQL request params";

		return retVal;
	}

	istd::TSharedNullable<sdl::imtbase::UndoManager::CUndoRedoInput::V1_0> undoRedoInput = collectionUndoRedoInput->undoRedoInput;

	if (!undoRedoInput || !undoRedoInput->documentId || !undoRedoInput->steps){
		errorMessage = "Invalid GraphQL request params";

		retVal.Version_1_0->status = sdl::imtbase::UndoManager::EUndoStatus::InvalidDocumentId;

		return retVal;
	}

	QByteArray documentId = *undoRedoInput->documentId;
	QByteArray userId = GetUserId(gqlRequest);

	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	if (m_documentManagerCompPtr.IsValid()) {
		idoc::IUndoManager* undoManagerPtr = nullptr;
		if (m_documentManagerCompPtr->GetDocumentUndoManager(userId, documentId, undoManagerPtr) != imtdoc::IDocumentManager::OS_OK) {
			errorMessage = "Undo manager not available";

			retVal.Version_1_0->status = sdl::imtbase::UndoManager::EUndoStatus::Failed;

			return retVal;
		}

		if (undoManagerPtr->GetAvailableRedoSteps() < *undoRedoInput->steps){
			errorMessage = "The number of available redo steps is less than requested";

			retVal.Version_1_0->status = sdl::imtbase::UndoManager::EUndoStatus::InvalidStepCount;

			return retVal;
		}

		if (!undoManagerPtr->DoRedo(*undoRedoInput->steps)){
			errorMessage = "Redo operation failed";

			retVal.Version_1_0->status = sdl::imtbase::UndoManager::EUndoStatus::Failed;

			return retVal;
		}

		retVal.Version_1_0->status = sdl::imtbase::UndoManager::EUndoStatus::Success;
	}

	return retVal;
}


sdl::imtbase::UndoManager::CUndoStatus CCollectionDocumentManagerControllerComp::OnResetUndo(
			const CDM::CResetUndoGqlRequest& resetUndoRequest,
			const::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::UndoManager::CUndoStatus retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->status.emplace();

	const auto& arguments = resetUndoRequest.GetRequestedArguments();

	auto documentId = arguments.input.Version_1_0;
	if (!documentId || !documentId->id){
		errorMessage = "Invalid GraphQL request params";

		retVal.Version_1_0->status = sdl::imtbase::UndoManager::EUndoStatus::InvalidDocumentId;

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);

	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	if (m_documentManagerCompPtr.IsValid()) {
		idoc::IUndoManager* undoManagerPtr = nullptr;
		if (m_documentManagerCompPtr->GetDocumentUndoManager(userId, *documentId->id, undoManagerPtr) != imtdoc::IDocumentManager::OS_OK) {
			errorMessage = "Undo manager not available";

			retVal.Version_1_0->status = sdl::imtbase::UndoManager::EUndoStatus::Failed;

			return retVal;
		}

		undoManagerPtr->ResetUndo();

		retVal.Version_1_0->status = sdl::imtbase::UndoManager::EUndoStatus::Success;
	}

	return retVal;
}


// reimplemented (::imtservergql::CPermissibleGqlRequestHandlerComp)

bool CCollectionDocumentManagerControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
	if (isSupported){
		const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
		if (inputParamPtr == nullptr){
			return false;
		}

		QByteArray collectionId = inputParamPtr->GetParamArgumentValue("collectionId").toByteArray();

		return m_documentManagerCompPtr.IsValid() && (collectionId == *m_collectionIdAttrPtr);
	}

	return false;
}


// private methods

QByteArray CCollectionDocumentManagerControllerComp::GetUserId(const ::imtgql::CGqlRequest& gqlRequest) const
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr != nullptr){
		const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			return userInfoPtr->GetId();
		}
	}

	return QByteArray();
}


} // namespace imtservergql

