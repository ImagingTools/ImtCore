// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <idoc/IDocumentManager.h>
#include <idoc/IUndoManager.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtgql/CGqlRequest.h>
#include <imtdoc/CCollectionDocumentManager.h>
#include <imtbasesdl/SDL/1.0/CPP/DocumentManager.h>
#include <imtbasesdl/SDL/1.0/CPP/UndoManager.h>


namespace imtservergql
{


namespace CDM = sdl::imtbase::DocumentManager;
namespace UM = sdl::imtbase::UndoManager;


template<class Base, class ColorCollectionDocumentManagerDefs>
class TCollectionDocumentManagerCompBase : public Base, public imtdoc::CCollectionDocumentManager
{
public:
	typedef Base BaseClass;
	typedef imtdoc::CCollectionDocumentManager BaseClass2;
	typedef ColorCollectionDocumentManagerDefs Defs;

	I_BEGIN_BASE_COMPONENT(TCollectionDocumentManagerCompBase)
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "Collection ID", true, "CollectiondId");
		I_ASSIGN_MULTI_0(m_objectTypeIdAttrPtr, "ObjectTypeId", "Object type ID", false);
		I_ASSIGN(m_collectionCompPtr, "Collection", "Collection", false, "");
		I_ASSIGN(m_undoManagerFactPtr, "UndoManager", "Undo manager", false, "UndoManager");
		I_ASSIGN_MULTI_0(m_objectFactPtr, "ObjectFactory", "Object factory", false);
	I_END_COMPONENT

	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override;

protected:
	// reimplemented (CGraphQlHandlerCompBase)
	CDM::CDocumentList OnGetOpenedDocumentList(
		const typename Defs::GetOpenedDocumentListGqlRequest& getDocumentListRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	CDM::CDocumentId OnCreateNewDocument(
		const typename Defs::CreateNewDocumentGqlRequest& createDocumentRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	CDM::CDocumentId OnOpenDocument(
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

	imtdoc::CCollectionDocumentManager* GetNonConstThis() const;
	int GetObjectFactoryIndex(const QByteArray& typeId) const;
	QByteArray GetUserId(const ::imtgql::CGqlRequest& gqlRequest) const;

	// reimplemented (imtdoc::CCollectionDocumentManager)
	virtual imtbase::IObjectCollection* GetCollection() const override;
	virtual istd::IChangeableSharedPtr CreateObject(const QByteArray& typeId) const override;
	virtual idoc::IUndoManagerSharedPtr CreateUndoManager() const override;

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

template<class Base, class ColorCollectionDocumentManagerDefs>
inline void TCollectionDocumentManagerCompBase<Base, ColorCollectionDocumentManagerDefs>::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	Q_ASSERT(!(*m_collectionIdAttrPtr).isEmpty());
	Q_ASSERT(m_collectionCompPtr.IsValid());
	Q_ASSERT(m_undoManagerFactPtr.IsValid());
}


// protected methods

template<class Base, class ColorCollectionDocumentManagerDefs>
inline imtdoc::CCollectionDocumentManager* TCollectionDocumentManagerCompBase<
	Base,
	ColorCollectionDocumentManagerDefs>::GetNonConstThis() const
{
	return const_cast<CCollectionDocumentManager*>(dynamic_cast<const CCollectionDocumentManager*>(this));
}


template<class Base, class ColorCollectionDocumentManagerDefs>
inline int TCollectionDocumentManagerCompBase<Base, ColorCollectionDocumentManagerDefs>::GetObjectFactoryIndex(
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


template<class Base, class ColorCollectionDocumentManagerDefs>
inline QByteArray TCollectionDocumentManagerCompBase<Base, ColorCollectionDocumentManagerDefs>::GetUserId(
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


// reimplemented (imtdoc::CCollectionDocumentManager)

template<class Base, class ColorCollectionDocumentManagerDefs>
inline imtbase::IObjectCollection* TCollectionDocumentManagerCompBase<Base, ColorCollectionDocumentManagerDefs>::
	GetCollection() const
{
	if (m_collectionCompPtr.IsValid()) {
		return m_collectionCompPtr.GetPtr();
	}

	return nullptr;
}


template<class Base, class ColorCollectionDocumentManagerDefs>
inline istd::IChangeableSharedPtr TCollectionDocumentManagerCompBase<Base, ColorCollectionDocumentManagerDefs>::
	CreateObject(const QByteArray& typeId) const
{
	int index = GetObjectFactoryIndex(typeId);

	if (index >= 0) {
		return m_objectFactPtr.CreateInstance(index);
	}

	return nullptr;
}


template<class Base, class ColorCollectionDocumentManagerDefs>
inline idoc::IUndoManagerSharedPtr TCollectionDocumentManagerCompBase<Base, ColorCollectionDocumentManagerDefs>::
	CreateUndoManager() const
{
	return m_undoManagerFactPtr.CreateInstance();
}


// reimplemented (::imtservergql::CPermissibleGqlRequestHandlerComp)

template<class Base, class ColorCollectionDocumentManagerDefs>
inline bool TCollectionDocumentManagerCompBase<Base, ColorCollectionDocumentManagerDefs>::IsRequestSupported(
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


// reimplemented (CGraphQlHandlerCompBase)

template<class Base, class ColorCollectionDocumentManagerDefs>
inline CDM::CDocumentList TCollectionDocumentManagerCompBase<Base, ColorCollectionDocumentManagerDefs>::
	OnGetOpenedDocumentList(
		const typename Defs::GetOpenedDocumentListGqlRequest& getOpenedDocumentListRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	CDM::CDocumentList retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->documentList.emplace();

	QByteArray userId = GetUserId(gqlRequest);

	if (!userId.isEmpty()) {
		DocumentList list = BaseClass2::GetOpenedDocumentList(userId);
		for (const DocumentInfo& info : list) {
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

template<class Base, class ColorCollectionDocumentManagerDefs>
inline CDM::CDocumentId TCollectionDocumentManagerCompBase<Base, ColorCollectionDocumentManagerDefs>::
	OnCreateNewDocument(
		const typename Defs::CreateNewDocumentGqlRequest& createDocumentRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	CDM::CDocumentId retVal;

	const auto& arguments = createDocumentRequest.GetRequestedArguments();
	const auto& info = createDocumentRequest.GetRequestInfo();

	auto documentTypeId = arguments.input.Version_1_0;
	if (!documentTypeId || documentTypeId->typeId) {
		errorMessage = "Invalid GQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()) {
		errorMessage = "Unable to get user ID from context";

		return retVal;
	}

	QByteArray documentId = GetNonConstThis()->CreateNewDocument(userId, *documentTypeId->typeId);
	if (documentId.isEmpty()) {
		errorMessage = "Unable to create document or undo manager";

		return retVal;
	}

	retVal.Version_1_0.emplace().id = documentId;

	return retVal;
}


template<class Base, class ColorCollectionDocumentManagerDefs>
inline CDM::CDocumentId TCollectionDocumentManagerCompBase<Base, ColorCollectionDocumentManagerDefs>::OnOpenDocument(
	const typename Defs::OpenDocumentGqlRequest& openDocumentRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	CDM::CDocumentId retVal;

	const auto& arguments = openDocumentRequest.GetRequestedArguments();
	const auto& info = openDocumentRequest.GetRequestInfo();

	auto objectId = arguments.input.Version_1_0;
	if (!objectId || objectId->id) {
		errorMessage = "Invalid GQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()) {
		errorMessage = "Unable to get user ID from context";

		return retVal;
	}

	QByteArray documentId = GetNonConstThis()->OpenDocument(userId, *objectId->id);
	if (documentId.isEmpty()) {
		errorMessage = "Unable to open document or create undo manager";

		return retVal;
	}

	retVal.Version_1_0.emplace().id = documentId;

	return retVal;
}

template<class Base, class ColorCollectionDocumentManagerDefs>
inline CDM::CDocumentOperationStatus TCollectionDocumentManagerCompBase<Base, ColorCollectionDocumentManagerDefs>::
	OnSaveDocument(
		const typename Defs::SaveDocumentGqlRequest& saveDocumentRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	CDM::CDocumentOperationStatus retVal;

	const auto& arguments = saveDocumentRequest.GetRequestedArguments();
	const auto& info = saveDocumentRequest.GetRequestInfo();

	auto documentId = arguments.input.Version_1_0;
	if (!documentId || !documentId->id) {
		errorMessage = "Invalid GQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()) {
		errorMessage = "Unable to get user ID from context";

		return retVal;
	}

	retVal.Version_1_0.emplace();

	OperationStatus status = GetNonConstThis()->SaveDocument(userId, *documentId->id);
	switch (status) {
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

	if (status != OperationStatus::OS_OK) {
		errorMessage = "Unable to open document or create undo manager";
	}

	return retVal;
}


template<class Base, class ColorCollectionDocumentManagerDefs>
inline CDM::CDocumentOperationStatus TCollectionDocumentManagerCompBase<Base, ColorCollectionDocumentManagerDefs>::
	OnCloseDocument(
		const typename Defs::CloseDocumentGqlRequest& closeDocumentRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	CDM::CDocumentOperationStatus retVal;

	const auto& arguments = closeDocumentRequest.GetRequestedArguments();
	const auto& info = closeDocumentRequest.GetRequestInfo();

	auto documentId = arguments.input.Version_1_0;
	if (!documentId || !documentId->id) {
		errorMessage = "Invalid GQL request params";

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()) {
		errorMessage = "Unable to get user ID from context";

		return retVal;
	}

	retVal.Version_1_0.emplace();

	OperationStatus status = GetNonConstThis()->CloseDocument(userId, *documentId->id);
	switch (status) {
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


template<class Base, class ColorCollectionDocumentManagerDefs>
inline UM::CUndoInfo TCollectionDocumentManagerCompBase<Base, ColorCollectionDocumentManagerDefs>::OnGetUndoInfo(
	const typename Defs::GetUndoInfoGqlRequest& getUndoInfoRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	UM::CUndoInfo retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->status.emplace();

	const auto& arguments = getUndoInfoRequest.GetRequestedArguments();
	const auto& info = getUndoInfoRequest.GetRequestInfo();

	auto documentId = arguments.input.Version_1_0;
	if (!documentId || !documentId->id) {
		errorMessage = "Invalid GQL request params";

		retVal.Version_1_0->status->status = UM::EUndoStatus::InvalidDocumentId;

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);
	if (userId.isEmpty()) {
		errorMessage = "Unable to get user ID from context";

		retVal.Version_1_0->status->status = UM::EUndoStatus::InvalidUserId;

		return retVal;
	}

	QMutexLocker locker(&m_mutex);
	if (!m_userDocuments.contains(userId) || !m_userDocuments[userId].contains(*documentId->id)) {
		idoc::IUndoManager* undoManagerPtr = m_userDocuments[userId][*documentId->id].undoManagerPtr.GetPtr();

		int count = undoManagerPtr->GetAvailableUndoSteps();
		retVal.Version_1_0->availableUndoSteps = count;
		retVal.Version_1_0->undoLevelDescriptions.emplace();
		for (int i = 0; i < count; i++) {
			QString description = undoManagerPtr->GetUndoLevelDescription(i);
			retVal.Version_1_0->undoLevelDescriptions->append(description);
		}

		count = undoManagerPtr->GetAvailableRedoSteps();
		retVal.Version_1_0->availableRedoSteps = count;
		retVal.Version_1_0->redoLevelDescriptions.emplace();
		for (int i = 0; i < count; i++) {
			QString description = undoManagerPtr->GetRedoLevelDescription(i);
			retVal.Version_1_0->redoLevelDescriptions->append(description);
		}
	}

	retVal.Version_1_0->status->status = UM::EUndoStatus::Success;

	return retVal;
}


template<class Base, class ColorCollectionDocumentManagerDefs>
inline UM::CUndoStatus TCollectionDocumentManagerCompBase<Base, ColorCollectionDocumentManagerDefs>::OnDoUndo(
	const typename Defs::DoUndoGqlRequest& doUndoRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	UM::CUndoStatus retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->status.emplace();

	const auto& arguments = doUndoRequest.GetRequestedArguments();
	const auto& info = doUndoRequest.GetRequestInfo();

	istd::TSharedNullable<UM::CUndoRedoInput::V1_0> undoRedoInput = arguments.input.Version_1_0;
	if (!undoRedoInput || !undoRedoInput->documentId || !undoRedoInput->steps) {
		errorMessage = "Invalid GQL request params";

		retVal.Version_1_0->status = UM::EUndoStatus::InvalidDocumentId;

		return retVal;
	}

	QByteArray documentId = *undoRedoInput->documentId;
	QByteArray userId = GetUserId(gqlRequest);

	if (userId.isEmpty()) {
		errorMessage = "Unable to get user ID from context";

		retVal.Version_1_0->status = UM::EUndoStatus::InvalidUserId;

		return retVal;
	}

	QMutexLocker locker(&m_mutex);

	idoc::IUndoManager* undoManagerPtr = nullptr;
	if (!m_userDocuments.contains(userId) || !m_userDocuments[userId].contains(documentId)) {
		errorMessage = "Undo operation failed";

		retVal.Version_1_0->status = UM::EUndoStatus::Failed;

		return retVal;
	}

	undoManagerPtr = m_userDocuments[userId][documentId].undoManagerPtr.GetPtr();

	if (undoManagerPtr->GetAvailableUndoSteps() < *undoRedoInput->steps) {
		errorMessage = "The number of available undo steps is less than requested";

		retVal.Version_1_0->status = UM::EUndoStatus::InvalidStepCount;

		return retVal;
	}

	if (!undoManagerPtr->DoUndo(*undoRedoInput->steps)) {
		errorMessage = "Undo operation failed";

		retVal.Version_1_0->status = UM::EUndoStatus::Failed;

		return retVal;
	}

	retVal.Version_1_0->status = UM::EUndoStatus::Success;

	return retVal;
}


template<class Base, class ColorCollectionDocumentManagerDefs>
inline UM::CUndoStatus TCollectionDocumentManagerCompBase<Base, ColorCollectionDocumentManagerDefs>::OnDoRedo(
	const typename Defs::DoRedoGqlRequest& doRedoRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	UM::CUndoStatus retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->status.emplace();

	const auto& arguments = doRedoRequest.GetRequestedArguments();
	const auto& info = doRedoRequest.GetRequestInfo();

	istd::TSharedNullable<UM::CUndoRedoInput::V1_0> undoRedoInput = arguments.input.Version_1_0;
	if (!undoRedoInput || !undoRedoInput->documentId || !undoRedoInput->steps) {
		errorMessage = "Invalid GQL request params";

		retVal.Version_1_0->status = UM::EUndoStatus::InvalidDocumentId;

		return retVal;
	}

	QByteArray documentId = *undoRedoInput->documentId;
	QByteArray userId = GetUserId(gqlRequest);

	if (userId.isEmpty()) {
		errorMessage = "Unable to get user ID from context";

		retVal.Version_1_0->status = UM::EUndoStatus::InvalidUserId;

		return retVal;
	}

	QMutexLocker locker(&m_mutex);

	idoc::IUndoManager* undoManagerPtr = nullptr;
	if (!m_userDocuments.contains(userId) || !m_userDocuments[userId].contains(documentId)) {
		errorMessage = "Redo operation failed";

		retVal.Version_1_0->status = UM::EUndoStatus::Failed;

		return retVal;
	}

	undoManagerPtr = m_userDocuments[userId][documentId].undoManagerPtr.GetPtr();

	if (undoManagerPtr->GetAvailableRedoSteps() < *undoRedoInput->steps) {
		errorMessage = "The number of available redo steps is less than requested";

		retVal.Version_1_0->status = UM::EUndoStatus::InvalidStepCount;

		return retVal;
	}

	if (!undoManagerPtr->DoRedo(*undoRedoInput->steps)) {
		errorMessage = "Redo operation failed";

		retVal.Version_1_0->status = UM::EUndoStatus::Failed;

		return retVal;
	}

	retVal.Version_1_0->status = UM::EUndoStatus::Success;

	return retVal;
}


template<class Base, class ColorCollectionDocumentManagerDefs>
inline UM::CUndoStatus TCollectionDocumentManagerCompBase<Base, ColorCollectionDocumentManagerDefs>::OnResetUndo(
	const typename Defs::ResetUndoGqlRequest& resetUndoRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	UM::CUndoStatus retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->status.emplace();

	const auto& arguments = resetUndoRequest.GetRequestedArguments();
	const auto& info = resetUndoRequest.GetRequestInfo();

	auto documentId = arguments.input.Version_1_0;
	if (!documentId || !documentId->id) {
		errorMessage = "Invalid GQL request params";

		retVal.Version_1_0->status = UM::EUndoStatus::InvalidDocumentId;

		return retVal;
	}

	QByteArray userId = GetUserId(gqlRequest);

	if (userId.isEmpty()) {
		errorMessage = "Unable to get user ID from context";

		retVal.Version_1_0->status = UM::EUndoStatus::InvalidUserId;

		return retVal;
	}

	QMutexLocker locker(&m_mutex);

	idoc::IUndoManager* undoManagerPtr = nullptr;
	if (!m_userDocuments.contains(userId) || !m_userDocuments[userId].contains(*documentId->id)) {
		errorMessage = "Reset undo operation failed";

		retVal.Version_1_0->status = UM::EUndoStatus::Failed;

		return retVal;
	}

	undoManagerPtr = m_userDocuments[userId][*documentId->id].undoManagerPtr.GetPtr();
	undoManagerPtr->ResetUndo();

	retVal.Version_1_0->status = UM::EUndoStatus::Success;

	return retVal;
}


} // namespace imtservergql
