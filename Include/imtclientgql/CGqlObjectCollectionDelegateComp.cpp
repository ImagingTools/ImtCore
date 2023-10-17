#include <imtclientgql/CGqlObjectCollectionDelegateComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imod/TModelWrap.h>
#include <iprm/IIdParam.h>
#include <iser/CMemoryWriteArchive.h>
#include <iser/CMemoryReadArchive.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtgql/CGqlRequest.h>
#include <imtclientgql/CGqlObjectCollectionResponse.h>
#include <imtbase/CFilterCollectionProxy.h>
#include <imtbase/COperationContext.h>


namespace imtclientgql
{


// public methods

bool CGqlObjectCollectionDelegateComp::GetObjectInfo(
			const QByteArray& objectId,
			imtgql::IGqlStructuredCollectionResponse::ObjectInfo& outInfo) const
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateGetObjectInfoRequest(objectId);
		if (!requestPtr.IsValid()){
			return false;
		}

		istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
		if (responsePtr.IsValid()){
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
				QVariant variant;
				if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					if (variant.canConvert<QJsonObject>()){
						QJsonObject object = variant.value<QJsonObject>();
						object = object.value("info").toObject();
						outInfo.id = objectId;
						outInfo.typeId = object.value("typeId").toString().toUtf8();
						outInfo.version = object.value("version").toInt();
						outInfo.name = object.value("name").toString();
						outInfo.description = object.value("description").toString();

						return true;
					}
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateComp::GetObjectMetaInfo(const QByteArray& objectId, idoc::MetaInfoPtr& outInfo) const
{
	outInfo.SetPtr(new idoc::CStandardDocumentMetaInfo());

	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateGetObjectMetaInfoRequest(objectId);
		if (!requestPtr.IsValid()){
			return false;
		}

		istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
		if (responsePtr.IsValid()){
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
				QVariant variant;
				if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){					
					if (variant.canConvert<QJsonObject>()){
						QJsonObject jsonObject = variant.value<QJsonObject>();
						QByteArray objectData = QByteArray::fromBase64(jsonObject.value("metaInfo").toString().toUtf8());

						return DeSerializeObject(outInfo.GetPtr(), objectData);
					}

				}
			}
		}

	}

	return false;
}


bool CGqlObjectCollectionDelegateComp::GetObjectDataMetaInfo(const QByteArray& objectId, idoc::MetaInfoPtr& outInfo) const
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateGetObjectDataMetaInfoRequest(objectId);
		if (!requestPtr.IsValid()){
			return false;
		}

		istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
		if (responsePtr.IsValid()){
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
				QVariant variant;
				if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					if (variant.canConvert<QJsonObject>()){
						QJsonObject jsonObject = variant.value<QJsonObject>();
						QByteArray objectData = QByteArray::fromBase64(jsonObject.value("dataMetaInfo").toString().toUtf8());
						if (m_metaInfoCreatorCompPtr.IsValid()){
							QByteArray typeId = GetObjectTypeId(objectId);
							m_metaInfoCreatorCompPtr->CreateMetaInfo(nullptr, typeId, outInfo);
						}
						else{
							outInfo.SetPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>());
						}

						return DeSerializeObject(outInfo.GetPtr(), objectData);
					}
				}
			}
		}
	}

	return false;
}


QByteArray CGqlObjectCollectionDelegateComp::GetObjectTypeId(const QByteArray& /*objectId*/) const
{
	QByteArrayList typeIds = GetSupportedObjectTypeIds();
	if (typeIds.count() == 1){
		return typeIds.at(0);
	}

	return QByteArray();
}


QByteArray CGqlObjectCollectionDelegateComp::InsertObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable& object,
				const QByteArray& proposedObjectId,
				const QByteArray& nodeId,
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr,
				const imtbase::IOperationContext* operationContextPtr) const
{
	if (m_clientCompPtr.IsValid()){
		imtgql::IGqlStructuredCollectionResponse::ObjectInfo info;
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateInsertObjectRequest(
					typeId,
					name,
					description,
					&object,
					"",
					proposedObjectId,
					nodeId,
					dataMetaInfoPtr,
					elementMetaInfoPtr,
					operationContextPtr);

		if (!requestPtr.IsValid()){
			return QByteArray();
		}

		istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
		if (responsePtr.IsValid()){
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
				QVariant variant;
				if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					if (variant.canConvert<QJsonObject>()){
						QJsonObject jsonObject = variant.value<QJsonObject>();
						jsonObject = jsonObject.value("addedNotification").toObject();
						QByteArray objectId = jsonObject.value("Id").toString().toUtf8();
						return objectId;
					}
				}
			}

		}
	}

	return QByteArray();
}


bool CGqlObjectCollectionDelegateComp::GetObjectData(
			const QByteArray& objectId,
			const QByteArray& typeId,
			imtbase::IObjectCollection::DataPtr objectPtr) const
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateGetObjectRequest(objectId);
		if (!requestPtr.IsValid()){
			return false;
		}

		istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
		if (responsePtr.IsValid()){
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
				QVariant variant;
				if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
#if QT_VERSION < 0x060000
					if (variant.type() == QMetaType::QByteArray || variant.type() == QMetaType::QString){
#else
					if (variant.typeId() == QMetaType::QByteArray || variant.typeId() == QMetaType::QString){
#endif
						QByteArray objectData = QByteArray::fromBase64(variant.toByteArray());

						return DeSerializeObject(objectPtr.GetPtr(), objectData);
					}
				}
			}

		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateComp::SetObjectData(
			const QByteArray& objectId,
			const QByteArray& typeId,
			const istd::IChangeable& object,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr) const
{
	if (m_clientCompPtr.IsValid()){
		imtgql::IGqlStructuredCollectionResponse::ObjectInfo info;
		if (GetObjectInfo(objectId, info)){
			istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateSetObjectRequest(
						objectId,
						&object,
						"",
						dataMetaInfoPtr,
						collectionItemMetaInfoPtr,
						0,
						operationContextPtr);
			if (!requestPtr.IsValid()){
				return false;
			}

			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
						return true;
					}
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionDelegateComp::GetSubCollection(
			imtbase::IObjectCollection* subcollection,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (!m_clientCompPtr.IsValid() || subcollection == nullptr){
		return false;
	}

	istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateGetSubCollectionRequest(offset, count, selectionParamsPtr);
	istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
	responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
	if (responsePtr.IsValid()){
		if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
			QVariant variant;
			if (responsePtr->IsSuccessfull()){
				QVariant variant;
				if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					if (variant.canConvert<QJsonObject>()){
						QJsonObject jsonObject = variant.value<QJsonObject>();
						QJsonArray items = jsonObject.value("items").toArray();
						for (int index = 0; index < items.count(); index++){
							jsonObject = items.at(index).toObject();
							QJsonObject info = jsonObject.value("info").toObject();
							QByteArray objectId = info.value("id").toString().toUtf8();
							QByteArray typeId = info.value("typeId").toString().toUtf8();
							int version = info.value("version").toInt();
							QString name = info.value("name").toString();
							QString description = info.value("description").toString();

							imtbase::COperationContext operationContext;
							idoc::CStandardDocumentMetaInfo metainfo;
							idoc::MetaInfoPtr dataMetainfoPtr;

							if (m_metaInfoCreatorCompPtr.IsValid()){
								QByteArray typeId = GetObjectTypeId(objectId);
								m_metaInfoCreatorCompPtr->CreateMetaInfo(nullptr, typeId, dataMetainfoPtr);
							}
							else{
								dataMetainfoPtr.SetPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>());
							}

							QByteArray objectData = QByteArray::fromBase64(jsonObject.value("metaInfo").toString().toUtf8());
							bool res = DeSerializeObject(&metainfo, objectData);
							Q_ASSERT(res);
							objectData = QByteArray::fromBase64(jsonObject.value("dataMetaInfo").toString().toUtf8());
							res = DeSerializeObject(dataMetainfoPtr.GetPtr(), objectData);
							Q_ASSERT(res);
							objectData = QByteArray::fromBase64(jsonObject.value("operationContext").toString().toUtf8());
							DeSerializeObject(&operationContext, objectData);

							subcollection->InsertNewObject(
										typeId,
										name,
										description,
										nullptr,
										objectId,
										dataMetainfoPtr.GetPtr(),
										&metainfo,
										&operationContext);

							dataMetainfoPtr.SetPtr(nullptr);
						}

						return true;
					}
				}
			}
		}
	}

}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectInfoRequest(const QByteArray& objectId) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "Info";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlObject input("input");
	input.InsertField("Id", QVariant(objectId));
	requestPtr->AddParam(input);

	imtgql::CGqlObject query("info");
	requestPtr->AddField(query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectMetaInfoRequest(const QByteArray& objectId) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "MetaInfo";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlObject input("input");
	input.InsertField("Id", QVariant(objectId));
	requestPtr->AddParam(input);

	imtgql::CGqlObject query("metaInfo");
	requestPtr->AddField(query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectDataMetaInfoRequest(const QByteArray& objectId) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "DataMetaInfo";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlObject input("input");
	input.InsertField("Id", QVariant(objectId));
	requestPtr->AddParam(input);

	imtgql::CGqlObject query("dataMetaInfo");
	requestPtr->AddField(query);

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
//	const iser::ISerializable* objectConst = dynamic_cast<const iser::ISerializable*>(objectPtr);
//	iser::ISerializable* object = dynamic_cast<iser::ISerializable*>(const_cast<iser::ISerializable*>(objectConst));
//	if (object != nullptr){
//		iser::CMemoryWriteArchive archive;
//		if (!object->Serialize(archive)){
//			QByteArray errorMessage = QObject::tr("Error when serializing an object").toUtf8();
//			SendErrorMessage(0, errorMessage);
//		}
//		else{
//			data = QByteArray((char*)archive.GetBuffer(), archive.GetBufferSize());
//		}
//	}

	QByteArray commandId = *m_collectionIdAttrPtr + "Add";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_MUTATION, commandId);
	imtgql::CGqlObject input("input");
	input.InsertField("typeId", QVariant(typeId));
	input.InsertField("name", QVariant(name));
	input.InsertField("description", QVariant(description));
	SerializeObject(objectPtr,data);
	input.InsertField("item", QVariant(data.toBase64()));
	input.InsertField("uploadUrl", QVariant(uploadUrl));
	input.InsertField("Id", QVariant(proposedObjectId));
	input.InsertField("nodeId", QVariant(nodeId));
	SerializeObject(dataMetaInfoPtr,data);
	input.InsertField("dataMetaInfo", QVariant(data.toBase64()));
	SerializeObject(collectionItemMetaInfoPtr,data);
	input.InsertField("collectionItemMetaInfo", QVariant(data.toBase64()));
	SerializeObject(operationContextPtr,data);
	input.InsertField("operationContext", QVariant(data.toBase64()));

	requestPtr->AddParam(input);

	imtgql::CGqlObject query("addedNotification");
	requestPtr->AddField(query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectRequest(const QByteArray& objectId) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "View";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlObject input("input");
	input.InsertField("Id", QVariant(objectId));
	requestPtr->AddParam(input);

	imtgql::CGqlObject query("item");
	requestPtr->AddField(query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateSetObjectRequest(
			const QByteArray& objectId,
			const istd::IChangeable* objectPtr,
			const QString& uploadUrl,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr,
			int clientVersion,
			const imtbase::IOperationContext* operationContextPtr) const
{
	QByteArray data;

	QByteArray commandId = *m_collectionIdAttrPtr + "Update";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_MUTATION, commandId);
	imtgql::CGqlObject input("input");
	input.InsertField("Id", QVariant(objectId));
	SerializeObject(objectPtr,data);
	input.InsertField("item", QVariant(data.toBase64()));
	input.InsertField("uploadUrl", QVariant(uploadUrl));
	SerializeObject(dataMetaInfoPtr,data);
	input.InsertField("dataMetaInfo", QVariant(data.toBase64()));
	SerializeObject(collectionItemMetaInfoPtr,data);
	input.InsertField("collectionItemMetaInfo", QVariant(data.toBase64()));
	input.InsertField("clientVersion", QVariant(clientVersion));
	SerializeObject(operationContextPtr,data);
	input.InsertField("operationContext", QVariant(data.toBase64()));

	requestPtr->AddParam(input);

	imtgql::CGqlObject query("updatedNotification");
	requestPtr->AddField(query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateRemoveObjectRequest(
			const QByteArray& objectId,
			int clientElementVersion,
			const imtbase::IOperationContext* operationContextPtr) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "Remove";
	QByteArray data;
	imtgql::CGqlRequest* queryPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_MUTATION, commandId);
	
	imtgql::CGqlObject input("input");
	input.InsertField("Id", QVariant(objectId));
	input.InsertField("version", QVariant(clientElementVersion));
	SerializeObject(operationContextPtr,data);
	input.InsertField("operationContext", QVariant(data.toBase64()));
	queryPtr->AddParam(input);

	imtgql::CGqlObject query("removedNotification");
	queryPtr->AddField(query);

	return queryPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetElementCountRequest(const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "Count";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlObject input("input");
	requestPtr->AddParam(input);

	imtgql::CGqlObject query("itemsCount");
	requestPtr->AddField(query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetElementListRequest(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "Ids";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlObject input("input");
	input.InsertField("offset", QVariant(offset));
	input.InsertField("count", QVariant(count));
	QByteArray data;
	SerializeObject(selectionParamsPtr,data);
	input.InsertField("selectionParams", QVariant(data.toBase64()));
	requestPtr->AddParam(input);

	imtgql::CGqlObject query("itemIds");
	requestPtr->AddField(query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetSubCollectionRequest(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "SubCollection";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlObject input("input");

	imtgql::CGqlObject viewParams("viewParams");
	viewParams.InsertField("offset", QVariant(offset));
	viewParams.InsertField("count", QVariant(count));
	QByteArray data;
	SerializeObject(selectionParamsPtr,data);
	viewParams.InsertField("selectionParams", QVariant(data.toBase64()));
	input.InsertField("viewParams", viewParams);

	requestPtr->AddParam(input);

	imtgql::CGqlObject query("items");
	query.InsertField("info");
	query.InsertField("metaInfo");
	query.InsertField("dataMetaInfo");
	query.InsertField("operationContext");
	requestPtr->AddField(query);

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

	imtgql::CGqlObject input("input");
	input.InsertField("id", QVariant(objectId));
	input.InsertField("newName", QVariant(name));
	input.InsertField("version", QVariant(clientVersion));
	SerializeObject(operationContextPtr,data);
	input.InsertField("operationContext", QVariant(data.toBase64()));
	queryPtr->AddParam(input);

	imtgql::CGqlObject query("rename");
	queryPtr->AddField(query);

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

	imtgql::CGqlObject input("input");
	input.InsertField("id", QVariant(objectId));
	input.InsertField("description", QVariant(description));
	input.InsertField("version", QVariant(clientVersion));
	SerializeObject(operationContextPtr,data);
	input.InsertField("operationContext", QVariant(data.toBase64()));
	queryPtr->AddParam(input);

	imtgql::CGqlObject query("setDescription");
	queryPtr->AddField(query);

	return queryPtr;
}



imtgql::IGqlResponse* CGqlObjectCollectionDelegateComp::CreateResponse(const imtgql::IGqlRequest& /*request*/) const
{
	return new imtclientgql::CGqlObjectCollectionResponse();
}


bool CGqlObjectCollectionDelegateComp::SerializeObject(const istd::IPolymorphic* object, QByteArray& objectData) const
{
	objectData.clear();

	const iser::ISerializable* objectConst = dynamic_cast<const iser::ISerializable*>(object);
	iser::ISerializable* serializableObject = dynamic_cast<iser::ISerializable*>(const_cast<iser::ISerializable*>(objectConst));

	if (serializableObject == nullptr){
		QByteArray errorMessage = QObject::tr("Object data metainfo is not Serializable").toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}
	iser::CMemoryWriteArchive archive;
	if (!serializableObject->Serialize(archive)){
		QByteArray errorMessage = QObject::tr("Error when serializing an object").toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}
	else{
		objectData = QByteArray((char*)archive.GetBuffer(), archive.GetBufferSize());
	}

	return true;
}


bool CGqlObjectCollectionDelegateComp::DeSerializeObject(istd::IPolymorphic* object, const QByteArray& objectData) const
{
	iser::ISerializable* serializableObject = dynamic_cast<iser::ISerializable*>(object);
	if (serializableObject == nullptr){
		QByteArray errorMessage = QObject::tr("Object data metainfo is not Serializable").toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}
	iser::CMemoryReadArchive archive(objectData.data(), objectData.count());
	if (!serializableObject->Serialize(archive)){
		QByteArray errorMessage = QObject::tr("Error when serializing an object").toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}

	return true;
}


} // namespace imtclientgql


