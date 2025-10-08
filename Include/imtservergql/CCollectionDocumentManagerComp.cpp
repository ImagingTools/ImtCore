#include <imtservergql/CCollectionDocumentManagerComp.h>


namespace imtservergql
{


// protected methods

// reimplemented (CGraphQlHandlerCompBase)

sdl::imtbase::CollectionDocumentManager::CDocumentList CCollectionDocumentManagerComp::OnGetOpenedDocumentList(
	const sdl::imtbase::CollectionDocumentManager::CGetOpenedDocumentListGqlRequest& getOpenedDocumentListRequest,
	const::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	CDM::CDocumentList retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->documentList.emplace();

	QByteArray userId = GetUserId(gqlRequest);

	if (!userId.isEmpty()){
		DocumentList list = BaseClass2::GetOpenedDocumentList(userId);
		for (const DocumentInfo& info : list){
			CDM::CDocumentInfo sdlInfo;
			sdlInfo.Version_1_0.emplace();

			sdlInfo.Version_1_0->documentId = info.documentId;
			sdlInfo.Version_1_0->objectId = info.objectId;
			sdlInfo.Version_1_0->hasChanges = info.hasChanges;

			retVal.Version_1_0->documentList->append(sdlInfo.Version_1_0);
		}
	}

	return retVal;
}


sdl::imtbase::CollectionDocumentManager::CDocumentId CCollectionDocumentManagerComp::OnCreateNewDocument(const sdl::imtbase::CollectionDocumentManager::CCreateNewDocumentGqlRequest& createNewDocumentRequest,const::imtgql::CGqlRequest& gqlRequest,QString& errorMessage) const
{
	CDM::CDocumentId retVal;

	const auto& arguments = createNewDocumentRequest.GetRequestedArguments();
	const auto& info = createNewDocumentRequest.GetRequestInfo();

	auto documentTypeId = arguments.input.Version_1_0;
	if (!documentTypeId || !documentTypeId->typeId){
		errorMessage = "Invalid GQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		errorMessage = "Unable to get user ID from context";

		return retVal;
	}

	QByteArray documentId = GetNonConstThis()->CreateNewDocument(userId, *documentTypeId->typeId);
	if (documentId.isEmpty()){
		errorMessage = "Unable to create document or undo manager";

		return retVal;
	}

	retVal.Version_1_0.emplace().id = documentId;

	return retVal;
}


sdl::imtbase::CollectionDocumentManager::CDocumentId CCollectionDocumentManagerComp::OnOpenDocument(
	const sdl::imtbase::CollectionDocumentManager::COpenDocumentGqlRequest& openDocumentRequest,
	const::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	CDM::CDocumentId retVal;

	const auto& arguments = openDocumentRequest.GetRequestedArguments();
	const auto& info = openDocumentRequest.GetRequestInfo();

	auto objectId = arguments.input.Version_1_0;
	if (!objectId || !objectId->id){
		errorMessage = "Invalid GQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		errorMessage = "Unable to get user ID from context";

		return retVal;
	}

	QByteArray documentId = GetNonConstThis()->OpenDocument(userId, *objectId->id);
	if (documentId.isEmpty()){
		errorMessage = "Unable to open document or create undo manager";

		return retVal;
	}

	retVal.Version_1_0.emplace().id = documentId;

	return retVal;
}


sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus CCollectionDocumentManagerComp::OnSaveDocument(
	const sdl::imtbase::CollectionDocumentManager::CSaveDocumentGqlRequest& saveDocumentRequest,
	const::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	CDM::CDocumentOperationStatus retVal;

	const auto& arguments = saveDocumentRequest.GetRequestedArguments();
	const auto& info = saveDocumentRequest.GetRequestInfo();

	auto documentId = arguments.input.Version_1_0;
	if (!documentId || !documentId->id){
		errorMessage = "Invalid GQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		errorMessage = "Unable to get user ID from context";

		return retVal;
	}

	retVal.Version_1_0.emplace();

	OperationStatus status = GetNonConstThis()->SaveDocument(userId, *documentId->id);
	switch (status){
	case imtdoc::ICollectionDocumentManager::OS_OK:
		retVal.Version_1_0->status = CDM::EDocumentOperationStatus::Success;
		break;
	case imtdoc::ICollectionDocumentManager::OS_INVALID_USER_ID:
		retVal.Version_1_0->status = CDM::EDocumentOperationStatus::InvalidUserId;
		break;
	case imtdoc::ICollectionDocumentManager::OS_INVALID_DOCUMENT_ID:
		retVal.Version_1_0->status = CDM::EDocumentOperationStatus::InvalidDocumentId;
		break;
	case imtdoc::ICollectionDocumentManager::OS_FAILED:
		retVal.Version_1_0->status = CDM::EDocumentOperationStatus::Failed;
		break;
	default:
		break;
	}

	if (status != OperationStatus::OS_OK){
		errorMessage = "Unable to open document or create undo manager";
	}

	return retVal;
}


sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus CCollectionDocumentManagerComp::OnCloseDocument(
	const sdl::imtbase::CollectionDocumentManager::CCloseDocumentGqlRequest& closeDocumentRequest,
	const::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	CDM::CDocumentOperationStatus retVal;

	const auto& arguments = closeDocumentRequest.GetRequestedArguments();
	const auto& info = closeDocumentRequest.GetRequestInfo();

	auto documentId = arguments.input.Version_1_0;
	if (!documentId || !documentId->id){
		errorMessage = "Invalid GQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		errorMessage = "Unable to get user ID from context";

		return retVal;
	}

	retVal.Version_1_0.emplace();

	OperationStatus status = GetNonConstThis()->CloseDocument(userId, *documentId->id);
	switch (status){
	case imtdoc::ICollectionDocumentManager::OS_OK:
		retVal.Version_1_0->status = CDM::EDocumentOperationStatus::Success;
		break;
	case imtdoc::ICollectionDocumentManager::OS_INVALID_USER_ID:
		retVal.Version_1_0->status = CDM::EDocumentOperationStatus::InvalidUserId;
		break;
	case imtdoc::ICollectionDocumentManager::OS_INVALID_DOCUMENT_ID:
		retVal.Version_1_0->status = CDM::EDocumentOperationStatus::InvalidDocumentId;
		break;
	case imtdoc::ICollectionDocumentManager::OS_FAILED:
		retVal.Version_1_0->status = CDM::EDocumentOperationStatus::Failed;
		break;
	default:
		break;
	}

	return retVal;
}


sdl::imtbase::UndoManager::CUndoInfo CCollectionDocumentManagerComp::OnGetUndoInfo(
	const sdl::imtbase::CollectionDocumentManager::CGetUndoInfoGqlRequest& getUndoInfoRequest,
	const::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	UM::CUndoInfo retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->status.emplace();

	const auto& arguments = getUndoInfoRequest.GetRequestedArguments();
	const auto& info = getUndoInfoRequest.GetRequestInfo();

	auto documentId = arguments.input.Version_1_0;
	if (!documentId || !documentId->id){
		errorMessage = "Invalid GQL request params";

		retVal.Version_1_0->status->status = UM::EUndoStatus::Failed;

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()){
		errorMessage = "Unable to get user ID from context";

		retVal.Version_1_0->status->status = UM::EUndoStatus::Failed;

		return retVal;
	}

	QMutexLocker locker(&m_mutex);
	if (!m_userDocuments.contains(userId)){
		errorMessage = "Invalid user ID";

		retVal.Version_1_0->status->status = UM::EUndoStatus::InvalidUserId;
	}

	if (!m_userDocuments[userId].contains(*documentId->id)){
		errorMessage = "Invalid document ID";

		retVal.Version_1_0->status->status = UM::EUndoStatus::InvalidDocumentId;
	}

	idoc::IUndoManager* undoManagerPtr = m_userDocuments[userId][*documentId->id].undoManagerPtr.GetPtr();

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

	retVal.Version_1_0->status->status = UM::EUndoStatus::Success;

	return retVal;
}


sdl::imtbase::UndoManager::CUndoStatus CCollectionDocumentManagerComp::OnDoUndo(
	const sdl::imtbase::CollectionDocumentManager::CDoUndoGqlRequest& doUndoRequest,
	const::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	UM::CUndoStatus retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->status.emplace();

	const auto& arguments = doUndoRequest.GetRequestedArguments();
	const auto& info = doUndoRequest.GetRequestInfo();

	istd::TSharedNullable<CDM::CCollectionUndoRedoInput::V1_0> collectionUndoRedoInput = arguments.input.Version_1_0;

	if (!collectionUndoRedoInput || !collectionUndoRedoInput->undoRedoInput){
		errorMessage = "Invalid GQL request params";
		
		return retVal;
	}

	istd::TSharedNullable<UM::CUndoRedoInput::V1_0> undoRedoInput = collectionUndoRedoInput->undoRedoInput;

	if (!undoRedoInput || !undoRedoInput->documentId || !undoRedoInput->steps){
		errorMessage = "Invalid GQL request params";

		retVal.Version_1_0->status = UM::EUndoStatus::InvalidDocumentId;

		return retVal;
	}

	QByteArray documentId = *undoRedoInput->documentId;
	QByteArray userId = GetUserId(gqlRequest);

	if (userId.isEmpty()){
		errorMessage = "Unable to get user ID from context";

		retVal.Version_1_0->status = UM::EUndoStatus::InvalidUserId;

		return retVal;
	}

	QMutexLocker locker(&m_mutex);

	idoc::IUndoManager* undoManagerPtr = nullptr;
	if (!m_userDocuments.contains(userId) || !m_userDocuments[userId].contains(documentId)){
		errorMessage = "Undo operation failed";

		retVal.Version_1_0->status = UM::EUndoStatus::Failed;

		return retVal;
	}

	undoManagerPtr = m_userDocuments[userId][documentId].undoManagerPtr.GetPtr();

	if (undoManagerPtr->GetAvailableUndoSteps() < *undoRedoInput->steps){
		errorMessage = "The number of available undo steps is less than requested";

		retVal.Version_1_0->status = UM::EUndoStatus::InvalidStepCount;

		return retVal;
	}

	if (!undoManagerPtr->DoUndo(*undoRedoInput->steps)){
		errorMessage = "Undo operation failed";

		retVal.Version_1_0->status = UM::EUndoStatus::Failed;

		return retVal;
	}

	retVal.Version_1_0->status = UM::EUndoStatus::Success;

	return retVal;
}


sdl::imtbase::UndoManager::CUndoStatus CCollectionDocumentManagerComp::OnDoRedo(
	const sdl::imtbase::CollectionDocumentManager::CDoRedoGqlRequest& doRedoRequest,
	const::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	UM::CUndoStatus retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->status.emplace();

	const auto& arguments = doRedoRequest.GetRequestedArguments();
	const auto& info = doRedoRequest.GetRequestInfo();

	istd::TSharedNullable<CDM::CCollectionUndoRedoInput::V1_0> collectionUndoRedoInput = arguments.input.Version_1_0;

	if (!collectionUndoRedoInput || !collectionUndoRedoInput->undoRedoInput){
		errorMessage = "Invalid GQL request params";

		return retVal;
	}

	istd::TSharedNullable<UM::CUndoRedoInput::V1_0> undoRedoInput = collectionUndoRedoInput->undoRedoInput;

	if (!undoRedoInput || !undoRedoInput->documentId || !undoRedoInput->steps){
		errorMessage = "Invalid GQL request params";

		retVal.Version_1_0->status = UM::EUndoStatus::InvalidDocumentId;

		return retVal;
	}

	QByteArray documentId = *undoRedoInput->documentId;
	QByteArray userId = GetUserId(gqlRequest);

	if (userId.isEmpty()){
		errorMessage = "Unable to get user ID from context";

		retVal.Version_1_0->status = UM::EUndoStatus::InvalidUserId;

		return retVal;
	}

	QMutexLocker locker(&m_mutex);

	idoc::IUndoManager* undoManagerPtr = nullptr;
	if (!m_userDocuments.contains(userId) || !m_userDocuments[userId].contains(documentId)){
		errorMessage = "Redo operation failed";

		retVal.Version_1_0->status = UM::EUndoStatus::Failed;

		return retVal;
	}

	undoManagerPtr = m_userDocuments[userId][documentId].undoManagerPtr.GetPtr();

	if (undoManagerPtr->GetAvailableRedoSteps() < *undoRedoInput->steps){
		errorMessage = "The number of available redo steps is less than requested";

		retVal.Version_1_0->status = UM::EUndoStatus::InvalidStepCount;

		return retVal;
	}

	if (!undoManagerPtr->DoRedo(*undoRedoInput->steps)){
		errorMessage = "Redo operation failed";

		retVal.Version_1_0->status = UM::EUndoStatus::Failed;

		return retVal;
	}

	retVal.Version_1_0->status = UM::EUndoStatus::Success;

	return retVal;
}


sdl::imtbase::UndoManager::CUndoStatus CCollectionDocumentManagerComp::OnResetUndo(
	const sdl::imtbase::CollectionDocumentManager::CResetUndoGqlRequest& resetUndoRequest,
	const::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	UM::CUndoStatus retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->status.emplace();

	const auto& arguments = resetUndoRequest.GetRequestedArguments();
	const auto& info = resetUndoRequest.GetRequestInfo();

	auto documentId = arguments.input.Version_1_0;
	if (!documentId || !documentId->id){
		errorMessage = "Invalid GQL request params";

		retVal.Version_1_0->status = UM::EUndoStatus::InvalidDocumentId;

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);

	if (userId.isEmpty()){
		errorMessage = "Unable to get user ID from context";

		retVal.Version_1_0->status = UM::EUndoStatus::InvalidUserId;

		return retVal;
	}

	QMutexLocker locker(&m_mutex);

	idoc::IUndoManager* undoManagerPtr = nullptr;
	if (!m_userDocuments.contains(userId) || !m_userDocuments[userId].contains(*documentId->id)){
		errorMessage = "Reset undo operation failed";

		retVal.Version_1_0->status = UM::EUndoStatus::Failed;

		return retVal;
	}

	undoManagerPtr = m_userDocuments[userId][*documentId->id].undoManagerPtr.GetPtr();
	undoManagerPtr->ResetUndo();

	retVal.Version_1_0->status = UM::EUndoStatus::Success;

	return retVal;
}


// reimplemented (imtdoc::CCollectionDocumentManager)

imtbase::IObjectCollection* CCollectionDocumentManagerComp::GetCollection() const
{
	if (m_collectionCompPtr.IsValid()){
		return m_collectionCompPtr.GetPtr();
	}

	return nullptr;
}


istd::IChangeableSharedPtr CCollectionDocumentManagerComp::CreateObject(const QByteArray& typeId) const
{
	int index = GetObjectFactoryIndex(typeId);

	if (index >= 0){
		return m_objectFactPtr.CreateInstance(index);
	}

	return nullptr;
}


idoc::IUndoManagerSharedPtr CCollectionDocumentManagerComp::CreateUndoManager() const
{
	return m_undoManagerFactPtr.CreateInstance();
}


// reimplemented (::imtservergql::CPermissibleGqlRequestHandlerComp)

bool CCollectionDocumentManagerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
	if (isSupported){
		const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
		if (inputParamPtr == nullptr){
			return false;
		}

		QByteArray collectionId = inputParamPtr->GetParamArgumentValue("collectionId").toByteArray();

		return collectionId == *m_collectionIdAttrPtr;
	}

	return false;
}


// private methods

imtdoc::CCollectionDocumentManager* CCollectionDocumentManagerComp::GetNonConstThis() const
{
	return const_cast<CCollectionDocumentManager*>(dynamic_cast<const CCollectionDocumentManager*>(this));
}


int CCollectionDocumentManagerComp::GetObjectFactoryIndex(const QByteArray& typeId) const
{
	int count = qMin(m_objectTypeIdAttrPtr.GetCount(), m_objectFactPtr.GetCount());
	for (int i = 0; i < count; i++){
		if (m_objectTypeIdAttrPtr[i] == typeId){
			return i;
		}
	}

	return -1;
}


QByteArray CCollectionDocumentManagerComp::GetUserId(const ::imtgql::CGqlRequest& gqlRequest) const
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr != nullptr){
		imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			return userInfoPtr->GetId();
		}
	}

	return QByteArray("Test");
}


} // namespace imtservergql

