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
#include <iser/CMemoryWriteArchive.h>
#include <iser/CMemoryReadArchive.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/CFilterCollectionProxy.h>
#include <imtbase/COperationContext.h>
#include <imtgql/CGqlRequest.h>


namespace imtclientgql
{


// public methods


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectTypeIdRequest(const QByteArray& objectId) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "TypeId";

	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlParamObject input;
	input.InsertParam("id", QVariant(objectId));
	requestPtr->AddParam("input", input);

	imtgql::CGqlFieldObject query;
	requestPtr->AddField("objectTypeId", query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectInfoRequest(const QByteArray& objectId) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "Info";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlParamObject input;
	input.InsertParam("id", QVariant(objectId));
	requestPtr->AddParam("input", input);

	imtgql::CGqlFieldObject query;
	requestPtr->AddField("info", query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectMetaInfoRequest(const QByteArray& objectId) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "MetaInfo";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlParamObject input;
	input.InsertParam("id", QVariant(objectId));
	requestPtr->AddParam("input", input);

	imtgql::CGqlFieldObject query;
	requestPtr->AddField("metaInfo", query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectDataMetaInfoRequest(const QByteArray& objectId) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "DataMetaInfo";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlParamObject input;
	input.InsertParam("id", QVariant(objectId));
	requestPtr->AddParam("input", input);

	imtgql::CGqlFieldObject query;
	requestPtr->AddField("dataMetaInfo", query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateInsertObjectRequest(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			const istd::IChangeable* objectPtr,
			const QString& uploadUrl,
			const QByteArray& proposedObjectId,
			const QByteArray& nodeId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr) const
{
	QByteArray data;
	QByteArray commandId = *m_collectionIdAttrPtr + "Add";
	istd::TDelPtr<imtgql::CGqlRequest> requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_MUTATION, commandId);
	imtgql::CGqlParamObject input;
	input.InsertParam("typeId", QVariant(typeId));
	input.InsertParam("name", QVariant(name));
	input.InsertParam("description", QVariant(description));

	if (!SerializeObject(objectPtr, data)){
		SendErrorMessage(0,
					QString("Unable to create insert request for object with id '%1', typeId '%2'. Error: Serialization object failed")
						.arg(qPrintable(proposedObjectId), qPrintable(typeId)),
						"CGqlObjectCollectionDelegateComp");

		return nullptr;
	}

	input.InsertParam("item", QVariant(data.toBase64()));
	input.InsertParam("uploadUrl", QVariant(uploadUrl));
	input.InsertParam("id", QVariant(proposedObjectId));
	input.InsertParam("nodeId", QVariant(nodeId));

	if (dataMetaInfoPtr != nullptr){
		if (SerializeObject(dataMetaInfoPtr, data)){
			input.InsertParam("dataMetaInfo", QVariant(data.toBase64()));
		}
		else{
			SendWarningMessage(
					0,
					QString("Unable to serialize 'DataMetaInfo' for object '%1' with typeId '%2'").arg(qPrintable(proposedObjectId), qPrintable(typeId)),
					"CGqlObjectCollectionDelegateComp");
		}
	}

	if (collectionItemMetaInfoPtr != nullptr){
		if (SerializeObject(collectionItemMetaInfoPtr, data)){
			input.InsertParam("collectionItemMetaInfo", QVariant(data.toBase64()));
		}
		else{
			SendWarningMessage(
						0,
						QString("Unable to serialize 'CollectionItemMetaInfo' for object '%1' with typeId '%2'").arg(qPrintable(proposedObjectId), qPrintable(typeId)),
						"CGqlObjectCollectionDelegateComp");
		}
	}

	if (operationContextPtr != nullptr){
		if (SerializeObject(operationContextPtr, data)){
			input.InsertParam("operationContext", QVariant(data.toBase64()));
		}
		else{
			SendWarningMessage(
					0,
					QString("Unable to serialize 'OperationContext' for object '%1' with typeId '%2'").arg(qPrintable(proposedObjectId), qPrintable(typeId)),
					"CGqlObjectCollectionDelegateComp");
		}
	}

	requestPtr->AddParam("input", input);

	imtgql::CGqlFieldObject query;
	requestPtr->AddField("addedNotification", query);

	return requestPtr.PopPtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectRequest(const QByteArray& objectId) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "View";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlParamObject input;
	input.InsertParam("id", QVariant(objectId));
	requestPtr->AddParam("input", input);

	imtgql::CGqlFieldObject query;
	query.InsertField("Id");
	requestPtr->AddField("item", query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateSetObjectRequest(
			const QByteArray& objectId,
			const QByteArray& typeId,
			const istd::IChangeable* objectPtr,
			const QString& uploadUrl,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr,
			int clientVersion,
			const imtbase::IOperationContext* operationContextPtr) const
{
	QByteArray data;

	QByteArray commandId = *m_collectionIdAttrPtr + "Update";
	istd::TDelPtr<imtgql::CGqlRequest> requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_MUTATION, commandId);
	imtgql::CGqlParamObject input;
	input.InsertParam("id", QVariant(objectId));
	input.InsertParam("typeId", QVariant(typeId));

	if (!SerializeObject(objectPtr, data)){
		SendErrorMessage(0,
						 QString("Unable to create update request for object '%1' with type-ID '%2'. Error: Serialization object failed")
							 .arg(qPrintable(objectId), qPrintable(typeId)),
						 "CGqlObjectCollectionDelegateComp");
		return nullptr;
	}
	input.InsertParam("item", QVariant(data.toBase64()));
	input.InsertParam("uploadUrl", QVariant(uploadUrl));

	if (dataMetaInfoPtr != nullptr){
		if (SerializeObject(dataMetaInfoPtr, data)){
			input.InsertParam("dataMetaInfo", QVariant(data.toBase64()));
		}
		else{
			SendWarningMessage(
					0,
					QString("Unable to serialize 'DataMetaInfo' for object '%1' with typeId '%2'").arg(qPrintable(objectId), qPrintable(typeId)),
					"CGqlObjectCollectionDelegateComp");
		}
	}

	if (collectionItemMetaInfoPtr != nullptr){
		if (SerializeObject(collectionItemMetaInfoPtr, data)){
			input.InsertParam("collectionItemMetaInfo", QVariant(data.toBase64()));
		}
		else{
			SendWarningMessage(
						0,
						QString("Unable to serialize 'CollectionItemMetaInfo' for object '%1' with typeId '%2'").arg(qPrintable(objectId), qPrintable(typeId)),
						"CGqlObjectCollectionDelegateComp");
		}
	}

	if (operationContextPtr != nullptr){
		if (SerializeObject(operationContextPtr, data)){
			input.InsertParam("operationContext", QVariant(data.toBase64()));
		}
		else{
			SendWarningMessage(
						0,
						QString("Unable to serialize 'OperationContext' for object '%1' with typeId '%2'").arg(qPrintable(objectId), qPrintable(typeId)),
						"CGqlObjectCollectionDelegateComp");
		}
	}

	input.InsertParam("clientVersion", QVariant(clientVersion));

	requestPtr->AddParam("input", input);

	imtgql::CGqlFieldObject query;
	requestPtr->AddField("updatedNotification", query);

	return requestPtr.PopPtr();
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateRemoveObjectRequest(
			const QByteArray& objectId,
			int clientElementVersion,
			const imtbase::IOperationContext* operationContextPtr) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "Remove";
	QByteArray data;
	imtgql::CGqlRequest* queryPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_MUTATION, commandId);
	
	imtgql::CGqlParamObject input;
	input.InsertParam("id", QVariant(objectId));
	input.InsertParam("version", QVariant(clientElementVersion));
	if (operationContextPtr != nullptr){
		if (SerializeObject(operationContextPtr,data)){
			input.InsertParam("operationContext", QVariant(data.toBase64()));
		}
		else{
			SendWarningMessage(0,
							   QString("Unable to insert field 'operationContext' to the request '%1'. Error: Serialization failed")
								   .arg(qPrintable(commandId)),
							   "CGqlObjectCollectionDelegateComp");
		}
	}

	queryPtr->AddParam("input", input);

	imtgql::CGqlFieldObject query;
	queryPtr->AddField("removedNotification", query);

	return queryPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetElementCountRequest(const iprm::IParamsSet* selectionParamsPtr) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "Count";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlParamObject input;

	QByteArray data;
	if (selectionParamsPtr != nullptr){
		if (SerializeObject(selectionParamsPtr, data)){
			input.InsertParam("selectionParams", QVariant(data.toBase64()));
		}
		else{
			SendWarningMessage(0,
							   QString("Unable to insert field 'selectionParams' to the request '%1'. Error: Serialization failed")
								   .arg(qPrintable(commandId)),
							   "CGqlObjectCollectionDelegateComp");
		}
	}

	requestPtr->AddParam("input", input);

	imtgql::CGqlFieldObject query;
	requestPtr->AddField("itemsCount", query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetElementListRequest(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "Ids";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlParamObject input;
	input.InsertParam("offset", QVariant(offset));
	input.InsertParam("count", QVariant(count));
	QByteArray data;
	if (selectionParamsPtr != nullptr){
		if (SerializeObject(selectionParamsPtr, data)){
			input.InsertParam("selectionParams", QVariant(data.toBase64()));
		}
		else{
			SendWarningMessage(0,
							   QString("Unable to insert field 'selectionParams' to the request '%1'. Error: Serialization failed")
								   .arg(qPrintable(commandId)),
							   "CGqlObjectCollectionDelegateComp");
		}
	}
	requestPtr->AddParam("input", input);

	imtgql::CGqlFieldObject query;
	requestPtr->AddField("itemIds", query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetSubCollectionRequest(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "SubCollection";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlParamObject input;

	imtgql::CGqlParamObject viewParams;
	viewParams.InsertParam("offset", QVariant(offset));
	viewParams.InsertParam("count", QVariant(count));
	QByteArray data;
	if (selectionParamsPtr != nullptr){
		if (SerializeObject(selectionParamsPtr, data)){
			viewParams.InsertParam("selectionParams", QVariant(data.toBase64()));
		}
		else{
			SendWarningMessage(0,
							   QString("Unable to insert field 'selectionParams' to the request '%1'. Error: Serialization failed")
								   .arg(qPrintable(commandId)),
							   "CGqlObjectCollectionDelegateComp");
		}
	}

	input.InsertParam("viewParams", viewParams);

	requestPtr->AddParam("input", input);

	imtgql::CGqlFieldObject query;
	query.InsertField("info");
	query.InsertField("metaInfo");
	query.InsertField("dataMetaInfo");
	query.InsertField("operationContext");
	requestPtr->AddField("items", query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateSetObjectNameRequest(
			const QByteArray& objectId,
			const QString& name,
			int clientVersion,
			const imtbase::IOperationContext* operationContextPtr) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "SetName";
	QByteArray data;
	imtgql::CGqlRequest* queryPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_MUTATION, commandId);

	imtgql::CGqlParamObject input;
	input.InsertParam("id", QVariant(objectId));
	input.InsertParam("newName", QVariant(name));
	input.InsertParam("version", QVariant(clientVersion));

	if (operationContextPtr != nullptr){
		if (SerializeObject(operationContextPtr, data)){
			input.InsertParam("operationContext", QVariant(data.toBase64()));
		}
		else{
			SendWarningMessage(0,
							   QString("Unable to insert field 'operationContext' to the request '%1'. Error: Serialization failed")
								   .arg(qPrintable(commandId)),
							   "CGqlObjectCollectionDelegateComp");
		}
	}
	queryPtr->AddParam("input", input);

	imtgql::CGqlFieldObject query;
	queryPtr->AddField("rename", query);

	return queryPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateSetObjectDescriptionRequest(
			const QByteArray& objectId,
			const QString& description,
			int clientVersion,
			const imtbase::IOperationContext* operationContextPtr) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "SetDescription";
	QByteArray data;
	imtgql::CGqlRequest* queryPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_MUTATION, commandId);

	imtgql::CGqlParamObject input;
	input.InsertParam("id", QVariant(objectId));
	input.InsertParam("description", QVariant(description));
	input.InsertParam("version", QVariant(clientVersion));

	if (operationContextPtr != nullptr){
		if (SerializeObject(operationContextPtr, data)){
			input.InsertParam("operationContext", QVariant(data.toBase64()));
		}
		else{
			SendWarningMessage(0,
							   QString("Unable to insert field 'operationContext' to the request '%1'. Error: Serialization failed")
								   .arg(qPrintable(commandId)),
							   "CGqlObjectCollectionDelegateComp");
		}
	}

	queryPtr->AddParam("input", input);

	imtgql::CGqlFieldObject query;
	queryPtr->AddField("setDescription", query);

	return queryPtr;
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
	ResponseData responseData = GetResponseData(response);
	if (responseData.data.contains("id")){
		out = responseData.data["id"].toString().toLatin1();
		
		return true;
	}

	return false;
}


bool CGqlObjectCollectionDelegateComp::GetObjectTypeId(const imtgql::IGqlResponse& response, Id& out) const
{
	ResponseData responseData = GetResponseData(response);
	if (responseData.data.contains("typeId")){
		QJsonValue responseDataValue = responseData.data["typeId"];
		out = responseDataValue.toString().toUtf8();

		return true;
	}

	return false;
}


bool CGqlObjectCollectionDelegateComp::GetObjectInfo(const imtgql::IGqlResponse& response, ObjectInfo& out) const
{
	ResponseData responseData = GetResponseData(response);
	if (responseData.data.contains("info")){
		QJsonObject info = responseData.data["info"].toObject();

		out.id = info.value("id").toString().toLatin1();
		out.typeId = info.value("typeId").toString().toUtf8();
		out.version = info.value("version").toInt();
		out.name = info.value("name").toString();
		out.description = info.value("description").toString();

		return true;
	}

	return false;
}


bool CGqlObjectCollectionDelegateComp::GetObjectData(const imtgql::IGqlResponse& response, istd::IChangeable& out) const
{
	ResponseData responseData = GetResponseData(response);
	if (responseData.data.contains("objectData")){
		QJsonValue responseDataValue = responseData.data["objectData"];
		if (responseDataValue.isString()){
			QByteArray objectDataBase64 = responseDataValue.toString().toUtf8();
			QByteArray objectData = QByteArray::fromBase64(objectDataBase64);

			return DeSerializeObject(&out, objectData);
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateComp::GetMetaInfo(const imtgql::IGqlResponse& response, idoc::IDocumentMetaInfo& out) const
{
	ResponseData responseData = GetResponseData(response);
	QJsonValue responseDataValue;
	if (responseData.data.contains("metaInfo")){
		responseDataValue = responseData.data["metaInfo"];
	}
	else if (responseData.data.contains("dataMetaInfo")){
		responseDataValue = responseData.data["dataMetaInfo"];
	}

	if (responseDataValue.isString()){
		QByteArray metaInfoDataBase64 = responseDataValue.toString().toUtf8();
		QByteArray metaInfoData = QByteArray::fromBase64(metaInfoDataBase64);
		if (!metaInfoData.isEmpty()){
			return DeSerializeObject(&out, metaInfoData);
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateComp::GetItemCount(const imtgql::IGqlResponse& response, int& out) const
{
	ResponseData responseData = GetResponseData(response);
	if (responseData.data.contains("itemsCount")){
		QJsonValue itemsCountValue = responseData.data["itemsCount"];
		if (itemsCountValue.isDouble()){
			double retVal = itemsCountValue.toDouble();
			if (retVal >= 0){
				out = retVal;

				return true;
			}
		}
	}

	out = -1;

	return false;
}


bool CGqlObjectCollectionDelegateComp::GetItemIds(const imtgql::IGqlResponse& response, Ids& out) const
{
	ResponseData responseData = GetResponseData(response);
	if (responseData.data.contains("itemIds")){
		QJsonValue itemsIdsValue = responseData.data["itemIds"];
		if (itemsIdsValue.isString()){
			QStringList itemIds;
			QString itemsIdsValueStr = itemsIdsValue.toString();
			if (!itemsIdsValueStr.isEmpty()){
				itemIds = itemsIdsValueStr.split(";");
			}
			Ids retVal;
			for (const QString& itemId : itemIds){
				retVal.append(itemId.toLatin1());
			}

			out = retVal;

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

	ResponseData responseData = GetResponseData(response);
	QJsonValue responseDataValue;
	if (responseData.data.contains("items")){
		QJsonValue itemsValue = responseData.data.value("items");
		if (!itemsValue.isArray()){
			return nullptr;
		}

		QJsonArray items = itemsValue.toArray();
		for (int index = 0; index < items.count(); index++){
			QJsonObject jsonObject = items.at(index).toObject();
			QJsonObject info = jsonObject.value("info").toObject();
			QByteArray objectId = info.value("id").toString().toUtf8();
			QByteArray objectTypeId = info.value("typeId").toString().toUtf8();
			int version = info.value("version").toInt();
			Q_UNUSED(version);

			QString name = info.value("name").toString();
			QString description = info.value("description").toString();

			idoc::MetaInfoPtr dataMetainfoPtr;
			auto CreateMetaInfo = [&objectId, this, &objectTypeId](const QByteArray typeId,
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
					metaInfoPtr.SetCastedOrRemove(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>());
				}

				Q_ASSERT(metaInfoPtr.IsValid());

				return metaInfoPtr;
			};

			dataMetainfoPtr = CreateMetaInfo(objectTypeId, metaInfoCreatorList);

			idoc::CStandardDocumentMetaInfo metainfo;
			if (jsonObject.contains("metaInfo")){
				QByteArray metaInfoData = QByteArray::fromBase64(jsonObject.value("metaInfo").toString().toUtf8());
				bool retVal = DeSerializeObject(&metainfo, metaInfoData);
				if (!retVal){
					qDebug() << "Deserialization of the meta.information was failed!";
				}
			}

			if (jsonObject.contains("dataMetaInfo")){
				QByteArray dataMetaInfo = QByteArray::fromBase64(jsonObject.value("dataMetaInfo").toString().toUtf8());
				bool retVal = DeSerializeObject(dataMetainfoPtr.GetPtr(), dataMetaInfo);
				if (!retVal){
					qDebug() << "Deserialization of the object was failed!";
				}
			}

			imtbase::COperationContext operationContext;
			if (jsonObject.contains("operationContext")){
				QByteArray operationContextData = QByteArray::fromBase64(jsonObject.value("operationContext").toString().toUtf8());

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

	return subCollectionPtr.PopPtr();
}


// private methods

CGqlObjectCollectionDelegateComp::ResponseData CGqlObjectCollectionDelegateComp::GetResponseData(const imtgql::IGqlResponse& response) const
{
	ResponseData data;
	imtgql::IGqlResponse::GqlRequestPtr requestPtr = response.GetOriginalRequest();
	if (!requestPtr.isNull()){
		data.commandId = requestPtr->GetCommandId();

		if (!data.data.contains(data.commandId)){
			SendErrorMessage(0, "GqlResponse data don't contains " + data.commandId);
		}
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

	istd::TDelPtr<iser::CMemoryWriteArchive> archivePtr;
	if (m_versionInfoCompPtr.IsValid()){
		archivePtr.SetPtr(new iser::CMemoryWriteArchive(m_versionInfoCompPtr.GetPtr()));
	}
	else{
		archivePtr.SetPtr(new iser::CMemoryWriteArchive());
	}

	if (!serializableObject->Serialize(*archivePtr.GetPtr())){
		SendErrorMessage(0, QString("Unable to serialize object. Error: Serialization failed"));
		return false;
	}
	else{
		objectData = QByteArray((char*)archivePtr->GetBuffer(), archivePtr->GetBufferSize());
	}

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

	iser::CMemoryReadArchive archive(objectData.data(), objectData.length());
	if (!serializableObjectPtr->Serialize(archive)){
		SendErrorMessage(0, QString("Unable to deserialize object. Error: Deserialization failed"));

		return false;
	}

	return true;
}


} // namespace imtclientgql


