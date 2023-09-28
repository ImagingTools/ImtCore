#include <imtclientgql/CGqlObjectCollectionInfo.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtgql/IGqlPrimitiveTypeResponse.h>
#include <imtgql/IGqlStructuredCollectionResponse.h>


namespace imtclientgql
{


// public methods

CGqlObjectCollectionInfo::CGqlObjectCollectionInfo()
	:m_clientPtr(nullptr),
	m_delegatePtr(nullptr)
{
}


int CGqlObjectCollectionInfo::GetItemIndex(const QByteArray& id) const
{
	for (int i = 0; i < m_items.count(); ++i){
		if (m_items[i].id == id){
			return i;
		}
	}

	return -1;
}


void CGqlObjectCollectionInfo::SetGqlClient(const IGqlClient* gqlClientPtr)
{
	m_clientPtr = gqlClientPtr;
}


void CGqlObjectCollectionInfo::SetGqlDelegate(const IGqlObjectCollectionDelegate* gqlObjectCollectionDelegate)
{
	m_delegatePtr = gqlObjectCollectionDelegate;

	m_typeIds.ResetData();
	QByteArrayList ids = m_delegatePtr->GetSupportedObjectTypeIds();
	for (const QByteArray& id : ids) {
		m_typeIds.InsertOption("", id);
	}
}


void CGqlObjectCollectionInfo::SetNodeId(const QByteArray& nodeId)
{
	m_nodeId = nodeId;
}


// reimplemented (imtbase::TIStructuredCollectionInfo)

imtbase::IStructuredObjectCollectionInfo::ElementType CGqlObjectCollectionInfo::GetElementType(const QByteArray& elementId) const
{
	Item* itemPtr = FindItemById(elementId);
	if (itemPtr != nullptr) {
		if (itemPtr->typeId.isEmpty()) {
			return ET_NODE;
		}
		else {
			return ET_OBJECT;
		}
	}

	return ET_UNKNOWN;
}


QByteArrayList CGqlObjectCollectionInfo::GetElementBasePath(const QByteArray& elementId) const
{
	return m_basePath;
}


// reimplemented (imtbase::IObjectCollectionInfo)

const iprm::IOptionsList* CGqlObjectCollectionInfo::GetObjectTypesInfo() const
{
	return &m_typeIds;
}


imtbase::ICollectionInfo::Id CGqlObjectCollectionInfo::GetObjectTypeId(const Id& objectId) const
{
	Item* itemPtr = FindItemById(objectId);
	if (itemPtr != nullptr) {
		return itemPtr->typeId;
	}

	return Id();
}


idoc::MetaInfoPtr CGqlObjectCollectionInfo::GetDataMetaInfo(const Id& objectId) const
{
	Item* itemPtr = FindItemById(objectId);
	if (itemPtr != nullptr && !itemPtr->typeId.isEmpty()) {
		return itemPtr->dataMetaInfoPtr;
	}

	return idoc::MetaInfoPtr();
}


// reimplemented (ICollectionInfo)

int CGqlObjectCollectionInfo::GetElementsCount(const iprm::IParamsSet* /*selectionParamPtr*/) const
{
	const_cast<CGqlObjectCollectionInfo*>(this)->UpdateCache();

	return m_items.size();
}


imtbase::ICollectionInfo::Ids CGqlObjectCollectionInfo::GetElementIds(
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	const_cast<CGqlObjectCollectionInfo*>(this)->UpdateCache();

	Ids retVal;

	int elementsCount = m_items.count();
	for (int i = 0; i < elementsCount; ++i){
		retVal.push_back(m_items[i].id);
	}

	return retVal;
}


bool CGqlObjectCollectionInfo::GetSubsetInfo(
			imtbase::ICollectionInfo& /*subsetInfo*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	return false;
}


QVariant CGqlObjectCollectionInfo::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	int objectIndex = GetItemIndex(elementId);
	if (objectIndex >= 0){
		switch (infoType){
		case EIT_DESCRIPTION:
			return m_items[objectIndex].description;

		case EIT_NAME:
			return m_items[objectIndex].name;

		case EIT_ENABLED:
			true;

		default:
			return QVariant();
		}
	}

	return QVariant();
}

idoc::MetaInfoPtr CGqlObjectCollectionInfo::GetElementMetaInfo(const Id& elementId) const
{
	Item* itemPtr = FindItemById(elementId);
	if (itemPtr != nullptr) {
		return itemPtr->elementMetaInfoPtr;
	}

	return idoc::MetaInfoPtr();
}


bool CGqlObjectCollectionInfo::SetElementName(const Id& elementId, const QString& name)
{
	return false;
}


bool CGqlObjectCollectionInfo::SetElementDescription(const Id& elementId, const QString& description)
{
	return false;
}


bool CGqlObjectCollectionInfo::SetElementEnabled(const Id& elementId, bool isEnabled)
{
	return false;
}


// reimplemented (istd::IChangeable)

int CGqlObjectCollectionInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET | SO_COMPARE;
}


bool CGqlObjectCollectionInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	return false;
}


bool CGqlObjectCollectionInfo::IsEqual(const IChangeable & object) const
{
	const CGqlObjectCollectionInfo* sourcePtr = dynamic_cast<const CGqlObjectCollectionInfo*>(&object);
	if (sourcePtr != NULL){
		return (m_items == sourcePtr->m_items);
	}

	return false;
}

istd::IChangeable* CGqlObjectCollectionInfo::CloneMe(CompatibilityMode mode) const
{
	return nullptr;
}


bool CGqlObjectCollectionInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::IChangeable::ChangeSet changeSet(CF_ANY);
	changeSet.SetChangeInfo(CN_ALL_CHANGED, QVariant());
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	m_items.clear();

	return true;
}


// protected methods

CGqlObjectCollectionInfo::Item* CGqlObjectCollectionInfo::FindItemById(const Id& elementId) const
{
	int index = GetItemIndex(elementId);

	if (index >= 0){
		Q_ASSERT(index < m_items.count());

		return &(const_cast<CGqlObjectCollectionInfo*>(this))->m_items[index];
	}

	return nullptr;
}


void CGqlObjectCollectionInfo::UpdateCache()
{
	m_items.clear();

	Q_ASSERT(m_delegatePtr != nullptr);
	if (m_delegatePtr != nullptr){
		iprm::CParamsSet selectionParam;
		iprm::CIdParam nodeIdParam;
		nodeIdParam.SetId(m_nodeId);
		selectionParam.SetEditableParameter("NodeId", &nodeIdParam);

		imtgql::IGqlStructuredCollectionResponse::ElementList list = m_delegatePtr->GetElementList(0, -1, &selectionParam);
		for (const imtgql::IGqlStructuredCollectionResponse::ElementInfo& element : list) {
			Item item;

			item.id = element.id;
			item.name = element.name;
			item.description = element.description;
			item.typeId = element.isNode ? QByteArray() : element.typeId;
			if (!element.elementMetaInfoPtr.isNull()) {
				item.elementMetaInfoPtr.SetCastedOrRemove(element.elementMetaInfoPtr->CloneMe());
			}
			if (!element.dataMetaInfoPtr.isNull()) {
				item.dataMetaInfoPtr.SetCastedOrRemove(element.dataMetaInfoPtr->CloneMe());
			}

			m_items.append(item);
		}
	}
}


} // namespace imtclientgql


