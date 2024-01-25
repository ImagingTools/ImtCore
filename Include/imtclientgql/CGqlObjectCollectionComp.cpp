#include <imtclientgql/CGqlObjectCollectionComp.h>


// Qt includes
#include <QtNetwork/QNetworkReply>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imod/TModelWrap.h>
#include <iser/CMemoryWriteArchive.h>
#include <iser/CMemoryReadArchive.h>

// ImtCore includes
#include <imtbase/CTempDir.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlResponse.h>
#include <imtbase/CFilterCollectionProxy.h>
#include <imtbase/COperationContext.h>


namespace imtclientgql
{


// public methods

CGqlObjectCollectionComp::CGqlObjectCollectionComp()
{
}


// reimplemented (IObjectCollection)

const imtbase::IRevisionController* CGqlObjectCollectionComp::GetRevisionController() const
{
	return nullptr;
}


const imtbase::ICollectionDataController* CGqlObjectCollectionComp::GetDataController() const
{
	return nullptr;
}


int CGqlObjectCollectionComp::GetOperationFlags(const Id& elementId) const
{
	if (!elementId.isEmpty()){
		return OF_SUPPORT_DELETE | OF_SUPPORT_EDIT;
	}

	return OF_SUPPORT_INSERT | OF_SUPPORT_DELETE | OF_SUPPORT_EDIT | OF_SUPPORT_PAGINATION;
}


imtbase::ICollectionInfo::Id CGqlObjectCollectionComp::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			DataPtr defaultValuePtr,
			const QByteArray& proposedObjectId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr)
{

	if (m_delegateCompPtr.IsValid()){
		istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
		imtbase::ICollectionInfo::ElementInsertInfo insertInfo;

		QByteArray objectId = proposedObjectId.isEmpty() ? QUuid::createUuid().toByteArray() : proposedObjectId;

		insertInfo.elementId = objectId;
		changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED, QVariant::fromValue(insertInfo));
		istd::CChangeNotifier notifier(this, &changeSet);

		if (m_clientCompPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlRequest> requestPtr = m_delegateCompPtr->CreateInsertObjectRequest(
				typeId,
				name,
				description,
				defaultValuePtr.GetPtr(),
				"", // uploadUrl
				proposedObjectId,
				"", // nodeId,
				dataMetaInfoPtr,
				collectionItemMetaInfoPtr,
				operationContextPtr);

			if (!requestPtr.IsValid()){
				return QByteArray();
			}

			istd::TDelPtr<imtgql::IGqlResponse> responsePtr;
			responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessful()){
						variant = responsePtr->GetResult();
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

		if (objectId.isEmpty()){
			notifier.Abort();
		}

		return objectId;
	}

	return QByteArray();
}


bool CGqlObjectCollectionComp::RemoveElement(const Id& elementId, const imtbase::IOperationContext* operationContextPtr)
{
	imtclientgql::IGqlObjectCollectionResponse::ObjectInfo info;
	if (GetObjectInfo(elementId, info)){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = m_delegateCompPtr->CreateRemoveObjectRequest(elementId, info.version, operationContextPtr);
		if (requestPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlResponse> responsePtr;
			responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessful()){
						return true;
					}
				}
			}
		}
	}

	return false;
}


const istd::IChangeable* CGqlObjectCollectionComp::GetObjectPtr(const Id& /*objectId*/) const
{
	return nullptr;
}


bool CGqlObjectCollectionComp::GetObjectData(const Id& objectId, DataPtr& dataPtr) const
{
	QByteArray typeId = GetObjectTypeId(objectId);

	DataPtr retVal = GetObject(objectId, typeId);
	if (retVal.IsValid()){
		dataPtr = retVal;

		return true;
	}

	return false;
}


bool CGqlObjectCollectionComp::SetObjectData(
			const Id& objectId,
			const istd::IChangeable& object,
			CompatibilityMode /*mode*/,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_delegateCompPtr.IsValid()){
		imtclientgql::IGqlObjectCollectionResponse::ObjectInfo info;
		if (GetObjectInfo(objectId, info)){
			istd::TDelPtr<imtgql::IGqlRequest> requestPtr = m_delegateCompPtr->CreateSetObjectRequest(
				objectId,
				&object,
				"",
				nullptr,
				nullptr,
				0,
				operationContextPtr);
			if (!requestPtr.IsValid()){
				return false;
			}

			istd::TDelPtr<imtgql::IGqlResponse> responsePtr;
			responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){

					if (responsePtr->IsSuccessful()){
						QVariant variant = responsePtr->GetResult();
						istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
						imtbase::ICollectionInfo::ElementUpdateInfo updateInfo;
						updateInfo.elementId = objectId;
						changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_UPDATED, QVariant::fromValue(updateInfo));
						istd::CChangeNotifier notifier(this, &changeSet);

						return true;
					}
				}
			}
		}
	}

	return false;
}


imtbase::IObjectCollection* CGqlObjectCollectionComp::CreateSubCollection(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (m_delegateCompPtr.IsValid()){
		imtbase::IObjectCollection* subcollection = const_cast<imtbase::IObjectCollection*>(dynamic_cast<const imtbase::IObjectCollection*>(this));
		imtbase::CFilterCollectionProxy* retVal = new imtbase::CFilterCollectionProxy(*subcollection);

		if (!m_clientCompPtr.IsValid() || subcollection == nullptr){
			return nullptr;
		}

		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = m_delegateCompPtr->CreateGetSubCollectionRequest(offset, count, selectionParamsPtr);
		istd::TDelPtr<imtgql::IGqlResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		if (responsePtr.IsValid()){
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
				if (responsePtr->IsSuccessful()){
					QVariant variant = responsePtr->GetResult();
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

							idoc::MetaInfoPtr dataMetainfoPtr;

							if (m_metaInfoCreatorCompPtr.IsValid()){
								QByteArray typeId = GetObjectTypeId(objectId);
								m_metaInfoCreatorCompPtr->CreateMetaInfo(nullptr, typeId, dataMetainfoPtr);
							}
							else{
								dataMetainfoPtr.SetPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>());
							}

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

							retVal->InsertNewObject(
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

						return retVal;
					}

				}
			}
		}

		return nullptr;
	}

	return nullptr;
}


imtbase::IObjectCollectionIterator* CGqlObjectCollectionComp::CreateObjectCollectionIterator(
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	return nullptr;
}


// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CGqlObjectCollectionComp::GetObjectTypesInfo() const
{
	return &m_typesInfo;
}



QByteArray CGqlObjectCollectionComp::GetObjectTypeId(const Id& objectId) const
{
	// if (m_delegateCompPtr.IsValid()){
	// 	return m_delegateCompPtr->GetObjectTypeId(objectId);
	// }

	// return QByteArray();

	QByteArrayList ids;
	if (m_typeIdsAttrPtr.IsValid()) {
		for (int i = 0; i < m_typeIdsAttrPtr.GetCount(); i++){
			QByteArray id = m_typeIdsAttrPtr[i];
			if (!id.isEmpty()) {
				if (!ids.contains(id)) {
					ids.append(id);
				}
			}
		}
	}

	if (ids.count() == 1){
		return ids.at(0);
	}

	return QByteArray();

}


idoc::MetaInfoPtr CGqlObjectCollectionComp::GetDataMetaInfo(const Id& objectId) const
{
	if (m_delegateCompPtr.IsValid()){
		idoc::MetaInfoPtr outInfo;
		if (m_clientCompPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlRequest> requestPtr = m_delegateCompPtr->CreateGetObjectDataMetaInfoRequest(objectId);
			if (!requestPtr.IsValid()){
				return idoc::MetaInfoPtr();
			}

			istd::TDelPtr<imtgql::IGqlResponse> responsePtr;
			responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					if (responsePtr->IsSuccessful()){
						QVariant variant = responsePtr->GetResult();
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
							DeSerializeObject(outInfo.GetPtr(), objectData);

							return outInfo;
						}
					}
				}
			}
		}

	}

	return idoc::MetaInfoPtr();
}


// reimplemented (ICollectionInfo)

int CGqlObjectCollectionComp::GetElementsCount(const iprm::IParamsSet* selectionParamsPtr, ilog::IMessageConsumer* logPtr) const
{
	if (!m_delegateCompPtr.IsValid()){
		return -1;
	}

	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = m_delegateCompPtr->CreateGetElementCountRequest(selectionParamsPtr);
		istd::TDelPtr<imtgql::IGqlResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		if (responsePtr.IsValid()){
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
				if (responsePtr->IsSuccessful()){
					QVariant variant = responsePtr->GetResult();
#if QT_VERSION >= 0x060000
					if (variant.typeId() == QMetaType::Bool || variant.typeId() == QMetaType::LongLong){
#else
					if (variant.type() == QMetaType::Bool || variant.type() == QMetaType::LongLong){
#endif
						return variant.toInt();
					}
				}
			}
		}
	}

	return -1;
}


imtbase::ICollectionInfo::Ids CGqlObjectCollectionComp::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* logPtr) const
{
	imtbase::ICollectionInfo::Ids retVal;

	if (m_delegateCompPtr.IsValid()){
		if (m_clientCompPtr.IsValid()){
			istd::TDelPtr<imtgql::IGqlRequest> requestPtr = m_delegateCompPtr->CreateGetElementListRequest(offset, count, selectionParamsPtr);
			istd::TDelPtr<imtclientgql::IGqlObjectCollectionResponse> responsePtr;
			responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
			if (responsePtr.IsValid()){
				if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
					QVariant variant;
					if (responsePtr->IsSuccessful()){
						imtclientgql::IGqlObjectCollectionResponse::ObjectList list;
						if (responsePtr->GetObjectList(list)){
							for (const imtclientgql::IGqlObjectCollectionResponse::ObjectInfo& element : list){
								retVal += element.id;
							}
						}
					}
				}
			}
		}


	}

	return retVal;
}


bool CGqlObjectCollectionComp::GetSubsetInfo(
			ICollectionInfo& /*subsetInfo*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return false;
}


QVariant CGqlObjectCollectionComp::GetElementInfo(const Id& elementId, int infoType, ilog::IMessageConsumer* logPtr) const
{
	imtclientgql::IGqlObjectCollectionResponse::ObjectInfo info;
	bool isValid = false;

	if (GetObjectInfo(elementId, info)){
		isValid = true;
	}

	if (isValid){
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


idoc::MetaInfoPtr CGqlObjectCollectionComp::GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr) const
{
	idoc::MetaInfoPtr metaInfoPtr;

	imtclientgql::IGqlObjectCollectionResponse::ObjectInfo info;

	if (GetObjectInfo(elementId, info)){
		if (!info.dataMetaInfoPtr.isNull()){
			metaInfoPtr.SetCastedOrRemove(info.dataMetaInfoPtr->CloneMe());
		}
	}
	else{

		if (!GetObjectMetaInfo(elementId, metaInfoPtr)){
			metaInfoPtr.Reset();
		}
	}

	return metaInfoPtr;
}


bool CGqlObjectCollectionComp::SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* logPtr)
{
	if (m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr;
		istd::TDelPtr<imtgql::IGqlResponse> responsePtr;

		if (m_clientCompPtr.IsValid()){
			imtclientgql::IGqlObjectCollectionResponse::ObjectInfo info;
			if (GetObjectInfo(elementId, info)){
				istd::TDelPtr<imtgql::IGqlRequest> requestPtr = m_delegateCompPtr->CreateSetObjectNameRequest(elementId, name, info.version);
				istd::TDelPtr<imtgql::IGqlResponse> responsePtr;
				responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
				if (responsePtr.IsValid()){
					if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
						if (responsePtr->IsSuccessful()){
							QVariant variant = responsePtr->GetResult();
#if QT_VERSION >= 0x060000
							if (variant.typeId() == QMetaType::Bool){
#else
							if (variant.type() == QMetaType::Bool){
#endif
								return variant.toBool();
							}
						}
					}
				}
			}
		}

		return false;
	}

	return false;
}


bool CGqlObjectCollectionComp::SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr)
{
	if (m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr;
		istd::TDelPtr<imtgql::IGqlResponse> responsePtr;

		if (m_clientCompPtr.IsValid()){
			imtclientgql::IGqlObjectCollectionResponse::ObjectInfo info;
			if (GetObjectInfo(elementId, info)){
				istd::TDelPtr<imtgql::IGqlRequest> requestPtr = m_delegateCompPtr->CreateSetObjectDescriptionRequest(elementId, description, info.version);
				istd::TDelPtr<imtgql::IGqlResponse> responsePtr;
				responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
				if (responsePtr.IsValid()){
					if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
						if (responsePtr->IsSuccessful()){
							QVariant variant = responsePtr->GetResult();
#if QT_VERSION >= 0x060000
							if (variant.typeId() == QMetaType::Bool){
#else
							if (variant.type() == QMetaType::Bool){
#endif
								return variant.toBool();
							}
						}
					}
				}
			}
		}

		return false;
	}

	return false;
}


bool CGqlObjectCollectionComp::SetElementEnabled(const Id& /*elementId*/, bool /*isEnabled*/, ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


// reimplemented (gmgaws::ISubscriptionClient)

void CGqlObjectCollectionComp::OnResponseReceived(const QByteArray& /*subscriptionId*/, const QByteArray& /*subscriptionData*/)
{
	istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
	imtbase::ICollectionInfo::ElementInsertInfo insertInfo;
	changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED, QVariant::fromValue(insertInfo));
	istd::CChangeNotifier notifier(this, &changeSet);
}


void CGqlObjectCollectionComp::OnSubscriptionStatusChanged(
			const QByteArray& /*subscriptionId*/,
			const SubscriptionStatus& /*status*/,
			const QString& /*message*/)
{
}


// protected methods

imtbase::IObjectCollection::DataPtr CGqlObjectCollectionComp::CreateObjectInstance(const QByteArray& typeId) const
{
	int count = m_typesInfo.GetOptionsCount();
	for (int i = 0; i < count; i++){
		if (m_typeIdsAttrPtr[i] == typeId){
			icomp::IComponent* compPtr = m_objectFactoriesCompPtr.CreateComponent(i);
			return DataPtr(DataPtr::RootObjectPtr(compPtr), [compPtr, this](){
				return m_objectFactoriesCompPtr.ExtractInterface(compPtr);
			});
		}
	}

	return DataPtr();
}


// reimplemented (icomp::CComponentBase)

void CGqlObjectCollectionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int count = qMin(m_typeIdsAttrPtr.GetCount(), m_typeNamesAttrPtr.GetCount());
	count = qMin(count, m_objectFactoriesCompPtr.GetCount());

	for (int i = 0; i < count; ++i){
		QByteArray typeId = m_typeIdsAttrPtr[i];
		QString typeName = m_typeNamesAttrPtr[i];

		m_typesInfo.InsertOption(typeName, typeId);
	}
}


void CGqlObjectCollectionComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


// private methods

imtbase::IObjectCollection::DataPtr CGqlObjectCollectionComp::GetObject(
			const QByteArray& objectId,
			const QByteArray& typeId) const
{
	if (m_delegateCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr documentPtr = CreateObjectInstance(typeId);
		if (documentPtr.IsValid()){
			if (m_clientCompPtr.IsValid()){
				istd::TDelPtr<imtgql::IGqlRequest> requestPtr = m_delegateCompPtr->CreateGetObjectRequest(objectId);
				if (!requestPtr.IsValid()){
					return imtbase::IObjectCollection::DataPtr();
				}

				istd::TDelPtr<imtgql::IGqlResponse> responsePtr;
				responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
				if (responsePtr.IsValid()){
					if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
						if (responsePtr->IsSuccessful()){
							QVariant variant = responsePtr->GetResult();
#if QT_VERSION < 0x060000
							if (variant.type() == QMetaType::QByteArray || variant.type() == QMetaType::QString){
#else
							if (variant.typeId() == QMetaType::QByteArray || variant.typeId() == QMetaType::QString){
#endif
								QByteArray objectData = QByteArray::fromBase64(variant.toByteArray());
								DeSerializeObject(documentPtr.GetPtr(), objectData);

								return documentPtr;
							}
						}
					}

				}
			}

		}
	}

	return imtbase::IObjectCollection::DataPtr();
}


bool CGqlObjectCollectionComp::GetObjectInfo(
			const QByteArray& objectId,
			imtclientgql::IGqlObjectCollectionResponse::ObjectInfo& valueOut) const
{
	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = m_delegateCompPtr->CreateGetObjectInfoRequest(objectId);
		if (!requestPtr.IsValid()){
			return false;
		}

		istd::TDelPtr<imtgql::IGqlResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		if (responsePtr.IsValid()){
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
				if (responsePtr->IsSuccessful()){
					QVariant variant = responsePtr->GetResult();
					if (variant.canConvert<QJsonObject>()){
						QJsonObject object = variant.value<QJsonObject>();
						object = object.value("info").toObject();
						valueOut.id = objectId;
						valueOut.typeId = object.value("typeId").toString().toUtf8();
						valueOut.version = object.value("version").toInt();
						valueOut.name = object.value("name").toString();
						valueOut.description = object.value("description").toString();

						return true;
					}
				}
			}
		}
	}

	return false;
	return false;
}


bool CGqlObjectCollectionComp::GetObjectMetaInfo(const QByteArray& objectId, idoc::MetaInfoPtr& valueOut) const
{
	if (!m_delegateCompPtr.IsValid()){
		return false;
	}

	valueOut.SetPtr(new idoc::CStandardDocumentMetaInfo());

	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = m_delegateCompPtr->CreateGetObjectMetaInfoRequest(objectId);
		if (!requestPtr.IsValid()){
			return false;
		}

		istd::TDelPtr<imtgql::IGqlResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		if (responsePtr.IsValid()){
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
				if (responsePtr->IsSuccessful()){
					QVariant variant = responsePtr->GetResult();
					if (variant.canConvert<QJsonObject>()){
						QJsonObject jsonObject = variant.value<QJsonObject>();
						QByteArray objectData = QByteArray::fromBase64(jsonObject.value("metaInfo").toString().toUtf8());

						return DeSerializeObject(valueOut.GetPtr(), objectData);
					}
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionComp::GetObjectDataMetaInfo(const QByteArray& objectId, idoc::MetaInfoPtr& valueOut) const
{
	if (!m_delegateCompPtr.IsValid()){
		return false;
	}

	if (m_clientCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr = m_delegateCompPtr->CreateGetObjectDataMetaInfoRequest(objectId);
		if (!requestPtr.IsValid()){
			return false;
		}

		istd::TDelPtr<imtgql::IGqlResponse> responsePtr;
		responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
		if (responsePtr.IsValid()){
			if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
				if (responsePtr->IsSuccessful()){
					QVariant variant = responsePtr->GetResult();
					if (variant.canConvert<QJsonObject>()){
						QJsonObject jsonObject = variant.value<QJsonObject>();
						QByteArray objectData = QByteArray::fromBase64(jsonObject.value("dataMetaInfo").toString().toUtf8());
						if (m_metaInfoCreatorCompPtr.IsValid()){
							QByteArray typeId = GetObjectTypeId(objectId);
							m_metaInfoCreatorCompPtr->CreateMetaInfo(nullptr, typeId, valueOut);
						}
						else{
							valueOut.SetPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>());
						}

						return DeSerializeObject(valueOut.GetPtr(), objectData);
					}
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionComp::SerializeObject(const istd::IPolymorphic* object, QByteArray& objectData) const
{
	objectData.clear();

	if (object == nullptr){
		return false;
	}

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


bool CGqlObjectCollectionComp::DeSerializeObject(istd::IPolymorphic* object, const QByteArray& objectData) const
{
	if (object == nullptr){
		return false;
	}

	iser::ISerializable* serializableObject = dynamic_cast<iser::ISerializable*>(object);
	if (serializableObject == nullptr){
		QByteArray errorMessage = QString("Object data metainfo is not serializable").toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}

	iser::CMemoryReadArchive archive(objectData.data(), objectData.count());
	if (!serializableObject->Serialize(archive)){
		QByteArray errorMessage = QString("Error when serializing an object").toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}

	return true;
}



} // namespace imtclientgql


