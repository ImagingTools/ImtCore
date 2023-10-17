#include <imtbase/CCachedObjectCollectionComp.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/CMemoryWriteArchive.h>

#include <QtCore/QDebug>


namespace imtbase
{


// public methods

CCachedObjectCollectionComp::CCachedObjectCollectionComp()
			:m_operationFlags(IObjectCollection::OF_ALL & ~OF_SUPPORT_PAGINATION),
			m_selectionParamsCachePtr(nullptr)
{
}


void CCachedObjectCollectionComp::SetOperationFlags(int flags, const QByteArray& objectId)
{
	return;
}


// reimplemented (imtbase::IObjectCollection)

const IRevisionController* CCachedObjectCollectionComp::GetRevisionController() const
{
	return nullptr;
}


const ICollectionDataController* CCachedObjectCollectionComp::GetDataController() const
{
	return nullptr;
}


int CCachedObjectCollectionComp::GetOperationFlags(const QByteArray& objectId) const
{
	return m_operationFlags;
}


ICollectionInfo::Id CCachedObjectCollectionComp::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			DataPtr defaultValuePtr,
			const Id& proposedElementId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* elementMetaInfoPtr,
			const IOperationContext* /*operationContextPtr*/)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return ICollectionInfo::Id();
	}

	m_paginationDataList.Reset();

	ICollectionInfo::Id id =  m_objectCollectionCompPtr->InsertNewObject(
			typeId,
			name,
			description,
			defaultValuePtr,
			proposedElementId,
			dataMetaInfoPtr,
			elementMetaInfoPtr);
	if (!id.isEmpty()){
		istd::IChangeable::ChangeSet changeSet(CF_ADDED);
		changeSet.SetChangeInfo(CN_ELEMENT_INSERTED, id);
		istd::CChangeNotifier changeNotifier(this, &changeSet);
	}

	return id;
}


bool CCachedObjectCollectionComp::RemoveElement(const Id& elementId, const IOperationContext* operationContextPtr)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	bool retVal = m_objectCollectionCompPtr->RemoveElement(elementId, operationContextPtr);

	if (retVal){
		istd::IChangeable::ChangeSet changeSet(CF_REMOVED);
		changeSet.SetChangeInfo(CN_ELEMENT_REMOVED, elementId);
		istd::CChangeNotifier changeNotifier(this, &changeSet);
		m_paginationDataList.Reset();
	}

	return  retVal;
}


const istd::IChangeable* CCachedObjectCollectionComp::GetObjectPtr(const Id& objectId) const
{
	return nullptr;
}



bool CCachedObjectCollectionComp::GetObjectData(const Id& objectId, DataPtr& dataPtr) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	return  m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr);
}


bool CCachedObjectCollectionComp::SetObjectData(
			const Id& objectId,
			const istd::IChangeable& object,
			CompatibilityMode mode,
			const IOperationContext* operationContextPtr)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	m_paginationDataList.Reset();

	bool retVal = m_objectCollectionCompPtr->SetObjectData(objectId, object, mode, operationContextPtr);
	if (retVal){
		istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
		changeSet.SetChangeInfo(CN_ELEMENT_UPDATED, objectId);
		istd::CChangeNotifier changeNotifier(this, &changeSet);
	}

	return retVal;
}


IObjectCollection* CCachedObjectCollectionComp::CreateSubCollection(int offset, int count, const iprm::IParamsSet *selectionParamsPtr) const
{
	PaginationData* paginationDataPtr = CheckCache(offset, count, selectionParamsPtr);

	if (paginationDataPtr != nullptr){
		return paginationDataPtr->cachePtr->CreateSubCollection(offset, count, selectionParamsPtr);
	}

	return nullptr;
}


imtbase::IObjectCollectionIterator* CCachedObjectCollectionComp::CreateObjectCollectionIterator(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	return nullptr;
}


// reimplemented (IObjectCollectionInfo)
const iprm::IOptionsList* CCachedObjectCollectionComp::GetObjectTypesInfo() const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	return m_objectCollectionCompPtr->GetObjectTypesInfo();
}


QByteArray CCachedObjectCollectionComp::GetObjectTypeId(const Id& objectId) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return QByteArray();
	}

	return m_objectCollectionCompPtr->GetObjectTypeId(objectId);
}


idoc::MetaInfoPtr CCachedObjectCollectionComp::GetDataMetaInfo(const Id& objectId) const
{
	for (int index = 0; index < m_paginationDataList.GetCount(); index++){
		PaginationData* paginationDataPtr = m_paginationDataList.GetAt(index);
		if (paginationDataPtr->cachePtr->GetElementIds().contains(objectId)){
			return paginationDataPtr->cachePtr->GetDataMetaInfo(objectId);
		}
	}

	return idoc::MetaInfoPtr();
}


int CCachedObjectCollectionComp::GetElementsCount(const iprm::IParamsSet* selectionParamsPtr) const
{
	return m_objectCollectionCompPtr->GetElementsCount(selectionParamsPtr);
}


ICollectionInfo::Ids CCachedObjectCollectionComp::GetElementIds(
	int offset,
	int count,
	const iprm::IParamsSet* selectionParamsPtr) const
{
	PaginationData* paginationDataPtr = CheckCache(offset, count, selectionParamsPtr);
	if (paginationDataPtr != nullptr){
		return paginationDataPtr->cachePtr->GetElementIds();
	}

	return ICollectionInfo::Ids();
}


bool CCachedObjectCollectionComp::GetSubsetInfo(
			ICollectionInfo& subsetInfo,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	return false;
}


QVariant CCachedObjectCollectionComp::GetElementInfo(const Id& elementId, int infoType) const
{
	for (int index = 0; index < m_paginationDataList.GetCount(); index++){
		PaginationData* paginationDataPtr = m_paginationDataList.GetAt(index);
		if (paginationDataPtr->cachePtr->GetElementIds().contains(elementId)){
			return paginationDataPtr->cachePtr->GetElementInfo(elementId, infoType);
		}
	}

	return QVariant();
}


idoc::MetaInfoPtr CCachedObjectCollectionComp::GetElementMetaInfo(const Id& elementId) const
{
	for (int index = 0; index < m_paginationDataList.GetCount(); index++){
		PaginationData* paginationDataPtr = m_paginationDataList.GetAt(index);
		if (paginationDataPtr->cachePtr->GetElementIds().contains(elementId)){
			return paginationDataPtr->cachePtr->GetElementMetaInfo(elementId);
		}
	}

	return idoc::MetaInfoPtr();
}


bool CCachedObjectCollectionComp::SetElementName(const Id& elementId, const QString& name)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	m_paginationDataList.Reset();

	bool retVal =  m_objectCollectionCompPtr->SetElementName(elementId, name);
	if (retVal){
		istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
		changeSet.SetChangeInfo(CN_ELEMENT_UPDATED, elementId);
		istd::CChangeNotifier changeNotifier(this, &changeSet);
	}

	return retVal;
}


bool CCachedObjectCollectionComp::SetElementDescription(const Id& elementId, const QString& description)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	m_paginationDataList.Reset();

	bool retVal = m_objectCollectionCompPtr->SetElementDescription(elementId, description);
	if (retVal){
		istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
		changeSet.SetChangeInfo(CN_ELEMENT_UPDATED, elementId);
		istd::CChangeNotifier changeNotifier(this, &changeSet);
	}

	return retVal;
}


bool CCachedObjectCollectionComp::SetElementEnabled(const Id& elementId, bool isEnabled)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	m_paginationDataList.Reset();

	bool retVal = m_objectCollectionCompPtr->SetElementEnabled(elementId, isEnabled);
	if (retVal){
		istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
		changeSet.SetChangeInfo(CN_ELEMENT_UPDATED, elementId);
		istd::CChangeNotifier changeNotifier(this, &changeSet);
	}

	return retVal;
}


CCachedObjectCollectionComp::PaginationData* CCachedObjectCollectionComp::CheckCache(
		int offset,
		int count,
		const iprm::IParamsSet* selectionParamsPtr) const
{
	iser::CMemoryWriteArchive archive;
	iprm::IParamsSet* paramsSet = const_cast<iprm::IParamsSet*>(selectionParamsPtr);
	if (paramsSet != nullptr && !paramsSet->Serialize(archive)){
		QByteArray errorMessage = QObject::tr("Error when serializing an object").toUtf8();
		Q_ASSERT(0);

		return nullptr;
	}

	QByteArray data((char*)archive.GetBuffer(), archive.GetBufferSize());

	for (int index = 0; index < m_paginationDataList.GetCount(); index++){
		PaginationData* paginationDataPtr = m_paginationDataList.GetAt(index);
		if (
			paginationDataPtr != nullptr &&
			paginationDataPtr->offset == offset &&
			paginationDataPtr->count == count &&
			paginationDataPtr->selectionParamsData == data){

			return paginationDataPtr;
		}
	}

	IObjectCollection* subcollection = m_objectCollectionCompPtr->CreateSubCollection(offset, count, selectionParamsPtr);

	if (subcollection == nullptr){
		Q_ASSERT(0);

		return nullptr;
	}

	if (m_paginationDataList.GetCount() > 0 && m_paginationDataList.GetCount() > *m_cacheLimitPtr){
		m_paginationDataList.RemoveAt(0);
	}

	m_paginationDataList.PushBack(new PaginationData(offset, count, data, subcollection));

	return m_paginationDataList.GetAt(m_paginationDataList.GetCount() - 1);
}


} // namespace imtbase


