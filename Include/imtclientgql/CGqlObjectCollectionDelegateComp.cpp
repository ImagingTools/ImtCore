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


bool CGqlObjectCollectionDelegateComp::GetObjectMetaInfo(const QByteArray& objectId, idoc::IDocumentMetaInfo& outInfo) const
{
	outInfo.ResetData();

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
						iser::ISerializable* object = dynamic_cast<iser::ISerializable*>(&outInfo);
						if (object == nullptr){
							QByteArray errorMessage = QObject::tr("Object metainfo is not Serializable").toUtf8();
							SendErrorMessage(0, errorMessage);

							return false;
						}
						iser::CMemoryReadArchive archive(objectData.data(), objectData.count());
						if (!object->Serialize(archive)){
							QByteArray errorMessage = QObject::tr("Error when serializing an object").toUtf8();
							SendErrorMessage(0, errorMessage);

							return false;
						}

						return true;

					}

				}
			}
		}

	}

	return false;
}


bool CGqlObjectCollectionDelegateComp::GetObjectDataMetaInfo(const QByteArray& objectId, idoc::IDocumentMetaInfo& outInfo) const
{
	outInfo.ResetData();

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
						iser::ISerializable* object = dynamic_cast<iser::ISerializable*>(&outInfo);
						if (object == nullptr){
							QByteArray errorMessage = QObject::tr("Object data metainfo is not Serializable").toUtf8();
							SendErrorMessage(0, errorMessage);

							return false;
						}
						iser::CMemoryReadArchive archive(objectData.data(), objectData.count());
						if (!object->Serialize(archive)){
							QByteArray errorMessage = QObject::tr("Error when serializing an object").toUtf8();
							SendErrorMessage(0, errorMessage);

							return false;
						}

						return true;

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
#if QT_VERSION < 0x060000
					if (variant.type() == QMetaType::Bool){
#else
					if (variant.typeId() == QMetaType::Bool){
#endif
						return QByteArray();
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
						iser::ISerializable* object = dynamic_cast<iser::ISerializable*>(objectPtr.GetPtr());
						if (object == nullptr){
							QByteArray errorMessage = QObject::tr("Object is not Serializable").toUtf8();
							SendErrorMessage(0, errorMessage);

							return false;
						}
						iser::CMemoryReadArchive archive(objectData.data(), objectData.count());
						if (!object->Serialize(archive)){
							QByteArray errorMessage = QObject::tr("Error when serializing an object").toUtf8();
							SendErrorMessage(0, errorMessage);

							return false;
						}

						return true;
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
	imtgql::CGqlRequest* queryPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_MUTATION, "Remove");
	
	imtgql::CGqlObject id;
	id.InsertField("Id", QVariant(objectId));
	queryPtr->AddParam(id);

	imtgql::CGqlObject version;
	version.InsertField("version", QVariant(clientElementVersion));
	queryPtr->AddParam(version);

	imtgql::CGqlObject operationContext;
	QByteArray data;
	SerializeObject(operationContextPtr,data);
	operationContext.InsertField("operationContext", QVariant(data.toBase64()));
	queryPtr->AddParam(operationContext);

	imtgql::CGqlObject succeeded("succeeded");
	queryPtr->AddField(succeeded);

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


