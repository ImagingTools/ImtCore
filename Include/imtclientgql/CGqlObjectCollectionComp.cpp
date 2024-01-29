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
#include <imtgql/IGqlResponse.h>
#include <imtbase/CFilterCollectionProxy.h>
#include <imtbase/COperationContext.h>


namespace imtclientgql
{


typedef QSharedPointer<imtgql::IGqlRequest> GqlRequestPtr;
typedef QSharedPointer<imtgql::IGqlResponse> GqlResponsePtr;


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
	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return Id();
	}

	Id retVal;

	GqlRequestPtr requestPtr(m_delegateCompPtr->CreateInsertObjectRequest(
				typeId,
				name,
				description,
				defaultValuePtr.GetPtr(),
				"", // uploadUrl
				proposedObjectId,
				"", // nodeId,
				dataMetaInfoPtr,
				collectionItemMetaInfoPtr,
				operationContextPtr));

	if (!requestPtr.isNull()){
		GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (!responsePtr.isNull()){
			if (m_delegateCompPtr->GetObjectId(*responsePtr, retVal)){
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


bool CGqlObjectCollectionComp::RemoveElement(const Id& elementId, const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return false;
	}

	bool retVal = false;

	GqlRequestPtr requestPtr(m_delegateCompPtr->CreateRemoveObjectRequest(elementId, -1, operationContextPtr));

	if (!requestPtr.isNull()){
		GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (!responsePtr.isNull()){
			m_delegateCompPtr->GetOperationResult(*responsePtr, retVal);
		}
	}

	return retVal;
}


const istd::IChangeable* CGqlObjectCollectionComp::GetObjectPtr(const Id& /*objectId*/) const
{
	return nullptr;
}


bool CGqlObjectCollectionComp::GetObjectData(const Id& objectId, DataPtr& dataPtr) const
{
	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return false;
	}

	QByteArray typeId = GetObjectTypeId(objectId);

	DataPtr retVal = CreateObjectInstance(typeId);
	if (retVal.IsValid()){
		GqlRequestPtr requestPtr(m_delegateCompPtr->CreateGetObjectRequest(objectId));
		if (!requestPtr.isNull()){
			GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
			if (!responsePtr.isNull()){
				if (m_delegateCompPtr->GetObjectData(*responsePtr, *retVal)){
					dataPtr = retVal;

					return true;
				}
			}
		}
	}

	return false;
}


bool CGqlObjectCollectionComp::SetObjectData(
			const Id& objectId,
			const istd::IChangeable& object,
			CompatibilityMode /*mode*/,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return false;
	}
	
	bool retVal = false;

	imtclientgql::IGqlObjectCollectionDelegate::ObjectInfo info;
	if (GetObjectInfo(objectId, info)){
		GqlRequestPtr requestPtr(m_delegateCompPtr->CreateSetObjectRequest(
					objectId,
					&object,
					"",
					nullptr,
					nullptr,
					info.version,
					operationContextPtr));

		if (!requestPtr.isNull()){
			GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
			if (!responsePtr.isNull()){
				if (m_delegateCompPtr->GetOperationResult(*responsePtr, retVal)){
					if (retVal){
						istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
						imtbase::ICollectionInfo::ElementUpdateInfo updateInfo;
						updateInfo.elementId = objectId;
						changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_UPDATED, QVariant::fromValue(updateInfo));
						istd::CChangeNotifier notifier(this, &changeSet);
					}
				}
			}
		}
	}

	return retVal;
}


imtbase::IObjectCollection* CGqlObjectCollectionComp::CreateSubCollection(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	//if (m_delegateCompPtr.IsValid()){
	//	imtbase::IObjectCollection* subcollection = const_cast<imtbase::IObjectCollection*>(dynamic_cast<const imtbase::IObjectCollection*>(this));
	//	imtbase::CFilterCollectionProxy* retVal = new imtbase::CFilterCollectionProxy(*subcollection);

	//	if (!m_clientCompPtr.IsValid() || subcollection == nullptr){
	//		return nullptr;
	//	}

	//	istd::TDelPtr<imtgql::IGqlRequest> requestPtr = m_delegateCompPtr->CreateGetSubCollectionRequest(offset, count, selectionParamsPtr);
	//	istd::TDelPtr<imtgql::IGqlResponse> responsePtr;
	//	responsePtr.SetCastedOrRemove(m_delegateCompPtr->CreateResponse(*requestPtr));
	//	if (responsePtr.IsValid()){
	//		if (m_clientCompPtr->SendRequest(*requestPtr, *responsePtr)){
	//			if (responsePtr->IsSuccessful()){
	//				QVariant variant = responsePtr->GetResult();
	//				if (variant.canConvert<QJsonObject>()){
	//					QJsonObject jsonObject = variant.value<QJsonObject>();
	//					QJsonArray items = jsonObject.value("items").toArray();
	//					for (int index = 0; index < items.count(); index++){
	//						jsonObject = items.at(index).toObject();
	//						QJsonObject info = jsonObject.value("info").toObject();
	//						QByteArray objectId = info.value("id").toString().toUtf8();
	//						QByteArray typeId = info.value("typeId").toString().toUtf8();
	//						int version = info.value("version").toInt();
	//						QString name = info.value("name").toString();
	//						QString description = info.value("description").toString();

	//						idoc::MetaInfoPtr dataMetainfoPtr;

	//						if (m_metaInfoCreatorCompPtr.IsValid()){
	//							QByteArray typeId = GetObjectTypeId(objectId);
	//							m_metaInfoCreatorCompPtr->CreateMetaInfo(nullptr, typeId, dataMetainfoPtr);
	//						}
	//						else{
	//							dataMetainfoPtr.SetPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>());
	//						}

	//						idoc::CStandardDocumentMetaInfo metainfo;
	//						if (jsonObject.contains("metaInfo")){
	//							QByteArray metaInfoData = QByteArray::fromBase64(jsonObject.value("metaInfo").toString().toUtf8());
	//							bool retVal = DeSerializeObject(&metainfo, metaInfoData);
	//							if (!retVal){
	//								qDebug() << "Deserialization of the meta.information was failed!";
	//							}
	//						}

	//						if (jsonObject.contains("dataMetaInfo")){
	//							QByteArray dataMetaInfo = QByteArray::fromBase64(jsonObject.value("dataMetaInfo").toString().toUtf8());
	//							bool retVal = DeSerializeObject(dataMetainfoPtr.GetPtr(), dataMetaInfo);
	//							if (!retVal){
	//								qDebug() << "Deserialization of the object was failed!";
	//							}
	//						}

	//						imtbase::COperationContext operationContext;
	//						if (jsonObject.contains("operationContext")){
	//							QByteArray operationContextData = QByteArray::fromBase64(jsonObject.value("operationContext").toString().toUtf8());

	//							DeSerializeObject(&operationContext, operationContextData);
	//						}

	//						retVal->InsertNewObject(
	//									typeId,
	//									name,
	//									description,
	//									nullptr,
	//									objectId,
	//									dataMetainfoPtr.GetPtr(),
	//									&metainfo,
	//									&operationContext);

	//						dataMetainfoPtr.SetPtr(nullptr);
	//					}

	//					return retVal;
	//				}

	//			}
	//		}
	//	}

	//	return nullptr;
	//}

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
	idoc::MetaInfoPtr retVal;

	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return retVal;
	}

	GqlRequestPtr requestPtr(m_delegateCompPtr->CreateGetObjectDataMetaInfoRequest(objectId));
	if (!requestPtr.isNull()){
		GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (!responsePtr.isNull()){
			idoc::MetaInfoPtr metaInfoPtr;
			if (m_metaInfoCreatorCompPtr.IsValid()){
				QByteArray typeId = GetObjectTypeId(objectId);
				m_metaInfoCreatorCompPtr->CreateMetaInfo(nullptr, typeId, metaInfoPtr);
			}
			else{
				metaInfoPtr.SetPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>());
			}

			if (m_delegateCompPtr->GetMetaInfo(*responsePtr, *metaInfoPtr)){
				retVal = metaInfoPtr;
			}
		}
	}

	return retVal;
}


// reimplemented (ICollectionInfo)

int CGqlObjectCollectionComp::GetElementsCount(const iprm::IParamsSet* selectionParamsPtr, ilog::IMessageConsumer* logPtr) const
{
	int retVal = -1;

	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return retVal;
	}

	GqlRequestPtr requestPtr(m_delegateCompPtr->CreateGetElementCountRequest(selectionParamsPtr));
	if (!requestPtr.isNull()){
		GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (!responsePtr.isNull()){
			int count;
			if (m_delegateCompPtr->GetItemCount(*responsePtr, count)){
				retVal = count;
			}
		}
	}

	return retVal;
}


imtbase::ICollectionInfo::Ids CGqlObjectCollectionComp::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* logPtr) const
{
	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return Ids();
	}

	Ids retVal;

	GqlRequestPtr requestPtr(m_delegateCompPtr->CreateGetElementListRequest(offset, count, selectionParamsPtr));
	if (!requestPtr.isNull()){
		GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (!responsePtr.isNull()){
			Ids ids;
			if (m_delegateCompPtr->GetItemIds(*responsePtr, ids)){
				retVal = ids;
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


idoc::MetaInfoPtr CGqlObjectCollectionComp::GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr) const
{
	idoc::MetaInfoPtr metaInfoPtr;

	imtclientgql::IGqlObjectCollectionDelegate::ObjectInfo info;

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
	bool retVal = false;

	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return retVal;
	}

	imtclientgql::IGqlObjectCollectionDelegate::ObjectInfo info;
	if (GetObjectInfo(elementId, info)){
		GqlRequestPtr requestPtr(m_delegateCompPtr->CreateSetObjectNameRequest(elementId, name, info.version));
		GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (!responsePtr.isNull()){
			m_delegateCompPtr->GetOperationResult(*responsePtr, retVal);
		}
	}

	return retVal;
}


bool CGqlObjectCollectionComp::SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr)
{
	bool retVal = false;

	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return retVal;
	}

	imtclientgql::IGqlObjectCollectionDelegate::ObjectInfo info;
	if (GetObjectInfo(elementId, info)){
		GqlRequestPtr requestPtr(m_delegateCompPtr->CreateSetObjectDescriptionRequest(elementId, description, info.version));
		GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (!responsePtr.isNull()){
			m_delegateCompPtr->GetOperationResult(*responsePtr, retVal);
		}
	}

	return retVal;
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

bool CGqlObjectCollectionComp::GetObjectInfo(
			const QByteArray& objectId,
			imtclientgql::IGqlObjectCollectionDelegate::ObjectInfo& valueOut) const
{
	bool retVal = false;

	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return retVal;
	}

	GqlRequestPtr requestPtr(m_delegateCompPtr->CreateGetObjectInfoRequest(objectId));
	if (!requestPtr.isNull()){
		GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (!responsePtr.isNull()){
			retVal = m_delegateCompPtr->GetObjectInfo(*responsePtr, valueOut);
			if (retVal){
				retVal = valueOut.id == objectId;
			}
		}
	}

	return retVal;
}


bool CGqlObjectCollectionComp::GetObjectMetaInfo(const QByteArray& objectId, idoc::MetaInfoPtr& valueOut) const
{
	bool retVal = false;

	if (!m_clientCompPtr.IsValid() || !m_delegateCompPtr.IsValid()){
		return retVal;
	}

	GqlRequestPtr requestPtr(m_delegateCompPtr->CreateGetObjectMetaInfoRequest(objectId));
	if (!requestPtr.isNull()){
		GqlResponsePtr responsePtr = m_clientCompPtr->SendRequest(requestPtr);
		if (!responsePtr.isNull()){
			idoc::MetaInfoPtr metaInfoPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>());
			if (m_delegateCompPtr->GetMetaInfo(*responsePtr, *metaInfoPtr)){
				valueOut = metaInfoPtr;

				retVal = true;
			}
		}
	}

	return retVal;
}


} // namespace imtclientgql


