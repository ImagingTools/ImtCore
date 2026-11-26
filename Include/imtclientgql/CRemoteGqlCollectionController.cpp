// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CRemoteGqlCollectionController.h>


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <ilog/CMessage.h>
#include <imod/TModelWrap.h>
#include <iprm/IOptionsList.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtgql/IGqlResponse.h>


namespace imtclientgql
{


typedef istd::TSharedInterfacePtr<imtgql::IGqlRequest> GqlRequestPtr;
typedef istd::TSharedInterfacePtr<imtgql::IGqlResponse> GqlResponsePtr;


// public methods

CRemoteGqlCollectionController::CRemoteGqlCollectionController()
	:m_gqlClientPtr(nullptr),
	m_gqlObjectCollectionDelegatePtr(nullptr)
{
}


// reimplemented (IObjectCollection)

const imtbase::IRevisionController* CRemoteGqlCollectionController::GetRevisionController() const
{
	return nullptr;
}


const imtbase::ICollectionDataController* CRemoteGqlCollectionController::GetDataController() const
{
	return nullptr;
}


int CRemoteGqlCollectionController::GetOperationFlags(const Id& elementId) const
{
	if (!elementId.isEmpty()){
		return OF_SUPPORT_DELETE | OF_SUPPORT_EDIT;
	}

	return OF_SUPPORT_INSERT | OF_SUPPORT_DELETE | OF_SUPPORT_EDIT | OF_SUPPORT_PAGINATION;
}


imtbase::ICollectionInfo::Id CRemoteGqlCollectionController::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			const istd::IChangeable* defaultValuePtr,
			const QByteArray& proposedObjectId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_gqlClientPtr == nullptr || m_gqlObjectCollectionDelegatePtr == nullptr){
		return Id();
	}

	Id retVal;

	GqlRequestPtr requestPtr(m_gqlObjectCollectionDelegatePtr->CreateInsertObjectRequest(
				typeId,
				name,
				description,
				defaultValuePtr,
				"", // uploadUrl
				proposedObjectId,
				"", // nodeId,
				dataMetaInfoPtr,
				collectionItemMetaInfoPtr,
				operationContextPtr));

	if (requestPtr.IsValid()){
		const auto requestResult = m_gqlClientPtr->SendRequest(requestPtr);
		auto responsePtr = requestResult.responsePtr;
		if (requestResult){
			if (m_gqlObjectCollectionDelegatePtr->GetObjectId(*responsePtr, retVal)){
				imtbase::ICollectionInfo::ElementInsertInfo insertInfo;
				insertInfo.elementId = retVal;
				istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
				changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED, QVariant::fromValue(insertInfo));
				istd::CChangeNotifier notifier(this, &changeSet);
			}
		}
	}

	return retVal;
}


bool CRemoteGqlCollectionController::RemoveElements(const Ids& elementIds, const imtbase::IOperationContext* operationContextPtr)
{
	if (m_gqlClientPtr == nullptr || m_gqlObjectCollectionDelegatePtr == nullptr){
		return false;
	}

	bool retVal = false;

	GqlRequestPtr requestPtr(m_gqlObjectCollectionDelegatePtr->CreateRemoveObjectsRequest(elementIds, -1, operationContextPtr));

	if (requestPtr.IsValid()){
		const auto requestResult = m_gqlClientPtr->SendRequest(requestPtr);
		auto responsePtr = requestResult.responsePtr;
		if (requestResult){
			m_gqlObjectCollectionDelegatePtr->GetOperationResult(*responsePtr, retVal);
		}
	}

	return retVal;
}


bool CRemoteGqlCollectionController::RemoveElementSet(
			const iprm::IParamsSet* selectionParamsPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_gqlClientPtr == nullptr || m_gqlObjectCollectionDelegatePtr == nullptr){
		return false;
	}

	bool retVal = false;

	GqlRequestPtr requestPtr(m_gqlObjectCollectionDelegatePtr->CreateRemoveObjectSetRequest(selectionParamsPtr, -1, operationContextPtr));

	if (requestPtr.IsValid()){
		const auto requestResult = m_gqlClientPtr->SendRequest(requestPtr);
		auto responsePtr = requestResult.responsePtr;
		if (requestResult){
			m_gqlObjectCollectionDelegatePtr->GetOperationResult(*responsePtr, retVal);
		}
	}

	return retVal;
}


bool CRemoteGqlCollectionController::RestoreObjects(
			const Ids& /*objectIds*/,
			const imtbase::IOperationContext* /*operationContextPtr*/)
{
	return false;
}


bool CRemoteGqlCollectionController::RestoreObjectSet(
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/)
{
	return false;
}


const istd::IChangeable* CRemoteGqlCollectionController::GetObjectPtr(const Id& /*objectId*/) const
{
	return nullptr;
}


bool CRemoteGqlCollectionController::GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr) const
{
	bool isAccessDenied = false;
	return GetObjectDataWithStatus(objectId, dataPtr, isAccessDenied, dataConfigurationPtr);
}


bool CRemoteGqlCollectionController::GetObjectDataWithStatus(
			const Id& objectId,
			DataPtr& dataPtr,
			bool& isAccessDenied,
			const iprm::IParamsSet* dataConfigurationPtr) const
{
	isAccessDenied = false;

	if (m_gqlClientPtr == nullptr || m_gqlObjectCollectionDelegatePtr == nullptr){
		return false;
	}

	const auto typeResult = ReadObjectTypeId(objectId);
	isAccessDenied = typeResult.errorCategory == IGqlClient::EC_ACCESS_DENIED;
	if (!typeResult){
		return false;
	}

	DataPtr retVal = CreateObjectInstance(typeResult.model);
	if (retVal.IsValid()){
		GqlRequestPtr requestPtr(m_gqlObjectCollectionDelegatePtr->CreateGetObjectRequest(objectId, dataConfigurationPtr));
		if (requestPtr.IsValid()){
			const auto result = m_gqlClientPtr->SendRequest(requestPtr);
			isAccessDenied = result.errorCategory == IGqlClient::EC_ACCESS_DENIED;
			if (result){
				if (m_gqlObjectCollectionDelegatePtr->GetObjectData(*result.responsePtr, *retVal)){
					dataPtr = retVal;

					return true;
				}
			}
		}
	}

	return false;
}


bool CRemoteGqlCollectionController::SetObjectData(
			const Id& objectId,
			const istd::IChangeable& object,
			CompatibilityMode /*mode*/,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_gqlClientPtr == nullptr || m_gqlObjectCollectionDelegatePtr == nullptr){
		return false;
	}

	bool retVal = false;

	QByteArray typeId = GetObjectTypeId(objectId);

	imtclientgql::IGqlObjectCollectionDelegate::ObjectInfo info;
	if (GetObjectInfo(objectId, info)){
		GqlRequestPtr requestPtr(m_gqlObjectCollectionDelegatePtr->CreateSetObjectRequest(
					objectId,
					typeId,
					&object,
					"",
					nullptr,
					nullptr,
					info.version,
					operationContextPtr));

		if (requestPtr.IsValid()){
			const auto requestResult = m_gqlClientPtr->SendRequest(requestPtr);
			auto responsePtr = requestResult.responsePtr;
			if (requestResult){
				if (m_gqlObjectCollectionDelegatePtr->GetOperationResult(*responsePtr, retVal)){
					if (retVal){
						istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
						imtbase::IObjectCollection::ObjectDataChanged updateInfo;
						updateInfo.elementId = objectId;
						changeSet.SetChangeInfo(imtbase::IObjectCollection::CN_OBJECT_DATA_CHANGED, QVariant::fromValue(updateInfo));
						istd::CChangeNotifier notifier(this, &changeSet);
					}
				}
			}
		}
	}

	return retVal;
}


imtbase::IObjectCollectionUniquePtr CRemoteGqlCollectionController::CreateSubCollection(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	imtbase::IObjectCollection* retVal = nullptr;

	if (m_gqlClientPtr == nullptr || m_gqlObjectCollectionDelegatePtr == nullptr){
		return retVal;
	}

	imtbase::IObjectCollection* collectionPtr = const_cast<imtbase::IObjectCollection*>(dynamic_cast<const imtbase::IObjectCollection*>(this));
	Q_ASSERT(collectionPtr != nullptr);

	if (collectionPtr != nullptr){
		GqlRequestPtr requestPtr(m_gqlObjectCollectionDelegatePtr->CreateGetSubCollectionRequest(offset, count, selectionParamsPtr));
		if (requestPtr.IsValid()){
			const auto requestResult = m_gqlClientPtr->SendRequest(requestPtr);
			auto responsePtr = requestResult.responsePtr;
			if (requestResult){
				retVal = m_gqlObjectCollectionDelegatePtr->GetSubCollection(*collectionPtr, *responsePtr, m_metaInfoCreatorMap.values());
			}
		}
	}

	return retVal;
}


imtbase::IObjectCollectionIterator* CRemoteGqlCollectionController::CreateObjectCollectionIterator(
			const QByteArray& /*objectId*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	return nullptr;
}


// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CRemoteGqlCollectionController::GetObjectTypesInfo() const
{
	return nullptr;
}


QByteArray CRemoteGqlCollectionController::GetObjectTypeId(const Id& objectId) const
{
	return ReadObjectTypeId(objectId).model;
}


IGqlClient::GqlModelResult<QByteArray> CRemoteGqlCollectionController::ReadObjectTypeId(const Id& objectId) const
{
	// A collection with a single object type needs no server round-trip to learn it.
	const iprm::IOptionsList* typesInfoPtr = GetObjectTypesInfo();
	if (typesInfoPtr != nullptr && typesInfoPtr->GetOptionsCount() == 1){
		return {typesInfoPtr->GetOptionId(0), IGqlClient::EC_NONE, {}};
	}

	if (m_gqlClientPtr != nullptr && m_gqlObjectCollectionDelegatePtr != nullptr){
		GqlRequestPtr requestPtr(m_gqlObjectCollectionDelegatePtr->CreateGetObjectTypeIdRequest(objectId));
		if (requestPtr.IsValid()){
			const auto result = m_gqlClientPtr->SendRequest(requestPtr);
			if (!result){
				return {{}, result.errorCategory == IGqlClient::EC_NONE ? IGqlClient::EC_INVALID_RESPONSE : result.errorCategory,
							result.errorMessage.isEmpty() ? QStringLiteral("Object type response is missing") : result.errorMessage};
			}
			QByteArray typeId;
			if (m_gqlObjectCollectionDelegatePtr->GetObjectTypeId(*result.responsePtr, typeId)){
				return {typeId, IGqlClient::EC_NONE, {}};
			}
			return {{}, IGqlClient::EC_INVALID_RESPONSE, QStringLiteral("Object type response is invalid")};
		}
		return {{}, IGqlClient::EC_INVALID_REQUEST, QStringLiteral("Object type request could not be created")};
	}

	return {{}, IGqlClient::EC_INTERNAL, QStringLiteral("Remote collection client or delegate is not configured")};
}


idoc::MetaInfoPtr CRemoteGqlCollectionController::GetDataMetaInfo(const Id& objectId) const
{
	idoc::MetaInfoPtr retVal;

	if (m_gqlClientPtr == nullptr || m_gqlObjectCollectionDelegatePtr == nullptr){
		return retVal;
	}

	const QByteArray typeId = GetObjectTypeId(objectId);
	if (m_metaInfoCreatorMap.contains(typeId)){
		imtbase::IMetaInfoCreator* metaInfoCreatorPtr = m_metaInfoCreatorMap[typeId];
		Q_ASSERT(metaInfoCreatorPtr != nullptr);

		// The meta info is created from the object, so reading it also proves the object exists.
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (GetObjectData(objectId, dataPtr)){
			metaInfoCreatorPtr->CreateMetaInfo(dataPtr.GetPtr(), typeId, retVal);
		}

		return retVal;
	}

	GqlRequestPtr requestPtr(m_gqlObjectCollectionDelegatePtr->CreateGetObjectDataMetaInfoRequest(objectId));
	if (requestPtr.IsValid()){
		const auto requestResult = m_gqlClientPtr->SendRequest(requestPtr);
		auto responsePtr = requestResult.responsePtr;
		if (requestResult){
			retVal.SetPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>());
		}
	}

	return retVal;
}


// reimplemented (ICollectionInfo)

int CRemoteGqlCollectionController::GetElementsCount(const iprm::IParamsSet* selectionParamsPtr, ilog::IMessageConsumer* /*logPtr*/) const
{
	int retVal = -1;

	if (m_gqlClientPtr == nullptr || m_gqlObjectCollectionDelegatePtr == nullptr){
		return retVal;
	}

	GqlRequestPtr requestPtr(m_gqlObjectCollectionDelegatePtr->CreateGetElementCountRequest(selectionParamsPtr));
	if (requestPtr.IsValid()){
		const auto requestResult = m_gqlClientPtr->SendRequest(requestPtr);
		auto responsePtr = requestResult.responsePtr;
		if (requestResult){
			int count;
			if (m_gqlObjectCollectionDelegatePtr->GetItemCount(*responsePtr, count)){
				retVal = count;
			}
		}
	}

	return retVal;
}


imtbase::ICollectionInfo::Ids CRemoteGqlCollectionController::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* logPtr) const
{
	if (m_gqlClientPtr == nullptr || m_gqlObjectCollectionDelegatePtr == nullptr){
		return Ids();
	}

	Ids retVal;

	// An empty list is a selection of nothing only when the server answered it
	bool isRead = false;
	GqlRequestPtr requestPtr(m_gqlObjectCollectionDelegatePtr->CreateGetElementListRequest(offset, count, selectionParamsPtr));
	if (requestPtr.IsValid()){
		const auto requestResult = m_gqlClientPtr->SendRequest(requestPtr);
		auto responsePtr = requestResult.responsePtr;
		if (requestResult){
			IGqlObjectCollectionDelegate::Ids ids;
			isRead = m_gqlObjectCollectionDelegatePtr->GetItemIds(*responsePtr, ids);
			std::copy(ids.begin(), ids.end(), std::back_inserter(retVal));
		}
	}

	if (!isRead && logPtr != nullptr){
		logPtr->AddMessage(ilog::IMessageConsumer::MessagePtr(new ilog::CMessage(
					istd::IInformationProvider::IC_ERROR, 0, QStringLiteral("The element list could not be read from the server"), "CRemoteGqlCollectionController")));
	}

	return retVal;
}


bool CRemoteGqlCollectionController::GetSubsetInfo(
			ICollectionInfo& /*subsetInfo*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return false;
}


QVariant CRemoteGqlCollectionController::GetElementInfo(const Id& elementId, int infoType, ilog::IMessageConsumer* /*logPtr*/) const
{
	imtclientgql::IGqlObjectCollectionDelegate::ObjectInfo info;

	if (GetObjectInfo(elementId, info)){
		switch (infoType)
		{
		case EIT_NAME:
			return info.name;

		case EIT_DESCRIPTION:
			return info.description;

		case EIT_ENABLED:
			return true;
		}
	}

	return QVariant();
}


idoc::MetaInfoPtr CRemoteGqlCollectionController::GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* /*logPtr*/) const
{
	idoc::MetaInfoPtr metaInfoPtr;

	imtclientgql::IGqlObjectCollectionDelegate::ObjectInfo info;
	if (GetObjectInfo(elementId, info)){
		if (!info.dataMetaInfoPtr.isNull()){
			metaInfoPtr.MoveCastedPtr(info.dataMetaInfoPtr->CloneMe());
		}
	}
	else{
		if (!GetObjectMetaInfo(elementId, metaInfoPtr)){
			metaInfoPtr.Reset();
		}
	}

	return metaInfoPtr;
}


bool CRemoteGqlCollectionController::SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* /*logPtr*/)
{
	bool retVal = false;

	if (m_gqlClientPtr == nullptr || m_gqlObjectCollectionDelegatePtr == nullptr){
		return retVal;
	}

	imtclientgql::IGqlObjectCollectionDelegate::ObjectInfo info;
	if (GetObjectInfo(elementId, info)){
		GqlRequestPtr requestPtr(m_gqlObjectCollectionDelegatePtr->CreateSetObjectNameRequest(elementId, name, info.version, nullptr));
		const auto requestResult = m_gqlClientPtr->SendRequest(requestPtr);
		auto responsePtr = requestResult.responsePtr;
		if (requestResult){
			m_gqlObjectCollectionDelegatePtr->GetOperationResult(*responsePtr, retVal);
		}
	}

	return retVal;
}


bool CRemoteGqlCollectionController::SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* /*logPtr*/)
{
	bool retVal = false;

	if (m_gqlClientPtr == nullptr || m_gqlObjectCollectionDelegatePtr == nullptr){
		return retVal;
	}

	imtclientgql::IGqlObjectCollectionDelegate::ObjectInfo info;
	if (GetObjectInfo(elementId, info)){
		GqlRequestPtr requestPtr(m_gqlObjectCollectionDelegatePtr->CreateSetObjectDescriptionRequest(elementId, description, info.version, nullptr));
		const auto requestResult = m_gqlClientPtr->SendRequest(requestPtr);
		auto responsePtr = requestResult.responsePtr;
		if (requestResult){
			m_gqlObjectCollectionDelegatePtr->GetOperationResult(*responsePtr, retVal);
		}
	}

	return retVal;
}


bool CRemoteGqlCollectionController::SetElementEnabled(const Id& /*elementId*/, bool /*isEnabled*/, ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


// protected methods

imtbase::IObjectCollection::DataPtr CRemoteGqlCollectionController::CreateObjectInstance(const QByteArray& typeId) const
{
	return BaseClass::CreateInstance(typeId);
}


// private methods

bool CRemoteGqlCollectionController::GetObjectInfo(
			const QByteArray& objectId,
			imtclientgql::IGqlObjectCollectionDelegate::ObjectInfo& valueOut) const
{
	bool retVal = false;

	if (m_gqlClientPtr == nullptr || m_gqlObjectCollectionDelegatePtr == nullptr){
		return retVal;
	}

	GqlRequestPtr requestPtr(m_gqlObjectCollectionDelegatePtr->CreateGetObjectInfoRequest(objectId));
	if (requestPtr.IsValid()){
		const auto requestResult = m_gqlClientPtr->SendRequest(requestPtr);
		auto responsePtr = requestResult.responsePtr;
		if (requestResult){
			retVal = m_gqlObjectCollectionDelegatePtr->GetObjectInfo(*responsePtr, valueOut);
			if (retVal){
				retVal = valueOut.id == objectId;
			}
		}
	}

	return retVal;
}


bool CRemoteGqlCollectionController::GetObjectMetaInfo(const QByteArray& objectId, idoc::MetaInfoPtr& valueOut) const
{
	bool retVal = false;

	if (m_gqlClientPtr == nullptr || m_gqlObjectCollectionDelegatePtr == nullptr){
		return retVal;
	}

	GqlRequestPtr requestPtr(m_gqlObjectCollectionDelegatePtr->CreateGetObjectMetaInfoRequest(objectId));
	if (requestPtr.IsValid()){
		const auto requestResult = m_gqlClientPtr->SendRequest(requestPtr);
		auto responsePtr = requestResult.responsePtr;
		if (requestResult){
			idoc::MetaInfoPtr metaInfoPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>());
			if (m_gqlObjectCollectionDelegatePtr->GetMetaInfo(*responsePtr, *metaInfoPtr)){
				valueOut = metaInfoPtr;

				retVal = true;
			}
		}
	}

	return retVal;
}


} // namespace imtclientgql
