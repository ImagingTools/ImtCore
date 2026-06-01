// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <idoc/IUndoManager.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtgql/CGqlRequest.h>
#include <imtdoc/TCollectionDocumentServiceWrap.h>
#include <imtbasesdl/SDL/1.0/CPP/DocumentService_fwd.h>
#include <imtbasesdl/SDL/1.0/CPP/UndoManager_fwd.h>


namespace imtservergql
{


namespace CDM = sdl::V1_0::imtbase;
namespace UM = sdl::V1_0::imtbase;


template<class Base, class ColorCollectionDocumentServiceDefs>
class TCollectionDocumentServiceCompBase : public imtdoc::TCollectionDocumentServiceWrap<Base>
{
public:
	typedef imtdoc::TCollectionDocumentServiceWrap<Base> BaseClass;
	typedef ColorCollectionDocumentServiceDefs Defs;

	using OperationStatus = typename BaseClass::OperationStatus;
	using DocumentList = typename BaseClass::DocumentList;
	using DocumentListItem = typename BaseClass::DocumentListItem;
	using DocumentInfo = typename BaseClass::DocumentInfo;

	I_BEGIN_BASE_COMPONENT(TCollectionDocumentServiceCompBase)
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "Collection ID", true, "CollectiondId");
		I_ASSIGN_MULTI_0(m_objectTypeIdAttrPtr, "ObjectTypeId", "Object type ID", false);
		I_ASSIGN(m_collectionCompPtr, "Collection", "Collection", false, "");
		I_ASSIGN(m_undoManagerFactPtr, "UndoManager", "Undo manager", false, "UndoManager");
		I_ASSIGN_MULTI_0(m_objectFactPtr, "ObjectFactory", "Object factory", false);
	I_END_COMPONENT

	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override;

protected:
	// reimplemented (CCollectionDocumentServiceGqlHandlerCompBase)
	CDM::CDocumentList OnGetOpenedDocumentList(
				const typename Defs::GetOpenedDocumentListGqlRequest& getDocumentListRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	CDM::CDocumentInfo OnCreateNewDocument(
				const typename Defs::CreateNewDocumentGqlRequest& createDocumentRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	CDM::CDocumentInfo OnOpenDocument(
				const typename Defs::OpenDocumentGqlRequest& openDocumentRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	CDM::CDocumentOperationStatus OnSaveDocument(
				const typename Defs::SaveDocumentGqlRequest& saveDocumentRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	CDM::CDocumentOperationStatus OnCloseDocument(
				const typename Defs::CloseDocumentGqlRequest& closeDocumentRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	UM::CUndoInfo OnGetUndoInfo(
				const typename Defs::GetUndoInfoGqlRequest& getUndoInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	UM::CUndoStatus OnDoUndo(
				const typename Defs::DoUndoGqlRequest& doUndoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	UM::CUndoStatus OnDoRedo(
				const typename Defs::DoRedoGqlRequest& doRedoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	UM::CUndoStatus OnResetUndo(
				const typename Defs::ResetUndoGqlRequest& resetUndoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	BaseClass* GetNonConstThis() const;
	int GetObjectFactoryIndex(const QByteArray& typeId) const;
	QByteArray GetUserId(const ::imtgql::CGqlRequest& gqlRequest) const;

	// reimplemented (imtdoc::TCollectionDocumentServiceWrap)
	virtual imtbase::IObjectCollection* GetCollection() const override;
	virtual istd::IChangeableUniquePtr CreateObject(const QByteArray& typeId) const override;
	virtual idoc::IUndoManagerUniquePtr CreateUndoManager() const override;

	// reimplemented (::imtservergql::CPermissibleGqlRequestHandlerComp)
	bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

protected:
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
	I_MULTIATTR(QByteArray, m_objectTypeIdAttrPtr);
	I_REF(imtbase::IObjectCollection, m_collectionCompPtr);
	I_FACT(idoc::IUndoManager, m_undoManagerFactPtr);
	I_MULTIFACT(istd::IChangeable, m_objectFactPtr);
};


// protected methods

// reimplemented (icomp::CComponentBase)

template<class Base, class ColorCollectionDocumentServiceDefs>
inline void TCollectionDocumentServiceCompBase<Base, ColorCollectionDocumentServiceDefs>::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	Q_ASSERT(!(*m_collectionIdAttrPtr).isEmpty());
	Q_ASSERT(m_collectionCompPtr.IsValid());
	Q_ASSERT(m_undoManagerFactPtr.IsValid());
}


// protected methods

template<class Base, class ColorCollectionDocumentServiceDefs>
inline typename TCollectionDocumentServiceCompBase<Base, ColorCollectionDocumentServiceDefs>::BaseClass*
TCollectionDocumentServiceCompBase<
			Base,
			ColorCollectionDocumentServiceDefs>::GetNonConstThis() const
{
	return const_cast<BaseClass*>(static_cast<const BaseClass*>(this));
}


template<class Base, class ColorCollectionDocumentServiceDefs>
inline int TCollectionDocumentServiceCompBase<Base, ColorCollectionDocumentServiceDefs>::GetObjectFactoryIndex(
			const QByteArray& typeId) const
{
	int count = qMin(m_objectTypeIdAttrPtr.GetCount(), m_objectFactPtr.GetCount());
	for (int i = 0; i < count; i++) {
		if (m_objectTypeIdAttrPtr[i] == typeId) {
			return i;
		}
	}

	return -1;
}


template<class Base, class ColorCollectionDocumentServiceDefs>
inline QByteArray TCollectionDocumentServiceCompBase<Base, ColorCollectionDocumentServiceDefs>::GetUserId(
			const ::imtgql::CGqlRequest& gqlRequest) const
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr != nullptr) {
		imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
		if (userInfoPtr != nullptr) {
			return userInfoPtr->GetId();
		}
	}

	return QByteArray();
}


// reimplemented (imtdoc::TCollectionDocumentServiceWrap)

template<class Base, class ColorCollectionDocumentServiceDefs>
inline imtbase::IObjectCollection* TCollectionDocumentServiceCompBase<Base, ColorCollectionDocumentServiceDefs>::
			GetCollection() const
{
	if (m_collectionCompPtr.IsValid()) {
		return m_collectionCompPtr.GetPtr();
	}

	return nullptr;
}


template<class Base, class ColorCollectionDocumentServiceDefs>
inline istd::IChangeableUniquePtr TCollectionDocumentServiceCompBase<Base, ColorCollectionDocumentServiceDefs>::
			CreateObject(const QByteArray& typeId) const
{
	int index = GetObjectFactoryIndex(typeId);

	if (index >= 0) {
		return m_objectFactPtr.CreateInstance(index);
	}

	return nullptr;
}


template<class Base, class ColorCollectionDocumentServiceDefs>
inline idoc::IUndoManagerUniquePtr TCollectionDocumentServiceCompBase<Base, ColorCollectionDocumentServiceDefs>::
			CreateUndoManager() const
{
	return m_undoManagerFactPtr.CreateInstance();
}


// reimplemented (::imtservergql::CPermissibleGqlRequestHandlerComp)

template<class Base, class ColorCollectionDocumentServiceDefs>
inline bool TCollectionDocumentServiceCompBase<Base, ColorCollectionDocumentServiceDefs>::IsRequestSupported(
			const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
	if (isSupported) {
		const imtgql::CGqlParamObject* collectionIdParamPtr = gqlRequest.GetParamObject("collectionId");
		if (collectionIdParamPtr == nullptr) {
			return false;
		}

		QByteArray collectionId = collectionIdParamPtr->GetParamArgumentValue("collectionId").toByteArray();

		return collectionId == *m_collectionIdAttrPtr;
	}

	return false;
}


// reimplemented (CCollectionDocumentServiceGqlHandlerCompBase)

template<class Base, class ColorCollectionDocumentServiceDefs>
inline CDM::CDocumentList TCollectionDocumentServiceCompBase<Base, ColorCollectionDocumentServiceDefs>::
			OnGetOpenedDocumentList(
				const typename Defs::GetOpenedDocumentListGqlRequest& getOpenedDocumentListRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const
{
	CDM::CDocumentList retVal;
	retVal.documentList.emplace();

	QByteArray userId = GetUserId(gqlRequest);

	if (!userId.isEmpty()) {
		DocumentList list = BaseClass::GetOpenedDocumentList(userId);
		for (const DocumentInfo& info : list) {
			CDM::CDocumentInfo sdlInfo;
			sdlInfo.documentId = info.documentId;
			sdlInfo.objectId = info.objectId;
			sdlInfo.hasChanges = info.hasChanges;

			retVal.documentList->append(sdlInfo);
		}
	}

	return retVal;
}

template<class Base, class ColorCollectionDocumentServiceDefs>
inline CDM::CDocumentInfo TCollectionDocumentServiceCompBase<Base, ColorCollectionDocumentServiceDefs>::
			OnCreateNewDocument(
				const typename Defs::CreateNewDocumentGqlRequest& createDocumentRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const
{
	CDM::CDocumentInfo retVal;

	const auto& arguments = createDocumentRequest.GetRequestedArguments();
	const auto& info = createDocumentRequest.GetRequestInfo();

	auto documentTypeId = arguments.input;
	if (!documentTypeId || documentTypeId->typeId) {
		errorMessage = "Invalid GQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()) {
		errorMessage = "Unable to get user ID from context";

		return retVal;
	}

	typename BaseClass::TaskParams taskParams;
	taskParams.userId = userId;
	taskParams.documentTypeId = *documentTypeId->typeId;
	QByteArray taskId = GetNonConstThis()->BeginDocumentTask(BaseClass::TT_NEW, taskParams);
	typename BaseClass::TaskResult taskResult = GetNonConstThis()->WaitForTaskFinished(taskId);
	QByteArray documentId = taskResult.documentId;
	if (documentId.isEmpty()) {
		errorMessage = taskResult.errorMessage.isEmpty()
			? "Unable to create document or undo manager"
			: taskResult.errorMessage;

		return retVal;
	}

	retVal.documentId = documentId;
	retVal.objectTypeId = *documentTypeId->typeId;
	retVal.objectId = QByteArray();
	retVal.isDirty = false;
	retVal.hasNameProvider = false;
	retVal.isLoading = false;

	DocumentList list = BaseClass::GetOpenedDocumentList(userId);
	for (const DocumentListItem& docInfo : list) {
		if (docInfo.documentId == documentId) {
			retVal.documentName = docInfo.name;
			retVal.hasNameProvider = docInfo.hasNameProvider;
			break;
		}
	}

	return retVal;
}


template<class Base, class ColorCollectionDocumentServiceDefs>
inline CDM::CDocumentInfo TCollectionDocumentServiceCompBase<Base, ColorCollectionDocumentServiceDefs>::OnOpenDocument(
			const typename Defs::OpenDocumentGqlRequest& openDocumentRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	CDM::CDocumentInfo retVal;

	const auto& arguments = openDocumentRequest.GetRequestedArguments();
	const auto& info = openDocumentRequest.GetRequestInfo();

	auto objectId = arguments.input;
	if (!objectId || objectId->id) {
		errorMessage = "Invalid GQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()) {
		errorMessage = "Unable to get user ID from context";

		return retVal;
	}

	typename BaseClass::TaskParams taskParams;
	taskParams.userId = userId;
	taskParams.url = *objectId->id;
	QByteArray taskId = GetNonConstThis()->BeginDocumentTask(BaseClass::TT_OPEN, taskParams);
	typename BaseClass::TaskResult taskResult = GetNonConstThis()->WaitForTaskFinished(taskId);
	QByteArray documentId = taskResult.documentId;
	if (documentId.isEmpty()) {
		errorMessage = taskResult.errorMessage.isEmpty()
			? "Unable to open document or create undo manager"
			: taskResult.errorMessage;

		return retVal;
	}

	retVal.documentId = documentId;
	retVal.objectId = *objectId->id;
	retVal.isDirty = false;
	retVal.hasNameProvider = false;
	retVal.isLoading = true;

	DocumentList list = BaseClass::GetOpenedDocumentList(userId);
	for (const DocumentListItem& docInfo : list) {
		if (docInfo.documentId == documentId) {
			retVal.documentName = docInfo.name;
			retVal.objectTypeId = docInfo.typeId;
			retVal.hasNameProvider = docInfo.hasNameProvider;
			retVal.isLoading = docInfo.isLoading;
			break;
		}
	}

	return retVal;
}

template<class Base, class ColorCollectionDocumentServiceDefs>
inline CDM::CDocumentOperationStatus TCollectionDocumentServiceCompBase<Base, ColorCollectionDocumentServiceDefs>::
			OnSaveDocument(
				const typename Defs::SaveDocumentGqlRequest& saveDocumentRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const
{
	CDM::CDocumentOperationStatus retVal;

	const auto& arguments = saveDocumentRequest.GetRequestedArguments();
	const auto& info = saveDocumentRequest.GetRequestInfo();

	auto documentId = arguments.input;
	if (!documentId || !documentId->id) {
		errorMessage = "Invalid GQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()) {
		errorMessage = "Unable to get user ID from context";

		return retVal;
	}

	typename BaseClass::TaskParams taskParams;
	taskParams.userId = userId;
	taskParams.documentId = *documentId->id;
	taskParams.documentName = QString();
	QByteArray taskId = GetNonConstThis()->BeginDocumentTask(BaseClass::TT_SAVE, taskParams);
	typename BaseClass::TaskResult taskResult = GetNonConstThis()->WaitForTaskFinished(taskId);
	QString saveErrorMessage = taskResult.errorMessage;
	OperationStatus status = taskResult.status;
	switch (status) {
	case imtdoc::ICollectionDocumentService::OS_OK:
		retVal.status = CDM::EDocumentOperationStatus::Success;
		{
			QString resolvedName;
			if (GetNonConstThis()->GetDocumentName(userId, *documentId->id, resolvedName) == OperationStatus::OS_OK){
				retVal.documentName = resolvedName;
			}
		}
		break;
	case imtdoc::ICollectionDocumentService::OS_INVALID_USER_ID:
		retVal.status = CDM::EDocumentOperationStatus::InvalidUserId;
		break;
	case imtdoc::ICollectionDocumentService::OS_INVALID_DOCUMENT_ID:
		retVal.status = CDM::EDocumentOperationStatus::InvalidDocumentId;
		break;
	case imtdoc::ICollectionDocumentService::OS_INVALID_DOCUMENT_DATA:
		retVal.status = CDM::EDocumentOperationStatus::InvalidDocumentData;
		break;
	case imtdoc::ICollectionDocumentService::OS_FAILED:
		retVal.status = CDM::EDocumentOperationStatus::Failed;
		break;
	default:
		break;
	}

	if (status != OperationStatus::OS_OK) {
		QString responseMessage;
		switch (status) {
		case imtdoc::ICollectionDocumentService::OS_INVALID_USER_ID:
			responseMessage = "Invalid user ID";
			break;
		case imtdoc::ICollectionDocumentService::OS_INVALID_DOCUMENT_ID:
			responseMessage = "Invalid document ID";
			break;
		case imtdoc::ICollectionDocumentService::OS_INVALID_DOCUMENT_DATA:
			responseMessage = saveErrorMessage.isEmpty() ? "Document data is invalid" : saveErrorMessage;
			break;
		case imtdoc::ICollectionDocumentService::OS_FAILED:
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


template<class Base, class ColorCollectionDocumentServiceDefs>
inline CDM::CDocumentOperationStatus TCollectionDocumentServiceCompBase<Base, ColorCollectionDocumentServiceDefs>::
			OnCloseDocument(
				const typename Defs::CloseDocumentGqlRequest& closeDocumentRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const
{
	CDM::CDocumentOperationStatus retVal;

	const auto& arguments = closeDocumentRequest.GetRequestedArguments();
	const auto& info = closeDocumentRequest.GetRequestInfo();

	auto documentId = arguments.input;
	if (!documentId || !documentId->id) {
		errorMessage = "Invalid GQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()) {
		errorMessage = "Unable to get user ID from context";

		return retVal;
	}

	typename BaseClass::TaskParams taskParams;
	taskParams.userId = userId;
	taskParams.documentId = *documentId->id;
	QByteArray taskId = GetNonConstThis()->BeginDocumentTask(BaseClass::TT_CLOSE, taskParams);
	typename BaseClass::TaskResult taskResult = GetNonConstThis()->WaitForTaskFinished(taskId);
	OperationStatus status = taskResult.status;
	switch (status) {
	case imtdoc::ICollectionDocumentService::OS_OK:
		retVal.status = CDM::EDocumentOperationStatus::Success;
		break;
	case imtdoc::ICollectionDocumentService::OS_INVALID_USER_ID:
		retVal.status = CDM::EDocumentOperationStatus::InvalidUserId;
		break;
	case imtdoc::ICollectionDocumentService::OS_INVALID_DOCUMENT_ID:
		retVal.status = CDM::EDocumentOperationStatus::InvalidDocumentId;
		break;
	case imtdoc::ICollectionDocumentService::OS_FAILED:
		retVal.status = CDM::EDocumentOperationStatus::Failed;
		break;
	default:
		break;
	}

	return retVal;
}


template<class Base, class ColorCollectionDocumentServiceDefs>
inline UM::CUndoInfo TCollectionDocumentServiceCompBase<Base, ColorCollectionDocumentServiceDefs>::OnGetUndoInfo(
			const typename Defs::GetUndoInfoGqlRequest& getUndoInfoRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	UM::CUndoInfo retVal;
	retVal.status.emplace();

	const auto& arguments = getUndoInfoRequest.GetRequestedArguments();
	const auto& info = getUndoInfoRequest.GetRequestInfo();

	auto documentId = arguments.input;
	if (!documentId || !documentId->id) {
		errorMessage = "Invalid GQL request params";

		retVal.status->status = UM::EUndoStatus::InvalidDocumentId;

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()) {
		errorMessage = "Unable to get user ID from context";

		retVal.status->status = UM::EUndoStatus::InvalidUserId;

		return retVal;
	}

	QMutexLocker locker(&m_mutex);
	if (!m_userDocuments.contains(userId) || !m_userDocuments[userId].contains(*documentId->id)) {
		idoc::IUndoManager* undoManagerPtr = m_userDocuments[userId][*documentId->id].undoManagerPtr.GetPtr();

		int count = undoManagerPtr->GetAvailableUndoSteps();
		retVal.availableUndoSteps = count;
		retVal.undoLevelDescriptions.emplace();
		for (int i = 0; i < count; i++) {
			QString description = undoManagerPtr->GetUndoLevelDescription(i);
			retVal.undoLevelDescriptions->append(description);
		}

		count = undoManagerPtr->GetAvailableRedoSteps();
		retVal.availableRedoSteps = count;
		retVal.redoLevelDescriptions.emplace();
		for (int i = 0; i < count; i++) {
			QString description = undoManagerPtr->GetRedoLevelDescription(i);
			retVal.redoLevelDescriptions->append(description);
		}
	}

	retVal.status->status = UM::EUndoStatus::Success;

	return retVal;
}


template<class Base, class ColorCollectionDocumentServiceDefs>
inline UM::CUndoStatus TCollectionDocumentServiceCompBase<Base, ColorCollectionDocumentServiceDefs>::OnDoUndo(
			const typename Defs::DoUndoGqlRequest& doUndoRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	UM::CUndoStatus retVal;
	retVal.status.emplace();

	const auto& arguments = doUndoRequest.GetRequestedArguments();
	const auto& info = doUndoRequest.GetRequestInfo();

	istd::TNullableValue<UM::CUndoRedoInput> undoRedoInput = arguments.input;
	if (!undoRedoInput || !undoRedoInput->documentId || !undoRedoInput->steps) {
		errorMessage = "Invalid GQL request params";

		retVal.status = UM::EUndoStatus::InvalidDocumentId;

		return retVal;
	}

	QByteArray documentId = *undoRedoInput->documentId;
	QByteArray userId = GetUserId(gqlRequest);

	if (userId.isEmpty()) {
		errorMessage = "Unable to get user ID from context";

		retVal.status = UM::EUndoStatus::InvalidUserId;

		return retVal;
	}

	QMutexLocker locker(&m_mutex);

	idoc::IUndoManager* undoManagerPtr = nullptr;
	if (!m_userDocuments.contains(userId) || !m_userDocuments[userId].contains(documentId)) {
		errorMessage = "Undo operation failed";

		retVal.status = UM::EUndoStatus::Failed;

		return retVal;
	}

	undoManagerPtr = m_userDocuments[userId][documentId].undoManagerPtr.GetPtr();

	if (undoManagerPtr->GetAvailableUndoSteps() < *undoRedoInput->steps) {
		errorMessage = "The number of available undo steps is less than requested";

		retVal.status = UM::EUndoStatus::InvalidStepCount;

		return retVal;
	}

	if (!undoManagerPtr->DoUndo(*undoRedoInput->steps)) {
		errorMessage = "Undo operation failed";

		retVal.status = UM::EUndoStatus::Failed;

		return retVal;
	}

	retVal.status = UM::EUndoStatus::Success;

	return retVal;
}


template<class Base, class ColorCollectionDocumentServiceDefs>
inline UM::CUndoStatus TCollectionDocumentServiceCompBase<Base, ColorCollectionDocumentServiceDefs>::OnDoRedo(
			const typename Defs::DoRedoGqlRequest& doRedoRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	UM::CUndoStatus retVal;
	retVal.status.emplace();

	const auto& arguments = doRedoRequest.GetRequestedArguments();
	const auto& info = doRedoRequest.GetRequestInfo();

	istd::TNullableValue<UM::CUndoRedoInput> undoRedoInput = arguments.input;
	if (!undoRedoInput || !undoRedoInput->documentId || !undoRedoInput->steps) {
		errorMessage = "Invalid GQL request params";

		retVal.status = UM::EUndoStatus::InvalidDocumentId;

		return retVal;
	}

	QByteArray documentId = *undoRedoInput->documentId;
	QByteArray userId = GetUserId(gqlRequest);

	if (userId.isEmpty()) {
		errorMessage = "Unable to get user ID from context";

		retVal.status = UM::EUndoStatus::InvalidUserId;

		return retVal;
	}

	QMutexLocker locker(&m_mutex);

	idoc::IUndoManager* undoManagerPtr = nullptr;
	if (!m_userDocuments.contains(userId) || !m_userDocuments[userId].contains(documentId)) {
		errorMessage = "Redo operation failed";

		retVal.status = UM::EUndoStatus::Failed;

		return retVal;
	}

	undoManagerPtr = m_userDocuments[userId][documentId].undoManagerPtr.GetPtr();

	if (undoManagerPtr->GetAvailableRedoSteps() < *undoRedoInput->steps) {
		errorMessage = "The number of available redo steps is less than requested";

		retVal.status = UM::EUndoStatus::InvalidStepCount;

		return retVal;
	}

	if (!undoManagerPtr->DoRedo(*undoRedoInput->steps)) {
		errorMessage = "Redo operation failed";

		retVal.status = UM::EUndoStatus::Failed;

		return retVal;
	}

	retVal.status = UM::EUndoStatus::Success;

	return retVal;
}


template<class Base, class ColorCollectionDocumentServiceDefs>
inline UM::CUndoStatus TCollectionDocumentServiceCompBase<Base, ColorCollectionDocumentServiceDefs>::OnResetUndo(
			const typename Defs::ResetUndoGqlRequest& resetUndoRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	UM::CUndoStatus retVal;
	retVal.status.emplace();

	const auto& arguments = resetUndoRequest.GetRequestedArguments();
	const auto& info = resetUndoRequest.GetRequestInfo();

	auto documentId = arguments.input;
	if (!documentId || !documentId->id) {
		errorMessage = "Invalid GQL request params";

		retVal.status = UM::EUndoStatus::InvalidDocumentId;

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);

	if (userId.isEmpty()) {
		errorMessage = "Unable to get user ID from context";

		retVal.status = UM::EUndoStatus::InvalidUserId;

		return retVal;
	}

	QMutexLocker locker(&m_mutex);

	idoc::IUndoManager* undoManagerPtr = nullptr;
	if (!m_userDocuments.contains(userId) || !m_userDocuments[userId].contains(*documentId->id)) {
		errorMessage = "Reset undo operation failed";

		retVal.status = UM::EUndoStatus::Failed;

		return retVal;
	}

	undoManagerPtr = m_userDocuments[userId][*documentId->id].undoManagerPtr.GetPtr();
	undoManagerPtr->ResetUndo();

	retVal.status = UM::EUndoStatus::Success;

	return retVal;
}


} // namespace imtservergql
