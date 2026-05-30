// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CCollectionDocumentServiceControllerComp.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService.h>
#include <imtbasesdl/SDL/1.0/CPP/UndoManager.h>


// ACF includes
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtbase/COperationContext.h>
#include <imtauth/CUserInfo.h>


namespace imtservergql
{


// protected methods

// reimplemented (CCollectionDocumentServiceGqlHandlerCompBase)

CDM::CDocumentList CCollectionDocumentServiceControllerComp::OnGetOpenedDocumentList(
			const CDM::CGetOpenedDocumentListGqlRequest& /*getOpenedDocumentListRequest*/,
			const::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	CDM::CDocumentList retVal;
	retVal.documentList.emplace();

	if (m_documentManagerCompPtr.IsValid()){
		QByteArray userId = GetUserId(gqlRequest);

		imtdoc::IDocumentService::DocumentList list = m_documentManagerCompPtr->GetOpenedDocumentList(userId);
		for (const imtdoc::IDocumentService::DocumentListItem& info : list){
			CDM::CDocumentInfo sdlInfo;

			QString path = info.url.path();
			QStringList parts = path.split('/', Qt::SkipEmptyParts);
			QByteArray objectId = parts.count() > 0 ? parts.last().toUtf8() : QByteArray();

			sdlInfo.documentId = info.documentId;
			sdlInfo.documentName = info.name;
			sdlInfo.objectId = objectId;
			sdlInfo.objectTypeId = info.typeId;
			sdlInfo.isDirty = info.isDirty;
			sdlInfo.hasNameProvider = info.hasNameProvider;

			retVal.documentList->append(sdlInfo);
		}
	}

	return retVal;
}


CDM::CDocumentInfo CCollectionDocumentServiceControllerComp::OnCreateNewDocument(
			const CDM::CCreateNewDocumentGqlRequest& createNewDocumentRequest,
			const::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	CDM::CDocumentInfo retVal;

	const auto& arguments = createNewDocumentRequest.GetRequestedArguments();

	auto documentTypeId = arguments.input;
	if (!documentTypeId.typeId){
		errorMessage = "Invalid GraphQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	if (m_documentManagerCompPtr.IsValid()) {
		const QByteArray proposedSourceDocumentId = documentTypeId.proposedSourceDocumentId
			? *documentTypeId.proposedSourceDocumentId
			: QByteArray();
		imtdoc::IDocumentService::TaskParams taskParams;
		taskParams.userId = userId;
		taskParams.documentTypeId = *documentTypeId.typeId;
		taskParams.proposedSourceDocumentId = proposedSourceDocumentId;
		QByteArray taskId = m_documentManagerCompPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, taskParams);
		imtdoc::IDocumentService::TaskResult taskResult = m_documentManagerCompPtr->WaitForTaskFinished(taskId);
		QByteArray documentId = taskResult.documentId;
		if (documentId.isEmpty()){
			errorMessage = "Unable to create document or undo manager";

			return retVal;
		}

		QString documentName;
		m_documentManagerCompPtr->GetDocumentName(userId, documentId, documentName);

		retVal.documentId = documentId;
		retVal.documentName = documentName;
		retVal.objectTypeId = *documentTypeId.typeId;
		retVal.objectId = QByteArray();
		retVal.isDirty = false;
		retVal.hasNameProvider = false;
		retVal.isLoading = false;

		imtdoc::IDocumentService::DocumentList list = m_documentManagerCompPtr->GetOpenedDocumentList(userId);
		for (const imtdoc::IDocumentService::DocumentListItem& info : list){
			if (info.documentId == documentId){
				retVal.hasNameProvider = info.hasNameProvider;
				break;
			}
		}
	}

	return retVal;
}


CDM::CDocumentInfo CCollectionDocumentServiceControllerComp::OnOpenDocument(
			const CDM::COpenDocumentGqlRequest& openDocumentRequest,
			const::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	CDM::CDocumentInfo retVal;

	const auto& arguments = openDocumentRequest.GetRequestedArguments();

	auto objectId = arguments.input;
	if (!objectId.id){
		errorMessage = "Invalid GraphQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	QUrl url(QString("collection:///%1").arg(*objectId.id));

	if (m_documentManagerCompPtr.IsValid()) {
		imtdoc::IDocumentService::TaskParams taskParams;
		taskParams.userId = userId;
		taskParams.url = url;
		QByteArray taskId = m_documentManagerCompPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_OPEN, taskParams);
		imtdoc::IDocumentService::TaskResult taskResult = m_documentManagerCompPtr->WaitForTaskFinished(taskId);
		QByteArray documentId = taskResult.documentId;
		if (documentId.isEmpty()){
			errorMessage = "Unable to open document or create undo manager";

			return retVal;
		}

		retVal.documentId = documentId;
		retVal.objectId = *objectId.id;
		retVal.isDirty = false;
		retVal.hasNameProvider = false;
		retVal.isLoading = true;

		imtdoc::IDocumentService::DocumentList list = m_documentManagerCompPtr->GetOpenedDocumentList(userId);
		for (const imtdoc::IDocumentService::DocumentListItem& info : list){
			if (info.documentId == documentId){
				retVal.documentName = info.name;
				retVal.objectTypeId = info.typeId;
				retVal.hasNameProvider = info.hasNameProvider;
				retVal.isLoading = info.isLoading;
				break;
			}
		}
	}

	return retVal;
}


CDM::CDocumentInfo CCollectionDocumentServiceControllerComp::OnGetDocumentName(
			const CDM::CGetDocumentNameGqlRequest& getDocumentNameRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	CDM::CDocumentInfo retVal;

	const auto& arguments = getDocumentNameRequest.GetRequestedArguments();

	auto documentId = arguments.input;
	if (!documentId.id){
		errorMessage = "Invalid GraphQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	if (m_documentManagerCompPtr.IsValid()) {
		QString name;
		imtdoc::IDocumentService::OperationStatus status = m_documentManagerCompPtr->GetDocumentName(userId, *documentId.id, name);

		switch (status){
		case imtdoc::IDocumentService::OS_OK:{
			retVal.documentId = *documentId.id;
			retVal.documentName = name;
			break;
		}

		case imtdoc::IDocumentService::OS_INVALID_USER_ID:
			errorMessage = "Invalid user-ID";
			break;

		case imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID:
			errorMessage = "Invalid document ID";
			break;
		default:
			Q_ASSERT(false);
			break;
		}
	}

	return retVal;
}


CDM::CDocumentOperationStatus CCollectionDocumentServiceControllerComp::OnSetDocumentName(
			const CDM::CSetDocumentNameGqlRequest& setDocumentNameRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	CDM::CDocumentOperationStatus retVal;

	const auto& arguments = setDocumentNameRequest.GetRequestedArguments();

	auto documentId = arguments.input;
	if (!documentId.documentId || !documentId.documentName){
		errorMessage = "Invalid GraphQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	if (m_documentManagerCompPtr.IsValid()) {
		imtdoc::IDocumentService::OperationStatus status = m_documentManagerCompPtr->SetDocumentName(userId, *documentId.documentId, *documentId.documentName);
		switch (status){
		case imtdoc::IDocumentService::OS_OK:
			retVal.status = CDM::EDocumentOperationStatus::Success;
			break;
		case imtdoc::IDocumentService::OS_INVALID_USER_ID:
			errorMessage = "Invalid user-ID";
			retVal.status = CDM::EDocumentOperationStatus::InvalidUserId;
			break;
		case imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID:
			retVal.status = CDM::EDocumentOperationStatus::InvalidDocumentId;
			errorMessage = "Invalid document ID";
			break;
		case imtdoc::IDocumentService::OS_FAILED:
			retVal.status = CDM::EDocumentOperationStatus::Failed;
			errorMessage = "Failed to set document name";
			break;
		default:
			Q_ASSERT(false);
			break;
		}
	}

	return retVal;
}


CDM::CDocumentOperationStatus CCollectionDocumentServiceControllerComp::OnSaveDocument(
			const CDM::CSaveDocumentGqlRequest& saveDocumentRequest,
			const::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	CDM::CDocumentOperationStatus retVal;

	const auto& arguments = saveDocumentRequest.GetRequestedArguments();

	auto saveDocumentInput = arguments.input;
	if (!saveDocumentInput.documentId || !saveDocumentInput.documentName){
		errorMessage = "Invalid GraphQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	if (m_documentManagerCompPtr.IsValid()) {
		istd::TDelPtr<imtbase::IOperationContext> operationContextPtr;
		operationContextPtr.SetPtr(CreateOperationContextFromGqlRequest(gqlRequest));

		imtdoc::IDocumentService::TaskParams taskParams;
		taskParams.userId = userId;
		taskParams.documentId = *saveDocumentInput.documentId;
		taskParams.documentName = *saveDocumentInput.documentName;
		taskParams.operationContextPtr = operationContextPtr.GetPtr();
		QByteArray taskId = m_documentManagerCompPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, taskParams);
		imtdoc::IDocumentService::TaskResult taskResult = m_documentManagerCompPtr->WaitForTaskFinished(taskId);
		QString saveErrorMessage = taskResult.errorMessage;
		imtdoc::IDocumentService::OperationStatus status = taskResult.status;
		QString responseMessage;
		switch (status){
		case imtdoc::IDocumentService::OS_OK:
			retVal.status = CDM::EDocumentOperationStatus::Success;
			{
				QString resolvedName;
				if (m_documentManagerCompPtr->GetDocumentName(userId, *saveDocumentInput.documentId, resolvedName) == imtdoc::IDocumentService::OS_OK){
					retVal.documentName = resolvedName;
				}
			}
			break;
		case imtdoc::IDocumentService::OS_INVALID_USER_ID:
			retVal.status = CDM::EDocumentOperationStatus::InvalidUserId;
			responseMessage = "Invalid user-ID";
			break;
		case imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID:
			retVal.status = CDM::EDocumentOperationStatus::InvalidDocumentId;
			responseMessage = "Invalid document ID";
			break;
		case imtdoc::IDocumentService::OS_INVALID_DOCUMENT_DATA:
			retVal.status = CDM::EDocumentOperationStatus::InvalidDocumentData;
			responseMessage = saveErrorMessage.isEmpty() ? "Document data is invalid" : saveErrorMessage;
			break;
		case imtdoc::IDocumentService::OS_FAILED:
			retVal.status = CDM::EDocumentOperationStatus::Failed;
			responseMessage = "Failed to save document";
			break;
		default:
			break;
		}
		if (!responseMessage.isEmpty()) {
			retVal.message = responseMessage;
			errorMessage = responseMessage;
		}
	}

	return retVal;
}


CDM::CDocumentOperationStatus CCollectionDocumentServiceControllerComp::OnCloseDocument(
			const CDM::CCloseDocumentGqlRequest& closeDocumentRequest,
			const::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	CDM::CDocumentOperationStatus retVal;

	const auto& arguments = closeDocumentRequest.GetRequestedArguments();

	auto documentId = arguments.input;
	if (!documentId.id){
		errorMessage = "Invalid GraphQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	if (m_documentManagerCompPtr.IsValid()) {
		imtdoc::IDocumentService::TaskParams taskParams;
		taskParams.userId = userId;
		taskParams.documentId = *documentId.id;
		QByteArray taskId = m_documentManagerCompPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_CLOSE, taskParams);
		imtdoc::IDocumentService::TaskResult taskResult = m_documentManagerCompPtr->WaitForTaskFinished(taskId);
		imtdoc::IDocumentService::OperationStatus status = taskResult.status;
		switch (status){
		case imtdoc::IDocumentService::OS_OK:
			retVal.status = CDM::EDocumentOperationStatus::Success;
			break;
		case imtdoc::IDocumentService::OS_INVALID_USER_ID:
			retVal.status = CDM::EDocumentOperationStatus::InvalidUserId;
			break;
		case imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID:
			retVal.status = CDM::EDocumentOperationStatus::InvalidDocumentId;
			break;
		case imtdoc::IDocumentService::OS_FAILED:
			retVal.status = CDM::EDocumentOperationStatus::Failed;
			break;
		default:
			break;
		}
	}

	return retVal;
}


sdl::V1_0::imtbase::CUndoInfo CCollectionDocumentServiceControllerComp::OnGetUndoInfo(
			const CDM::CGetUndoInfoGqlRequest& getUndoInfoRequest,
			const::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CUndoInfo retVal;
	retVal.status.emplace();

	const auto& arguments = getUndoInfoRequest.GetRequestedArguments();

	auto documentId = arguments.input;
	if (!documentId.id){
		errorMessage = "Invalid GraphQL request params";

		retVal.status->status = sdl::V1_0::imtbase::EUndoStatus::Failed;

		return retVal;
	}

	retVal.documentId = documentId.id;

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	if (m_documentManagerCompPtr.IsValid()) {
		idoc::IUndoManager* undoManagerPtr = nullptr;
		if (m_documentManagerCompPtr-> GetDocumentUndoManager(userId, *documentId.id, undoManagerPtr) != imtdoc::IDocumentService::OS_OK){
			errorMessage = "Undo manager not available";

			retVal.status->status = sdl::V1_0::imtbase::EUndoStatus::Failed;

			return retVal;
		}

		retVal.isDirty = undoManagerPtr->GetDocumentChangeFlag() != idoc::IDocumentStateComparator::DCF_EQUAL;
		retVal.status.emplace().status = sdl::V1_0::imtbase::EUndoStatus::Success;

		int count = undoManagerPtr->GetAvailableUndoSteps();
		retVal.availableUndoSteps = count;
		retVal.undoLevelDescriptions.emplace();
		for (int i = 0; i < count; i++){
			QString description = undoManagerPtr->GetUndoLevelDescription(i);
			retVal.undoLevelDescriptions->append(description);
		}

		count = undoManagerPtr->GetAvailableRedoSteps();
		retVal.availableRedoSteps = count;
		retVal.redoLevelDescriptions.emplace();
		for (int i = 0; i < count; i++){
			QString description = undoManagerPtr->GetRedoLevelDescription(i);
			retVal.redoLevelDescriptions->append(description);
		}

		retVal.status->status = sdl::V1_0::imtbase::EUndoStatus::Success;
	}

	return retVal;
}


sdl::V1_0::imtbase::CUndoStatus CCollectionDocumentServiceControllerComp::OnDoUndo(
			const CDM::CDoUndoGqlRequest& doUndoRequest,
			const::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CUndoStatus retVal;
	retVal.status.emplace();

	const auto& arguments = doUndoRequest.GetRequestedArguments();

	istd::TSharedNullable<CDM::CCollectionUndoRedoInput> collectionUndoRedoInput = arguments.input;

	if (!collectionUndoRedoInput || !collectionUndoRedoInput->undoRedoInput){
		errorMessage = "Invalid GraphQL request params";
		
		return retVal;
	}

	istd::TSharedNullable<sdl::V1_0::imtbase::CUndoRedoInput> undoRedoInput = collectionUndoRedoInput->undoRedoInput;

	if (!undoRedoInput || !undoRedoInput->documentId || !undoRedoInput->steps){
		errorMessage = "Invalid GraphQL request params";

		retVal.status = sdl::V1_0::imtbase::EUndoStatus::InvalidDocumentId;

		return retVal;
	}

	QByteArray documentId = *undoRedoInput->documentId;
	QByteArray userId = GetUserId(gqlRequest);

	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	if (m_documentManagerCompPtr.IsValid()) {
		idoc::IUndoManager* undoManagerPtr = nullptr;
		if (m_documentManagerCompPtr->GetDocumentUndoManager(userId, documentId, undoManagerPtr) != imtdoc::IDocumentService::OS_OK) {
			errorMessage = "Undo manager not available";

			retVal.status = sdl::V1_0::imtbase::EUndoStatus::Failed;

			return retVal;
		}

		if (undoManagerPtr->GetAvailableUndoSteps() < *undoRedoInput->steps){
			errorMessage = "The number of available undo steps is less than requested";

			retVal.status = sdl::V1_0::imtbase::EUndoStatus::InvalidStepCount;

			return retVal;
		}

		if (!undoManagerPtr->DoUndo(*undoRedoInput->steps)){
			errorMessage = "Undo operation failed";

			retVal.status = sdl::V1_0::imtbase::EUndoStatus::Failed;

			return retVal;
		}

		retVal.status = sdl::V1_0::imtbase::EUndoStatus::Success;
	}

	return retVal;
}


sdl::V1_0::imtbase::CUndoStatus CCollectionDocumentServiceControllerComp::OnDoRedo(
			const CDM::CDoRedoGqlRequest& doRedoRequest,
			const::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CUndoStatus retVal;
	retVal.status.emplace();

	const auto& arguments = doRedoRequest.GetRequestedArguments();

	istd::TSharedNullable<CDM::CCollectionUndoRedoInput> collectionUndoRedoInput = arguments.input;

	if (!collectionUndoRedoInput || !collectionUndoRedoInput->undoRedoInput){
		errorMessage = "Invalid GraphQL request params";

		return retVal;
	}

	istd::TSharedNullable<sdl::V1_0::imtbase::CUndoRedoInput> undoRedoInput = collectionUndoRedoInput->undoRedoInput;

	if (!undoRedoInput || !undoRedoInput->documentId || !undoRedoInput->steps){
		errorMessage = "Invalid GraphQL request params";

		retVal.status = sdl::V1_0::imtbase::EUndoStatus::InvalidDocumentId;

		return retVal;
	}

	QByteArray documentId = *undoRedoInput->documentId;
	QByteArray userId = GetUserId(gqlRequest);

	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	if (m_documentManagerCompPtr.IsValid()) {
		idoc::IUndoManager* undoManagerPtr = nullptr;
		if (m_documentManagerCompPtr->GetDocumentUndoManager(userId, documentId, undoManagerPtr) != imtdoc::IDocumentService::OS_OK) {
			errorMessage = "Undo manager not available";

			retVal.status = sdl::V1_0::imtbase::EUndoStatus::Failed;

			return retVal;
		}

		if (undoManagerPtr->GetAvailableRedoSteps() < *undoRedoInput->steps){
			errorMessage = "The number of available redo steps is less than requested";

			retVal.status = sdl::V1_0::imtbase::EUndoStatus::InvalidStepCount;

			return retVal;
		}

		if (!undoManagerPtr->DoRedo(*undoRedoInput->steps)){
			errorMessage = "Redo operation failed";

			retVal.status = sdl::V1_0::imtbase::EUndoStatus::Failed;

			return retVal;
		}

		retVal.status = sdl::V1_0::imtbase::EUndoStatus::Success;
	}

	return retVal;
}


sdl::V1_0::imtbase::CUndoStatus CCollectionDocumentServiceControllerComp::OnResetUndo(
			const CDM::CResetUndoGqlRequest& resetUndoRequest,
			const::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CUndoStatus retVal;
	retVal.status.emplace();

	const auto& arguments = resetUndoRequest.GetRequestedArguments();

	auto documentId = arguments.input;
	if (!documentId.id){
		errorMessage = "Invalid GraphQL request params";

		retVal.status = sdl::V1_0::imtbase::EUndoStatus::InvalidDocumentId;

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);

	if (userId.isEmpty()){
		SendWarningMessage(0, "Unable to get user-ID from context");
	}

	if (m_documentManagerCompPtr.IsValid()) {
		idoc::IUndoManager* undoManagerPtr = nullptr;
		if (m_documentManagerCompPtr->GetDocumentUndoManager(userId, *documentId.id, undoManagerPtr) != imtdoc::IDocumentService::OS_OK) {
			errorMessage = "Undo manager not available";

			retVal.status = sdl::V1_0::imtbase::EUndoStatus::Failed;

			return retVal;
		}

		undoManagerPtr->ResetUndo();

		retVal.status = sdl::V1_0::imtbase::EUndoStatus::Success;
	}

	return retVal;
}


// reimplemented (::imtservergql::CPermissibleGqlRequestHandlerComp)

bool CCollectionDocumentServiceControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
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

imtbase::IOperationContext* CCollectionDocumentServiceControllerComp::CreateOperationContextFromGqlRequest(const ::imtgql::CGqlRequest& gqlRequest) const
{
	const imtgql::IGqlContext* requestContextPtr = gqlRequest.GetRequestContext();
	if (requestContextPtr == nullptr){
		return nullptr;
	}

	const imtauth::CIdentifiableUserInfo* userInfoPtr = dynamic_cast<const imtauth::CIdentifiableUserInfo*>(requestContextPtr->GetUserInfo());

	istd::TDelPtr<imtbase::COperationContext> operationContextPtr;
	operationContextPtr.SetPtr(new imtbase::COperationContext);

	if (userInfoPtr != nullptr){
		imtbase::IOperationContext::IdentifableObjectInfo objectInfo;
		objectInfo.id = userInfoPtr->GetObjectUuid();
		objectInfo.name = userInfoPtr->GetName();
		operationContextPtr->SetOperationOwnerId(objectInfo);
	}

	QByteArray tenantId = requestContextPtr->GetTenantId();
	operationContextPtr->SetTenantId(tenantId);

	return operationContextPtr.PopPtr();
}


QByteArray CCollectionDocumentServiceControllerComp::GetUserId(const ::imtgql::CGqlRequest& gqlRequest) const
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
