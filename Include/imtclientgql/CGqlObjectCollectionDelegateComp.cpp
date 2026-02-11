// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial

#include <imtclientgql/CGqlObjectCollectionDelegateComp.h>


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


namespace imtclientgql
{


// public methods

imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectTypeIdRequest(const QByteArray& objectId) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::imtbase::ImtCollection::GetObjectTypeIdRequestArguments arguments;
	arguments.input.Version_1_0.emplace();
	arguments.input.Version_1_0->objectId = objectId;
	arguments.input.Version_1_0->collectionId = *m_collectionIdAttrPtr;

	return CreateGqlRequest<
				sdl::imtbase::ImtCollection::GetObjectTypeIdRequestArguments,
				sdl::imtbase::ImtCollection::CGetObjectTypeIdGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectInfoRequest(const QByteArray& objectId) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::imtbase::ImtCollection::GetElementInfoRequestArguments arguments;
	arguments.input.Version_1_0.emplace();
	arguments.input.Version_1_0->collectionId = *m_collectionIdAttrPtr;
	arguments.input.Version_1_0->elementId = objectId;

	return CreateGqlRequest<
				sdl::imtbase::ImtCollection::GetElementInfoRequestArguments,
				sdl::imtbase::ImtCollection::CGetElementInfoGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectMetaInfoRequest(const QByteArray& objectId) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::imtbase::ImtCollection::GetElementMetaInfoRequestArguments arguments;
	arguments.input.Version_1_0.emplace();
	arguments.input.Version_1_0->collectionId = *m_collectionIdAttrPtr;
	arguments.input.Version_1_0->elementId = objectId;

	return CreateGqlRequest<
				sdl::imtbase::ImtCollection::GetElementMetaInfoRequestArguments,
				sdl::imtbase::ImtCollection::CGetElementMetaInfoGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectDataMetaInfoRequest(const QByteArray& objectId) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::imtbase::ImtCollection::GetDataMetaInfoRequestArguments arguments;
	arguments.input.Version_1_0.emplace();
	arguments.input.Version_1_0->collectionId = *m_collectionIdAttrPtr;
	arguments.input.Version_1_0->objectId = objectId;

	return CreateGqlRequest<
				sdl::imtbase::ImtCollection::GetDataMetaInfoRequestArguments,
				sdl::imtbase::ImtCollection::CGetDataMetaInfoGqlRequest>(arguments).PopInterfacePtr();
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

	sdl::imtbase::ImtCollection::InsertNewObjectRequestArguments arguments;
	arguments.input.Version_1_0.emplace();
	arguments.input.Version_1_0->collectionId = *m_collectionIdAttrPtr;
	arguments.input.Version_1_0->name = name;
	arguments.input.Version_1_0->description = description;

	QByteArray objectData;
	if (objectPtr != nullptr){
		if (!SerializeObject(objectPtr, objectData)){
			SendErrorMessage(0,
						QString("Unable to create insert request for object with id '%1', typeId '%2'. Error: Serialization object failed")
							.arg(qPrintable(proposedObjectId), qPrintable(typeId)),
							"CGqlObjectCollectionDelegateComp");

			return nullptr;
		}
	}
	arguments.input.Version_1_0->objectData = objectData;
	arguments.input.Version_1_0->proposedObjectId = proposedObjectId;

	if (dataMetaInfoPtr != nullptr){
		QByteArray dataMetaInfo;
		if (SerializeObject(dataMetaInfoPtr, dataMetaInfo)){
			arguments.input.Version_1_0->documentMetaInfo = dataMetaInfo;
		}
	}

	if (collectionItemMetaInfoPtr != nullptr){
		QByteArray collectionMetaInfo;
		if (SerializeObject(collectionItemMetaInfoPtr, collectionMetaInfo)){
			arguments.input.Version_1_0->collectionItemMetaInfo = collectionMetaInfo;
		}
	}

	if (operationContextPtr != nullptr){
		QByteArray operationContext;
		if (SerializeObject(operationContextPtr, operationContext)){
			arguments.input.Version_1_0->operationContext = operationContext;
		}
	}

	return CreateGqlRequest<
				sdl::imtbase::ImtCollection::InsertNewObjectRequestArguments,
				sdl::imtbase::ImtCollection::CInsertNewObjectGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectRequest(
			const QByteArray& objectId,
			const iprm::IParamsSet* /* paramsPtr */) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::imtbase::ImtCollection::GetObjectDataRequestArguments arguments;
	arguments.input.Version_1_0.emplace();
	arguments.input.Version_1_0->objectId = objectId;
	arguments.input.Version_1_0->collectionId = *m_collectionIdAttrPtr;

	return CreateGqlRequest<
				sdl::imtbase::ImtCollection::GetObjectDataRequestArguments,
				sdl::imtbase::ImtCollection::CGetObjectDataGqlRequest>(arguments).PopInterfacePtr();
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

	sdl::imtbase::ImtCollection::SetObjectDataRequestArguments arguments;
	arguments.input.Version_1_0.emplace();
	arguments.input.Version_1_0->collectionId = *m_collectionIdAttrPtr;
	arguments.input.Version_1_0->objectId = objectId;

	if (objectPtr != nullptr){
		QByteArray objectData;
		if (SerializeObject(objectPtr, objectData)){
			arguments.input.Version_1_0->objectData = objectData;
		}
	}

	if (operationContextPtr != nullptr){
		QByteArray operationContext;
		if (SerializeObject(operationContextPtr, operationContext)){
			arguments.input.Version_1_0->operationContext = operationContext;
		}
	}

	return CreateGqlRequest<
				sdl::imtbase::ImtCollection::SetObjectDataRequestArguments,
				sdl::imtbase::ImtCollection::CSetObjectDataGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateRemoveObjectsRequest(
			const imtbase::ICollectionInfo::Ids& objectIds,
			int /*clientElementVersion*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::imtbase::ImtCollection::RemoveElementsRequestArguments arguments;
	arguments.input.Version_1_0.emplace();
	arguments.input.Version_1_0->collectionId = *m_collectionIdAttrPtr;
	arguments.input.Version_1_0->elementIds.Emplace();
	arguments.input.Version_1_0->elementIds->FromList(objectIds);

	return CreateGqlRequest<
				sdl::imtbase::ImtCollection::RemoveElementsRequestArguments,
				sdl::imtbase::ImtCollection::CRemoveElementsGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateRemoveObjectSetRequest(
			const iprm::IParamsSet* selectionParamsPtr,
			int /*clientElementVersion*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::imtbase::ImtCollection::RemoveElementSetRequestArguments arguments;
	arguments.input.Version_1_0.emplace();
	arguments.input.Version_1_0->collectionId = *m_collectionIdAttrPtr;

	if (selectionParamsPtr != nullptr){
		sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0 paramsSet;
		if (GetParamsSetRepresentation(*selectionParamsPtr, paramsSet)){
			arguments.input.Version_1_0->selectionParams = paramsSet;
		}
	}

	return CreateGqlRequest<
				sdl::imtbase::ImtCollection::RemoveElementSetRequestArguments,
				sdl::imtbase::ImtCollection::CRemoveElementSetGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetElementCountRequest(
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::imtbase::ImtCollection::GetElementsCountRequestArguments arguments;
	arguments.input.Version_1_0.emplace();
	arguments.input.Version_1_0->collectionId = *m_collectionIdAttrPtr;

	if (selectionParamsPtr != nullptr){
		if (selectionParamsPtr != nullptr){
			sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0 paramsSet;
			if (GetParamsSetRepresentation(*selectionParamsPtr, paramsSet)){
				arguments.input.Version_1_0->selectionParams = paramsSet;
			}
		}
	}

	return CreateGqlRequest<
				sdl::imtbase::ImtCollection::GetElementsCountRequestArguments,
				sdl::imtbase::ImtCollection::CGetElementsCountGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetElementListRequest(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::imtbase::ImtCollection::GetElementIdsRequestArguments arguments;
	arguments.input.Version_1_0.emplace();
	arguments.input.Version_1_0->collectionId = *m_collectionIdAttrPtr;
	arguments.input.Version_1_0->count = count;
	arguments.input.Version_1_0->offset = offset;

	if (selectionParamsPtr != nullptr){
		if (selectionParamsPtr != nullptr){
			sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0 paramsSet;
			if (GetParamsSetRepresentation(*selectionParamsPtr, paramsSet)){
				arguments.input.Version_1_0->selectionParams = paramsSet;
			}
		}
	}

	return CreateGqlRequest<
				sdl::imtbase::ImtCollection::GetElementIdsRequestArguments,
				sdl::imtbase::ImtCollection::CGetElementIdsGqlRequest>(arguments).PopInterfacePtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetSubCollectionRequest(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (!m_collectionIdAttrPtr.IsValid()){
		return nullptr;
	}

	sdl::imtbase::ImtCollection::CreateSubCollectionRequestArguments arguments;
	arguments.input.Version_1_0.emplace();
	arguments.input.Version_1_0->collectionId = *m_collectionIdAttrPtr;
	arguments.input.Version_1_0->offset = offset;
	arguments.input.Version_1_0->count = count;

	if (selectionParamsPtr != nullptr){
		if (selectionParamsPtr != nullptr){
			sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0 paramsSet;
			if (GetParamsSetRepresentation(*selectionParamsPtr, paramsSet)){
				arguments.input.Version_1_0->selectionParams = paramsSet;
			}
		}
	}

	return CreateGqlRequest<
				sdl::imtbase::ImtCollection::CreateSubCollectionRequestArguments,
				sdl::imtbase::ImtCollection::CCreateSubCollectionGqlRequest>(arguments).PopInterfacePtr();
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

	sdl::imtbase::ImtCollection::SetObjectNameRequestArguments arguments;
	arguments.input.Version_1_0.emplace();
	arguments.input.Version_1_0->collectionId = *m_collectionIdAttrPtr;
	arguments.input.Version_1_0->objectId = objectId;
	arguments.input.Version_1_0->name = name;

	return CreateGqlRequest<
				sdl::imtbase::ImtCollection::SetObjectNameRequestArguments,
				sdl::imtbase::ImtCollection::CSetObjectNameGqlRequest>(arguments).PopInterfacePtr();
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

	sdl::imtbase::ImtCollection::SetObjectDescriptionRequestArguments arguments;
	arguments.input.Version_1_0.emplace();
	arguments.input.Version_1_0->collectionId = *m_collectionIdAttrPtr;
	arguments.input.Version_1_0->objectId = objectId;
	arguments.input.Version_1_0->description = description;

	return CreateGqlRequest<
				sdl::imtbase::ImtCollection::SetObjectDescriptionRequestArguments,
				sdl::imtbase::ImtCollection::CSetObjectDescriptionGqlRequest>(arguments).PopInterfacePtr();
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
	sdl::imtbase::ImtCollection::CInsertNewObjectPayload::V1_0 insertNewObjectPayload;

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
	sdl::imtbase::ImtCollection::CGetObjectTypeIdPayload::V1_0 getObjectTypeIdPayload;
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
	sdl::imtbase::ImtCollection::CGetElementInfoPayload::V1_0 getElementInfoPayload;
	ResponseData responseData = GetResponseData(response);
	if (getElementInfoPayload.ReadFromJsonObject(responseData.data)){
		if (getElementInfoPayload.elementInfo){
			sdl::imtbase::ImtBaseTypes::CParameter::V1_0 parameter = *getElementInfoPayload.elementInfo;
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
	sdl::imtbase::ImtCollection::CGetObjectDataPayload::V1_0 getObjectDataPayload;
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
	sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload::V1_0 getElementMetaInfoPayload;
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
	sdl::imtbase::ImtCollection::CGetElementsCountPayload::V1_0 getElementsCountPayload;
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
	sdl::imtbase::ImtCollection::CGetElementIdsPayload::V1_0 getElementIdsPayload;
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

	sdl::imtbase::ImtCollection::CCreateSubCollectionPayload::V1_0 createSubCollectionPayload;
	ResponseData responseData = GetResponseData(response);
	if (createSubCollectionPayload.ReadFromJsonObject(responseData.data)){
		if (createSubCollectionPayload.items){
			imtsdl::TElementList<sdl::imtbase::ImtCollection::CSubCollectionItem::V1_0> subCollectionItems = *createSubCollectionPayload.items;
			for (const istd::TSharedNullable<sdl::imtbase::ImtCollection::CSubCollectionItem::V1_0>& subCollectionItem : subCollectionItems){
				QByteArray objectId = *subCollectionItem->itemInfo->id;
				QByteArray objectTypeId = *subCollectionItem->itemInfo->typeId;
				QString name = *subCollectionItem->itemInfo->name;
				QString description = *subCollectionItem->itemInfo->description;

				idoc::MetaInfoPtr dataMetainfoPtr;
				auto CreateMetaInfo = [&objectId, this](const QByteArray& typeId,
										 QList<imtbase::IMetaInfoCreator*> metaInfoCreatorList){
					idoc::MetaInfoPtr metaInfoPtr;
					for (int i = 0; i < metaInfoCreatorList.count(); i++){
						imtbase::IMetaInfoCreator* metaInfoCreatorPtr = metaInfoCreatorList[i];
						if (metaInfoCreatorPtr != nullptr){
							QByteArrayList typeIds = metaInfoCreatorPtr->GetSupportedTypeIds();
							if (typeIds.contains(typeId)){
								imtbase::IObjectCollection::DataPtr dataPtr;
								if (m_objectCollectionCompPtr.IsValid()){
									// TODO: Read full object ?
									m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr);
								}

								metaInfoCreatorPtr->CreateMetaInfo(dataPtr.GetPtr(), typeId, metaInfoPtr);
							}
						}
					}

					if (!metaInfoPtr.IsValid()){
						metaInfoPtr.SetPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>());
					}

					Q_ASSERT(metaInfoPtr.IsValid());

					return metaInfoPtr;
				};

				dataMetainfoPtr = CreateMetaInfo(objectTypeId, metaInfoCreatorList);

				idoc::CStandardDocumentMetaInfo metainfo;
				if (subCollectionItem->metaInfo){
					QByteArray metaInfoData = (*subCollectionItem->metaInfo).toUtf8();
					bool retVal = DeSerializeObject(&metainfo, metaInfoData);
					if (!retVal){
						qDebug() << "Deserialization of the meta.information was failed!";
					}
				}

				if (subCollectionItem->dataMetaInfo){
					QByteArray dataMetaInfo = (*subCollectionItem->dataMetaInfo).toUtf8();
					bool retVal = DeSerializeObject(dataMetainfoPtr.GetPtr(), dataMetaInfo);
					if (!retVal){
						qDebug() << "Deserialization of the object was failed!";
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
							nullptr,
							objectId,
							dataMetainfoPtr.GetPtr(),
							&metainfo,
							&operationContext);

				dataMetainfoPtr.SetPtr(nullptr);
			}
		}
	}

	return subCollectionPtr.PopPtr();
}


// reimplemented (icomp::CComponentBase)

void CGqlObjectCollectionDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
	m_paramSetRepresentationController.RegisterSubController(m_complexCollectionFilterRepresentationController);
	m_paramSetRepresentationController.RegisterSubController(m_documentFilterRepresentationController);
}


// private methods

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
		SendErrorMessage(0, QString("Unable to serialize object. Error: Object is nullptr"));
		return false;
	}

	const iser::ISerializable* objectConst = dynamic_cast<const iser::ISerializable*>(object);
	iser::ISerializable* serializableObject = dynamic_cast<iser::ISerializable*>(const_cast<iser::ISerializable*>(objectConst));

	if (serializableObject == nullptr){
		SendErrorMessage(0, QString("Unable to serialize object. Error: Object is not serializable"));
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
		SendErrorMessage(0, QString("Unable to serialize object. Error: Serialization failed"));
		return false;
	}
	objectData = archivePtr->GetData();

	return true;
}


bool CGqlObjectCollectionDelegateComp::DeSerializeObject(istd::IPolymorphic* object, const QByteArray& objectData) const
{
	if (object == nullptr){
		SendErrorMessage(0, QString("Unable to deserialize object. Error: Object is invalid"));
		return false;
	}

	iser::ISerializable* serializableObjectPtr = dynamic_cast<iser::ISerializable*>(object);
	if (serializableObjectPtr == nullptr){
		SendErrorMessage(0, QString("Unable to deserialize object. Error: Object is not serializable"));
		return false;
	}

	iser::CJsonMemReadArchive archive(objectData, !objectData.isEmpty());
	if (!serializableObjectPtr->Serialize(archive)){
		SendErrorMessage(0, QString("Unable to deserialize object. Error: Deserialization failed"));

		return false;
	}

	return true;
}


bool CGqlObjectCollectionDelegateComp::GetParamsSetRepresentation(
			const iprm::IParamsSet& paramsSet,
			sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0& representation) const
{
	QJsonObject jsonObject;
	if (m_paramSetRepresentationController.GetRepresentationFromDataModel(paramsSet, jsonObject)){
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
		requestPtr->SetGqlContext(dynamic_cast<imtgql::IGqlContext*>(gqlContextPtr->CloneMe().PopInterfacePtr()));
	}

	if (!SdlRequest::SetupGqlRequest(*requestPtr, arguments)){
		return nullptr;
	}

	return istd::TUniqueInterfacePtr<imtgql::IGqlRequest>(requestPtr);
}


} // namespace imtclientgql


