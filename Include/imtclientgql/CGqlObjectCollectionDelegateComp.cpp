#include <imtclientgql/CGqlObjectCollectionDelegateComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imod/TModelWrap.h>
#include <iprm/IIdParam.h>
//#include <iser/CJsonMemWriteArchive.h>
//#include <iser/CJsonMemReadArchive.h>
#include <iser/CMemoryWriteArchive.h>
#include <iser/CMemoryReadArchive.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtgql/CGqlRequest.h>

// GmgCloudDoc includes
#include <imtclientgql/CGqlObjectCollectionResponse.h>


namespace imtclientgql
{


// public methods

// reimplemented (IGqlObjectCollectionDelegate)

//QByteArrayList CGqlObjectCollectionDelegateComp::GetSupportedObjectTypeIds() const
//{
//	return { *m_objectTypeIdAttrPtr };
//}


//imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetElementType(const QByteArray& elementId) const
//{
//	return nullptr;
//}

bool CGqlObjectCollectionDelegateComp::GetObjectInfo(const QByteArray& objectId, imtgql::IGqlStructuredCollectionResponse::ObjectInfo& outInfo) const
{
//	QByteArrayList typeIds = GetSupportedObjectTypeIds();
//	if (typeIds.count() == 1){
//		outInfo.id = objectId;
//		outInfo.typeId = typeIds.at(0);

//		return true;
//	}

//	return false;

	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateGetObjectInfoRequest(objectId);
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
		istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
		if (responsePtr.IsValid()){
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
				QVariant variant;
				if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					if (variant.canConvert<QJsonObject>()){
						QJsonObject jsonObject = variant.value<QJsonObject>();
						jsonObject = jsonObject.value("metaInfo").toObject();
						for (QString key: jsonObject.keys()){
							int type = key.toInt();
							switch (type){
								case idoc::IDocumentMetaInfo::MIT_CREATION_TIME: case idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME:
								{
									QString timestr = jsonObject.value(key).toString();
									outInfo.SetMetaInfo(type, QDateTime::fromString(timestr, Qt::ISODate));
									break;
								}
								default:
									QByteArray data = jsonObject.value(key).toString().toUtf8();
									outInfo.SetMetaInfo(type, data);
							}
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
		istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
		if (responsePtr.IsValid()){
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
				QVariant variant;
				if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					if (variant.canConvert<QJsonObject>()){
						QJsonObject jsonObject = variant.value<QJsonObject>();
						jsonObject = jsonObject.value("dataMetaInfo").toObject();
						for (QString key: jsonObject.keys()){
							int type = key.toInt();
							switch (type){
								case idoc::IDocumentMetaInfo::MIT_CREATION_TIME: case idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME:
								{
									QString timestr = jsonObject.value(key).toString();
									outInfo.SetMetaInfo(type, QDateTime::fromString(timestr, Qt::ISODate));
									break;
								}
								default:
									QByteArray data = jsonObject.value(key).toString().toUtf8();
									outInfo.SetMetaInfo(type, data);
							}
						}
						return true;
					}
				}
			}
		}
	}

	return false;
}



QByteArray CGqlObjectCollectionDelegateComp::GetObjectTypeId(const QByteArray& objectId) const
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
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateInsertObjectRequest(typeId, name, description, &object, proposedObjectId);
		istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
		if (responsePtr.IsValid()){
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
				QVariant variant;
				if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					if (variant.type() == QVariant::Bool){
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
		istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
		responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
		if (responsePtr.IsValid()){
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
				QVariant variant;
				if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
					if (variant.type() == QVariant::ByteArray || variant.type() == QVariant::String){
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
			istd::TDelPtr<imtgql::IGqlRequest> requestPtr = CreateSetObjectRequest(objectId, &object);
			istd::TDelPtr<imtgql::IGqlPrimitiveTypeResponse> responsePtr;
			responsePtr.SetCastedOrRemove(CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessfull() && responsePtr->GetValue(variant)){
						if (variant.type() == QVariant::Bool){
							return variant.toBool();
						}
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
	const iser::ISerializable* objectConst = dynamic_cast<const iser::ISerializable*>(objectPtr);
	iser::ISerializable* object = dynamic_cast<iser::ISerializable*>(const_cast<iser::ISerializable*>(objectConst));
	if (object == nullptr){
		QByteArray errorMessage = QObject::tr("Object is not Serializable").toUtf8();
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}
	iser::CMemoryWriteArchive archive;
	if (!object->Serialize(archive)){
		QByteArray errorMessage = QObject::tr("Error when serializing an object").toUtf8();
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}
	QByteArray data((char*)archive.GetBuffer(), archive.GetBufferSize());

	QByteArray commandId = *m_collectionIdAttrPtr + "ObjectAdd";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_MUTATION, commandId);
	imtgql::CGqlObject input("input");
	input.InsertField("Id", QVariant(proposedObjectId));
	input.InsertField("item", QVariant(data.toBase64()));

	requestPtr->AddParam(input);

	imtgql::CGqlObject query("addedNotification");
	requestPtr->AddField(query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectRequest(const QByteArray& objectId) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "ObjectView";
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
	const iser::ISerializable* objectConst = dynamic_cast<const iser::ISerializable*>(objectPtr);
	iser::ISerializable* object = dynamic_cast<iser::ISerializable*>(const_cast<iser::ISerializable*>(objectConst));
	if (object == nullptr){
		QByteArray errorMessage = QObject::tr("Object is not Serializable").toUtf8();
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}
	iser::CMemoryWriteArchive archive;
	if (!object->Serialize(archive)){
		QByteArray errorMessage = QObject::tr("Error when serializing an object").toUtf8();
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}
	QByteArray data((char*)archive.GetBuffer(), archive.GetBufferSize());

	QByteArray commandId = *m_collectionIdAttrPtr + "ObjectUpdate";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_MUTATION, commandId);
	imtgql::CGqlObject input("input");
	input.InsertField("Id", QVariant(objectId));
	input.InsertField("item", QVariant(data.toBase64()));

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

	imtgql::CGqlObject data("succeeded");
	queryPtr->AddField(data);

	return queryPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetElementCountRequest(const iprm::IParamsSet* selectionParamsPtr) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "Count";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlObject input("input");
//	input.InsertField("id", QVariant(objectId));
////	input.InsertField("uploadUrl", QVariant(uploadUrl));
//	input.InsertField("version", QVariant(0));
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
//	const iprm::IIdParam* idParamPtr = dynamic_cast<const iprm::IIdParam*>(selectionParamsPtr->GetParameter("NodeId"));
//	Q_ASSERT(idParamPtr != nullptr);
	QByteArray commandId = *m_collectionIdAttrPtr + "Ids";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlObject input("input");
	requestPtr->AddParam(input);

	imtgql::CGqlObject query("itemIds");
	requestPtr->AddField(query);

	return requestPtr;
}


//imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateUploadUrlsRequest(const QStringList& fileNames, const QByteArray& nodeId) const
//{
//	Q_ASSERT(fileNames.count() > 0);

//	imtgql::CGqlRequest* requestPtr = nullptr;

//	if (fileNames.count() > 0) {
//		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "getDocumentUploadUrls");

//		imtgql::CGqlObject input("input");
//		input.InsertField("fileNames", fileNames);
//		input.InsertField("folderId", QVariant(nodeId));
//		requestPtr->AddParam(input);

//		imtgql::CGqlObject query("uploadUrls");
//		query.InsertField("fileName");
//		query.InsertField("url");
//		requestPtr->AddField(query);
//	}

//	return requestPtr;
//}


//imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateDownloadUrlsRequest(const QByteArrayList& objectIds) const
//{
//	Q_ASSERT(objectIds.count() == 1);

//	imtgql::CGqlRequest* requestPtr = nullptr;

//	if (objectIds.count() == 1) {
//		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "getDocumentDownloadUrl");

//		imtgql::CGqlObject input("input");
//		input.InsertField("id", QVariant(objectIds[0]));
//		input.InsertField("isCGATS", false);

//		imtgql::CGqlObject query("url");
//		requestPtr->AddField(query);

//		requestPtr->AddParam(input);
//	}

//	return requestPtr;
//}


//imtcom::IFileTransfer* CGqlObjectCollectionDelegateComp::GetFileTransfer() const
//{
//	if (m_fileTransferCompPtr.IsValid()) {
//		return m_fileTransferCompPtr.GetPtr();
//	}

//	return nullptr;
//}


imtgql::IGqlResponse* CGqlObjectCollectionDelegateComp::CreateResponse(const imtgql::IGqlRequest& request) const
{
	return new imtclientgql::CGqlObjectCollectionResponse();
}



} // namespace imtclientgql


