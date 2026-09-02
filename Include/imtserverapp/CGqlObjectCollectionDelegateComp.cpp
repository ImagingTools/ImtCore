// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial

#include <imtserverapp/CGqlObjectCollectionDelegateComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QDebug>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imod/TModelWrap.h>
#include <iprm/IIdParam.h>
#include <iprm/TParamsPtr.h>
#include <iser/CJsonMemWriteArchive.h>
#include <iser/CJsonMemReadArchive.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/CFilterCollectionProxy.h>
#include <imtbase/COperationContext.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequestContextManager.h>


namespace imtserverapp
{


// public methods

imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectTypeIdRequest(const QByteArray& objectId) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::V1_0::imtbase::GetObjectTypeIdRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->objectId = objectId;
	arguments.input->collectionId = *m_collectionIdAttrPtr;

	return CreateGqlRequest<
				sdl::V1_0::imtbase::GetObjectTypeIdRequestArguments,
				sdl::V1_0::imtbase::CGetObjectTypeIdGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectInfoRequest(const QByteArray& objectId) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::V1_0::imtbase::GetElementInfoRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->collectionId = *m_collectionIdAttrPtr;
	arguments.input->elementId = objectId;

	return CreateGqlRequest<
				sdl::V1_0::imtbase::GetElementInfoRequestArguments,
				sdl::V1_0::imtbase::CGetElementInfoGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectMetaInfoRequest(const QByteArray& objectId) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::V1_0::imtbase::GetElementMetaInfoRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->collectionId = *m_collectionIdAttrPtr;
	arguments.input->elementId = objectId;

	return CreateGqlRequest<
				sdl::V1_0::imtbase::GetElementMetaInfoRequestArguments,
				sdl::V1_0::imtbase::CGetElementMetaInfoGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectDataMetaInfoRequest(const QByteArray& objectId) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::V1_0::imtbase::GetDataMetaInfoRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->collectionId = *m_collectionIdAttrPtr;
	arguments.input->objectId = objectId;

	return CreateGqlRequest<
				sdl::V1_0::imtbase::GetDataMetaInfoRequestArguments,
				sdl::V1_0::imtbase::CGetDataMetaInfoGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateInsertObjectRequest(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			const istd::IChangeable* objectPtr,
			const QString& /*uploadUrl*/,
			const QByteArray& proposedObjectId,
			const QByteArray& /*nodeId*/,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::V1_0::imtbase::InsertNewObjectRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->collectionId = *m_collectionIdAttrPtr;
	arguments.input->name = name;
	arguments.input->description = description;

	QByteArray objectData;
	if (objectPtr != nullptr){
		if (!SerializeObject(objectPtr, objectData)){
			SendErrorMessage(0,
						QStringLiteral("Unable to create insert request for object with id '%1', typeId '%2'. Error: Serialization object failed")
							.arg(proposedObjectId, typeId),
							"CGqlObjectCollectionDelegateComp");

			return nullptr;
		}
	}
	arguments.input->objectData = objectData;
	arguments.input->proposedObjectId = proposedObjectId;

	if (dataMetaInfoPtr != nullptr){
		QByteArray dataMetaInfo;
		if (SerializeObject(dataMetaInfoPtr, dataMetaInfo)){
			arguments.input->documentMetaInfo = dataMetaInfo;
		}
	}

	if (collectionItemMetaInfoPtr != nullptr){
		QByteArray collectionMetaInfo;
		if (SerializeObject(collectionItemMetaInfoPtr, collectionMetaInfo)){
			arguments.input->collectionItemMetaInfo = collectionMetaInfo;
		}
	}

	if (operationContextPtr != nullptr){
		QByteArray operationContext;
		if (SerializeObject(operationContextPtr, operationContext)){
			arguments.input->operationContext = operationContext;
		}
	}

	return CreateGqlRequest<
				sdl::V1_0::imtbase::InsertNewObjectRequestArguments,
				sdl::V1_0::imtbase::CInsertNewObjectGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectRequest(
			const QByteArray& objectId,
			const iprm::IParamsSet* /* paramsPtr */) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::V1_0::imtbase::GetObjectDataRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->objectId = objectId;
	arguments.input->collectionId = *m_collectionIdAttrPtr;

	return CreateGqlRequest<
				sdl::V1_0::imtbase::GetObjectDataRequestArguments,
				sdl::V1_0::imtbase::CGetObjectDataGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateSetObjectRequest(
			const QByteArray& objectId,
			const QByteArray& /*typeId*/,
			const istd::IChangeable* objectPtr,
			const QString& /*uploadUrl*/,
			const idoc::IDocumentMetaInfo* /*dataMetaInfoPtr*/,
			const idoc::IDocumentMetaInfo* /*collectionItemMetaInfoPtr*/,
			int /*clientVersion*/,
			const imtbase::IOperationContext* operationContextPtr) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::V1_0::imtbase::SetObjectDataRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->collectionId = *m_collectionIdAttrPtr;
	arguments.input->objectId = objectId;

	if (objectPtr != nullptr){
		QByteArray objectData;
		if (SerializeObject(objectPtr, objectData)){
			arguments.input->objectData = objectData;
		}
	}

	if (operationContextPtr != nullptr){
		QByteArray operationContext;
		if (SerializeObject(operationContextPtr, operationContext)){
			arguments.input->operationContext = operationContext;
		}
	}

	return CreateGqlRequest<
				sdl::V1_0::imtbase::SetObjectDataRequestArguments,
				sdl::V1_0::imtbase::CSetObjectDataGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateRemoveObjectsRequest(
			const imtbase::ICollectionInfo::Ids& objectIds,
			int /*clientElementVersion*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::V1_0::imtbase::RemoveElementsRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->collectionId = *m_collectionIdAttrPtr;
	arguments.input->elementIds.Emplace();
	arguments.input->elementIds->FromList(objectIds);

	return CreateGqlRequest<
				sdl::V1_0::imtbase::RemoveElementsRequestArguments,
				sdl::V1_0::imtbase::CRemoveElementsGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateRemoveObjectSetRequest(
			const iprm::IParamsSet* selectionParamsPtr,
			int /*clientElementVersion*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::V1_0::imtbase::RemoveElementSetRequestArguments arguments;
	arguments.input->collectionId = *m_collectionIdAttrPtr;

	if (selectionParamsPtr != nullptr){
		sdl::V1_0::imtbase::CParamsSet paramsSet;
		if (GetParamsSetRepresentation(*selectionParamsPtr, paramsSet)){
			arguments.input->selectionParams = paramsSet;
		}
	}

	return CreateGqlRequest<
				sdl::V1_0::imtbase::RemoveElementSetRequestArguments,
				sdl::V1_0::imtbase::CRemoveElementSetGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetElementCountRequest(
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::V1_0::imtbase::GetElementsCountRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->collectionId = *m_collectionIdAttrPtr;

	if (selectionParamsPtr != nullptr){
		if (selectionParamsPtr != nullptr){
			sdl::V1_0::imtbase::CParamsSet paramsSet;
			if (GetParamsSetRepresentation(*selectionParamsPtr, paramsSet)){
				arguments.input->selectionParams = paramsSet;
			}
		}
	}

	return CreateGqlRequest<
				sdl::V1_0::imtbase::GetElementsCountRequestArguments,
				sdl::V1_0::imtbase::CGetElementsCountGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetElementListRequest(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::V1_0::imtbase::GetElementIdsRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->collectionId = *m_collectionIdAttrPtr;
	arguments.input->count = count;
	arguments.input->offset = offset;

	if (selectionParamsPtr != nullptr){
		if (selectionParamsPtr != nullptr){
			sdl::V1_0::imtbase::CParamsSet paramsSet;
			if (GetParamsSetRepresentation(*selectionParamsPtr, paramsSet)){
				arguments.input->selectionParams = paramsSet;
			}
		}
	}

	return CreateGqlRequest<
				sdl::V1_0::imtbase::GetElementIdsRequestArguments,
				sdl::V1_0::imtbase::CGetElementIdsGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetSubCollectionRequest(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::V1_0::imtbase::CreateSubCollectionRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->collectionId = *m_collectionIdAttrPtr;
	arguments.input->offset = offset;
	arguments.input->count = count;

	if (selectionParamsPtr != nullptr){
		if (selectionParamsPtr != nullptr){
			sdl::V1_0::imtbase::CParamsSet paramsSet;
			if (GetParamsSetRepresentation(*selectionParamsPtr, paramsSet)){
				arguments.input->selectionParams = paramsSet;
			}
		}
	}

	return CreateGqlRequest<
				sdl::V1_0::imtbase::CreateSubCollectionRequestArguments,
				sdl::V1_0::imtbase::CCreateSubCollectionGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateSetObjectNameRequest(
			const QByteArray& objectId,
			const QString& name,
			int /*clientVersion*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::V1_0::imtbase::SetObjectNameRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->collectionId = *m_collectionIdAttrPtr;
	arguments.input->objectId = objectId;
	arguments.input->name = name;

	return CreateGqlRequest<
				sdl::V1_0::imtbase::SetObjectNameRequestArguments,
				sdl::V1_0::imtbase::CSetObjectNameGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateSetObjectDescriptionRequest(
			const QByteArray& objectId,
			const QString& description,
			int /*clientVersion*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::V1_0::imtbase::SetObjectDescriptionRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->collectionId = *m_collectionIdAttrPtr;
	arguments.input->objectId = objectId;
	arguments.input->description = description;

	return CreateGqlRequest<
				sdl::V1_0::imtbase::SetObjectDescriptionRequestArguments,
				sdl::V1_0::imtbase::CSetObjectDescriptionGqlRequest>(arguments).PopInterfacePtr();
}


bool CGqlObjectCollectionDelegateComp::IsValid(const imtgql::IGqlResponse& /*response*/) const
{
	return false;
}


bool CGqlObjectCollectionDelegateComp::GetOperationResult(const imtgql::IGqlResponse& response, bool& out) const
{
	ResponseData responseData = GetResponseData(response);

	out = responseData.errors.isEmpty();

	return true;
}


bool CGqlObjectCollectionDelegateComp::GetObjectId(const imtgql::IGqlResponse& response, Id& out) const
{
	sdl::V1_0::imtbase::CInsertNewObjectPayload insertNewObjectPayload;

	ResponseData responseData = GetResponseData(response);
	if (insertNewObjectPayload.ReadFromJsonObject(responseData.data)){
		if (insertNewObjectPayload.objectId){
			out = *insertNewObjectPayload.objectId;

			return true;
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateComp::GetObjectTypeId(const imtgql::IGqlResponse& response, Id& out) const
{
	sdl::V1_0::imtbase::CGetObjectTypeIdPayload getObjectTypeIdPayload;
	ResponseData responseData = GetResponseData(response);
	if (getObjectTypeIdPayload.ReadFromJsonObject(responseData.data)){
		if (getObjectTypeIdPayload.typeId){
			out = *getObjectTypeIdPayload.typeId;
			return true;
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateComp::GetObjectInfo(const imtgql::IGqlResponse& response, ObjectInfo& out) const
{
	sdl::V1_0::imtbase::CGetElementInfoPayload getElementInfoPayload;
	ResponseData responseData = GetResponseData(response);
	if (getElementInfoPayload.ReadFromJsonObject(responseData.data)){
		if (getElementInfoPayload.elementInfo){
			sdl::V1_0::imtbase::CParameter parameter = *getElementInfoPayload.elementInfo;
			if (parameter.id){
				out.id = *parameter.id;
			}

			if (parameter.name){
				out.name = *parameter.name;
			}

			if (parameter.description){
				out.description = *parameter.description;
			}

			if (parameter.typeId){
				out.typeId = *parameter.typeId;
			}

			return true;
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateComp::GetObjectData(const imtgql::IGqlResponse& response, istd::IChangeable& out) const
{
	sdl::V1_0::imtbase::CGetObjectDataPayload getObjectDataPayload;
	ResponseData responseData = GetResponseData(response);
	if (getObjectDataPayload.ReadFromJsonObject(responseData.data)){
		if (getObjectDataPayload.objectData){
			return DeSerializeObject(&out, (*getObjectDataPayload.objectData).toUtf8());
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateComp::GetMetaInfo(const imtgql::IGqlResponse& response, idoc::IDocumentMetaInfo& /* out */) const
{
	sdl::V1_0::imtbase::CGetElementMetaInfoPayload getElementMetaInfoPayload;
	ResponseData responseData = GetResponseData(response);

	if (getElementMetaInfoPayload.ReadFromJsonObject(responseData.data)){
		// if (getElementMetaInfoPayload.metaInfoData){
		// 	QByteArray metaInfoData = (*getElementMetaInfoPayload.metaInfoData).toUtf8();
		// 	if (!metaInfoData.isEmpty()){
		// 		return DeSerializeObject(&out, metaInfoData);
		// 	}
		// }
	}

	return false;
}


bool CGqlObjectCollectionDelegateComp::GetItemCount(const imtgql::IGqlResponse& response, int& out) const
{
	sdl::V1_0::imtbase::CGetElementsCountPayload getElementsCountPayload;
	ResponseData responseData = GetResponseData(response);
	if (getElementsCountPayload.ReadFromJsonObject(responseData.data)){
		if (getElementsCountPayload.count){
			out = *getElementsCountPayload.count;
			return true;
		}
	}

	out = -1;

	return false;
}


bool CGqlObjectCollectionDelegateComp::GetItemIds(const imtgql::IGqlResponse& response, Ids& out) const
{
	sdl::V1_0::imtbase::CGetElementIdsPayload getElementIdsPayload;
	ResponseData responseData = GetResponseData(response);
	if (getElementIdsPayload.ReadFromJsonObject(responseData.data)){
		if (getElementIdsPayload.elementIds.HasValue()){
			out = getElementIdsPayload.elementIds->ToList();

			return true;
		}
	}

	return false;
}


imtbase::IObjectCollection* CGqlObjectCollectionDelegateComp::GetSubCollection(
			imtbase::IObjectCollection& collection,
			const imtgql::IGqlResponse& response,
			QList<imtbase::IMetaInfoCreator*> metaInfoCreatorList) const
{
	istd::TDelPtr<imtbase::IObjectCollection> subCollectionPtr(new imtbase::CFilterCollectionProxy(collection));

	sdl::V1_0::imtbase::CCreateSubCollectionPayload createSubCollectionPayload;
	ResponseData responseData = GetResponseData(response);
	if (!createSubCollectionPayload.ReadFromJsonObject(responseData.data)){
		return subCollectionPtr.PopPtr();
	}

	if (!createSubCollectionPayload.items){
		return subCollectionPtr.PopPtr();
	}

	const imtsdl::TElementList<sdl::V1_0::imtbase::CSubCollectionItem>& subCollectionItems = *createSubCollectionPayload.items;
	for (const istd::TNullableValue<sdl::V1_0::imtbase::CSubCollectionItem>& subCollectionItem : subCollectionItems){
		const QByteArray objectId		= *subCollectionItem->itemInfo->id;
		const QByteArray objectTypeId	= *subCollectionItem->itemInfo->typeId;
		const QString name				= *subCollectionItem->itemInfo->name;
		const QString description		= *subCollectionItem->itemInfo->description;

		// Deserialize item data if available
		imtbase::IObjectCollection::DataPtr objectPtr;
		if (subCollectionItem->itemData){
			QByteArray itemDataInfo = (*subCollectionItem->itemData).toUtf8();
			objectPtr = CreateObject(objectTypeId);
			if (objectPtr.IsValid()){
				if (!DeSerializeObject(objectPtr.GetPtr(), itemDataInfo)){
					objectPtr.SetPtr(nullptr);
				}
			}
		}

		// Use meta info creator only when object data is available
		idoc::MetaInfoPtr dataMetainfoPtr;
		if (objectPtr.IsValid()){
			for (int i = 0; i < metaInfoCreatorList.count(); i++){
				imtbase::IMetaInfoCreator* metaInfoCreatorPtr = metaInfoCreatorList[i];
				if (metaInfoCreatorPtr == nullptr){
					continue;
				}

				const QByteArrayList typeIds = metaInfoCreatorPtr->GetSupportedTypeIds();
				if (!typeIds.contains(objectTypeId)){
					continue;
				}

				metaInfoCreatorPtr->CreateMetaInfo(objectPtr.GetPtr(), objectTypeId, dataMetainfoPtr);
				break;
			}
		}

		if (!dataMetainfoPtr.IsValid()){
			dataMetainfoPtr.SetPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>());
		}

		// Deserialize collection item meta info
		idoc::CStandardDocumentMetaInfo metainfo;
		if (subCollectionItem->metaInfo){
			QByteArray metaInfoData = (*subCollectionItem->metaInfo).toUtf8();
			if (!DeSerializeObject(&metainfo, metaInfoData)){
				qDebug() << "Deserialization of the meta information failed!";
			}
		}

		// Deserialize data meta info from response
		if (subCollectionItem->dataMetaInfo){
			QByteArray dataMetaInfo = (*subCollectionItem->dataMetaInfo).toUtf8();
			if (!DeSerializeObject(dataMetainfoPtr.GetPtr(), dataMetaInfo)){
				qDebug() << "Deserialization of the data meta info failed!";
			}
		}

		imtbase::COperationContext operationContext;
		if (subCollectionItem->operationContext){
			QByteArray operationContextData = (*subCollectionItem->operationContext).toUtf8();
			DeSerializeObject(&operationContext, operationContextData);
		}

		subCollectionPtr->InsertNewObject(
			objectTypeId,
			name,
			description,
			objectPtr.GetPtr(),
			objectId,
			dataMetainfoPtr.GetPtr(),
			&metainfo,
			&operationContext);

		dataMetainfoPtr.SetPtr(nullptr);
	}

	return subCollectionPtr.PopPtr();
}


// private methods

istd::IChangeableUniquePtr CGqlObjectCollectionDelegateComp::CreateObject(const QByteArray& typeId) const
{
	if (!m_objectFactoriesCompPtr.IsValid()){
		return nullptr;
	}

	for (int i = 0; i < m_typeIdsAttrPtr.GetCount(); ++i){
		if (i >= m_objectFactoriesCompPtr.GetCount()){
			break;
		}

		if (typeId == m_typeIdsAttrPtr[i]){
			return m_objectFactoriesCompPtr.CreateInstance(i);
		}
	}

	return nullptr;
}


CGqlObjectCollectionDelegateComp::ResponseData CGqlObjectCollectionDelegateComp::GetResponseData(const imtgql::IGqlResponse& response) const
{
	ResponseData data;
	imtgql::IGqlResponse::GqlRequestPtr requestPtr = response.GetOriginalRequest();
	if (requestPtr.IsValid()){
		data.commandId = requestPtr->GetCommandId();
	}

	QJsonDocument document = QJsonDocument::fromJson(response.GetResponseData());
	if (document.isObject()){
		QJsonObject rootObject = document.object();
		if (rootObject.contains("data")){
			QJsonObject dataObject = rootObject.value("data").toObject();
			if (dataObject.contains(data.commandId)){
				data.data = dataObject.value(data.commandId).toObject();
			}
		}

		if (rootObject.contains("errors")){
			data.errors = rootObject["errors"].toObject();
			QJsonDocument errorsDoc;
			errorsDoc.setObject(data.errors);
			SendErrorMessage(0, errorsDoc.toJson(QJsonDocument::Compact));
		}
	}

	return data;
}


bool CGqlObjectCollectionDelegateComp::SerializeObject(const istd::IPolymorphic* object, QByteArray& objectData) const
{
	if (object == nullptr){
		SendErrorMessage(0, QStringLiteral("Unable to serialize object. Error: Object is nullptr"));
		return false;
	}

	const iser::ISerializable* objectConst = dynamic_cast<const iser::ISerializable*>(object);
	iser::ISerializable* serializableObject = dynamic_cast<iser::ISerializable*>(const_cast<iser::ISerializable*>(objectConst));

	if (serializableObject == nullptr){
		SendErrorMessage(0, QStringLiteral("Unable to serialize object. Error: Object is not serializable"));
		return false;
	}

	objectData.clear();

	istd::TDelPtr<iser::CJsonMemWriteArchive> archivePtr;
	if (m_versionInfoCompPtr.IsValid()){
		archivePtr.SetPtr(new iser::CJsonMemWriteArchive(m_versionInfoCompPtr.GetPtr()));
	}
	else{
		archivePtr.SetPtr(new iser::CJsonMemWriteArchive());
	}

	if (!serializableObject->Serialize(*archivePtr.GetPtr())){
		SendErrorMessage(0, QStringLiteral("Unable to serialize object. Error: Serialization failed"));
		return false;
	}
	objectData = archivePtr->GetData();

	return true;
}


bool CGqlObjectCollectionDelegateComp::DeSerializeObject(istd::IPolymorphic* object, const QByteArray& objectData) const
{
	if (object == nullptr){
		SendErrorMessage(0, QStringLiteral("Unable to deserialize object. Error: Object is invalid"));
		return false;
	}

	iser::ISerializable* serializableObjectPtr = dynamic_cast<iser::ISerializable*>(object);
	if (serializableObjectPtr == nullptr){
		SendErrorMessage(0, QStringLiteral("Unable to deserialize object. Error: Object is not serializable"));
		return false;
	}

	iser::CJsonMemReadArchive archive(objectData, !objectData.isEmpty());
	if (!serializableObjectPtr->Serialize(archive)){
		SendErrorMessage(0, QStringLiteral("Unable to deserialize object. Error: Deserialization failed"));

		return false;
	}

	return true;
}


bool CGqlObjectCollectionDelegateComp::GetParamsSetRepresentation(
			const iprm::IParamsSet& paramsSet,
			sdl::V1_0::imtbase::CParamsSet& representation) const
{
	QJsonObject jsonObject;
	if (m_paramSetRepresentationControllerCompPtr.IsValid() && m_paramSetRepresentationControllerCompPtr->GetRepresentationFromDataModel(paramsSet, jsonObject)){
		if (representation.ReadFromJsonObject(jsonObject)){
			return true;
		}
	}

	return false;
}


template<class Arguments,class SdlRequest>
istd::TUniqueInterfacePtr<imtgql::IGqlRequest> CGqlObjectCollectionDelegateComp::CreateGqlRequest(Arguments arguments) const
{
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest();
	imtgql::IGqlContext* gqlContextPtr = imtgql::CGqlRequestContextManager::GetContext();
	if (gqlContextPtr != nullptr){
		istd::IChangeableUniquePtr clonedPtr = gqlContextPtr->CloneMe();
		imtgql::IGqlContextUniquePtr castedPtr;
		castedPtr.MoveCastedPtr(std::move(clonedPtr));
		requestPtr->SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(castedPtr)));
	}

	if (!SdlRequest::SetupGqlRequest(*requestPtr, arguments)){
		return nullptr;
	}

	return istd::TUniqueInterfacePtr<imtgql::IGqlRequest>(requestPtr);
}


} // namespace imtserverapp


