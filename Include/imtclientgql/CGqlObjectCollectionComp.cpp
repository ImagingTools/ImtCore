#include <imtclientgql/CGqlObjectCollectionComp.h>


// Qt includes
#include <QtNetwork/QNetworkReply>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/CTempDir.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlPrimitiveTypeResponse.h>
#include <imtgql/IGqlStructuredCollectionResponse.h>
#include <imtbase/CFilterCollectionProxy.h>


namespace imtclientgql
{


bool operator==(
			const imtgql::IGqlStructuredCollectionResponse::ElementInfo::PathItem& a,
			const imtgql::IGqlStructuredCollectionResponse::ElementInfo::PathItem& b)
{
	bool retVal = a.id == b.id && a.name == b.name;

	return retVal;
}


bool operator==(
			const imtgql::IGqlStructuredCollectionResponse::ElementInfo& a,
			const imtgql::IGqlStructuredCollectionResponse::ElementInfo& b)
{
	bool retVal =
				a.id == b.id &&
				a.name == b.name &&
				a.description == b.description &&
				a.path == b.path &&
				a.typeId == b.typeId &&
				a.version == b.version &&
				a.elementMetaInfoPtr.isNull() == b.elementMetaInfoPtr.isNull() &&
				a.elementMetaInfoPtr.isNull() == b.elementMetaInfoPtr.isNull() &&
				a.isNode == b.isNode;

	if (retVal && !a.elementMetaInfoPtr.isNull()){
		retVal &= a.elementMetaInfoPtr->IsEqual(*b.elementMetaInfoPtr);
	}

	if (retVal && !a.dataMetaInfoPtr.isNull()){
		retVal &= a.dataMetaInfoPtr->IsEqual(*b.dataMetaInfoPtr);
	}

	return retVal;
}


// public methods

CGqlObjectCollectionComp::CGqlObjectCollectionComp()
{
}


QByteArray CGqlObjectCollectionComp::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			IObjectCollection::DataPtr defaultValuePtr,
			const QByteArray& proposedObjectId,
			const QByteArray& parentId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* elementMetaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (m_delegateCompPtr.IsValid()){
		istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
		imtbase::ICollectionInfo::ElementInsertInfo insertInfo;

		QByteArray objectId = proposedObjectId.isEmpty() ? QUuid::createUuid().toByteArray() : proposedObjectId;

		insertInfo.elementId = objectId;
		changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED, QVariant::fromValue(insertInfo));
		istd::CChangeNotifier notifier(this, &changeSet);

		objectId = m_delegateCompPtr->InsertObject(
					typeId,
					name,
					description,
					*defaultValuePtr,
					objectId,
					parentId,
					dataMetaInfoPtr,
					elementMetaInfoPtr,
					operationContextPtr);
		if (objectId.isEmpty()){
			notifier.Abort();
		}

		return objectId;
	}

	return QByteArray();
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
	// return imtbase::ICollectionInfo::Id();
	return InsertNewObject(
				typeId,
				name,
				description,
				defaultValuePtr,
				proposedObjectId,
				QByteArray(),
				dataMetaInfoPtr,
				collectionItemMetaInfoPtr,
				operationContextPtr);
}


bool CGqlObjectCollectionComp::RemoveElement(const Id& elementId, const imtbase::IOperationContext* operationContextPtr)
{
	if (m_delegateCompPtr.IsValid()){
		if (m_delegateCompPtr->RemoveObject(elementId, operationContextPtr)){
			return true;
		}
		else{
			return m_delegateCompPtr->RemoveNode(elementId, operationContextPtr);
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
		imtgql::IGqlStructuredCollectionResponse::ObjectInfo info;
		if (GetObjectInfo(objectId, info)){
			if (m_delegateCompPtr->SetObjectData(
						objectId,
						info.typeId,
						object,
						nullptr,
						nullptr,
						operationContextPtr)){
				istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
				imtbase::ICollectionInfo::ElementUpdateInfo updateInfo;
				updateInfo.elementId = objectId;
				changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_UPDATED, QVariant::fromValue(updateInfo));
				istd::CChangeNotifier notifier(this, &changeSet);

				return true;
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
		imtbase::IObjectCollection* collection = const_cast<imtbase::IObjectCollection*>(dynamic_cast<const imtbase::IObjectCollection*>(this));
		imtbase::CFilterCollectionProxy* retVal = new imtbase::CFilterCollectionProxy(*collection);

		m_delegateCompPtr->GetSubCollection(
					retVal,
					offset,
					count,
					selectionParamsPtr);

		return retVal;
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
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->GetObjectTypeId(objectId);
	}

	return QByteArray();
}


idoc::MetaInfoPtr CGqlObjectCollectionComp::GetDataMetaInfo(const Id& objectId) const
{
	if (m_delegateCompPtr.IsValid()){
		idoc::MetaInfoPtr retVal;

		if (m_delegateCompPtr->GetObjectDataMetaInfo(objectId, retVal)){
			return retVal;
		}
	}

	return idoc::MetaInfoPtr();
}


// reimplemented (ICollectionInfo)

int CGqlObjectCollectionComp::GetElementsCount(const iprm::IParamsSet* selectionParamsPtr, ilog::IMessageConsumer* logPtr) const
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->GetElementCount(selectionParamsPtr);
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
		imtgql::IGqlStructuredCollectionResponse::ElementList list = m_delegateCompPtr->GetElementList(
					offset,
					count,
					selectionParamsPtr);

		for (const imtgql::IGqlStructuredCollectionResponse::ElementInfo& element : list){
			retVal += element.id;
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
	imtgql::IGqlStructuredCollectionResponse::ObjectInfo info;
	bool isValid = false;

	if (GetObjectInfo(elementId, info)){
		isValid = true;
	}
	else if (GetNodeInfo(elementId, info)){
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

	imtgql::IGqlStructuredCollectionResponse::ObjectInfo info;

	if (GetNodeInfo(elementId, info)){
		if (!info.elementMetaInfoPtr.isNull()){
			metaInfoPtr.SetCastedOrRemove(info.elementMetaInfoPtr->CloneMe());
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

		if (m_delegateCompPtr->SetObjectName(elementId, name)){
			return true;
		}
		else{
			return m_delegateCompPtr->SetNodeName(elementId, name);
		}
	}

	return false;
}


bool CGqlObjectCollectionComp::SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr)
{
	if (m_delegateCompPtr.IsValid()){
		istd::TDelPtr<imtgql::IGqlRequest> requestPtr;
		istd::TDelPtr<imtgql::IGqlResponse> responsePtr;

		if (m_delegateCompPtr->SetObjectDescription(elementId, description)){
			return true;
		}
		else{
			return m_delegateCompPtr->SetNodeDescription(elementId, description);
		}
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
			if (m_delegateCompPtr->GetObjectData(objectId, typeId, documentPtr)){
				return documentPtr;
			}
		}
	}

	return imtbase::IObjectCollection::DataPtr();
}


bool CGqlObjectCollectionComp::GetNodeInfo(
			const QByteArray& nodeId,
			imtgql::IGqlStructuredCollectionResponse::NodeInfo& valueOut) const
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->GetNodeInfo(nodeId, valueOut);
	}

	return false;
}


bool CGqlObjectCollectionComp::GetObjectInfo(
			const QByteArray& objectId,
			imtgql::IGqlStructuredCollectionResponse::ObjectInfo& valueOut) const
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->GetObjectInfo(objectId, valueOut);
	}

	return false;
}


bool CGqlObjectCollectionComp::GetObjectMetaInfo(const QByteArray& objectId, idoc::MetaInfoPtr& valueOut) const
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->GetObjectMetaInfo(objectId, valueOut);
	}

	return false;
}


bool CGqlObjectCollectionComp::GetObjectDataMetaInfo(const QByteArray& objectId, idoc::MetaInfoPtr& valueOut) const
{
	if (m_delegateCompPtr.IsValid()){
		return m_delegateCompPtr->GetObjectDataMetaInfo(objectId, valueOut);
	}

	return false;
}


} // namespace imtclientgql


