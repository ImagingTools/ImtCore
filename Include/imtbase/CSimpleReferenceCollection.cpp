// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CSimpleReferenceCollection.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>

// ImtCore includes
#include <imtbase/CObjectLink.h>

namespace imtbase
{


// public methods

CSimpleReferenceCollection::CSimpleReferenceCollection()
	:m_sourceCollectionInfoPtr(nullptr)
{
}


void CSimpleReferenceCollection::RegisterSourceCollection(const imtbase::IObjectCollection* sourceCollectionInfoPtr)
{
	m_sourceCollectionInfoPtr = sourceCollectionInfoPtr;
}


const imtbase::IObjectCollection* CSimpleReferenceCollection::GetSourceCollection() const
{
	return m_sourceCollectionInfoPtr;
}


// reimplemented (IReferenceCollection)

IReferenceCollection::ICollectionObjectLinkPtr CSimpleReferenceCollection::GetObjectLink(const Id& objectId, const QByteArray& repositoryId) const
{
	if (m_sourceCollectionInfoPtr == nullptr){
		return nullptr;
	}

	int referencesCount = m_references.size();
	for (int i = 0; i < referencesCount; ++i){
		const Reference& ref = m_references[i];
		if (ref.id == objectId && (repositoryId.isEmpty() || (ref.repositoryId == repositoryId))){
			istd::TDelPtr<imtbase::CObjectLink> linkImplPtr = new imtbase::CObjectLink;
			linkImplPtr->SetCollectionPtr(m_sourceCollectionInfoPtr);
			linkImplPtr->SetObjectUuid(objectId);

			ICollectionObjectLinkPtr retVal;
			retVal.reset(linkImplPtr.PopPtr());

			return retVal;
		}
	}

	return ICollectionObjectLinkPtr();
}


bool CSimpleReferenceCollection::InsertReference(const Id& objectId, const QByteArray& repositoryId)
{
	istd::CChangeNotifier changeNotifier(this);

	Reference reference;
	reference.id = objectId;
	reference.repositoryId = repositoryId;

	if (m_sourceCollectionInfoPtr != nullptr){
		reference.description = m_sourceCollectionInfoPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
		reference.name = m_sourceCollectionInfoPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
	}

	m_references.push_back(reference);

	return true;
}


bool CSimpleReferenceCollection::RemoveReference(const Id& objectId)
{
	int index = FindReference(objectId);
	if (index >= 0){
		istd::CChangeNotifier changeNotifier(this);

		m_references.removeAt(index);
	}

	return true;
}


bool CSimpleReferenceCollection::RemoveAllReferences()
{
	istd::CChangeNotifier changeNotifier(this);

	m_references.clear();

	return true;
}


// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CSimpleReferenceCollection::GetObjectTypesInfo() const
{
	return nullptr;
}


imtbase::ICollectionInfo::Id CSimpleReferenceCollection::GetObjectTypeId(const QByteArray& objectId) const
{
	int index = FindReference(objectId);
	if (index >= 0){
		return m_references[index].typeId;
	}

	return Id();
}

idoc::MetaInfoPtr CSimpleReferenceCollection::GetDataMetaInfo(const Id& /*objectId*/) const
{
	return idoc::MetaInfoPtr();
}


// reimplemented (imtbase::ICollectionInfo)

int CSimpleReferenceCollection::GetElementsCount(const iprm::IParamsSet* /*selectionParamPtr*/, ilog::IMessageConsumer* /*logPtr*/) const
{
	return m_references.count();
}


imtbase::ICollectionInfo::Ids CSimpleReferenceCollection::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	Ids retVal;

	Q_ASSERT(offset >= 0);

	int elementsCount = count >= 0 ? qMin(count, m_references.size()) : m_references.size();

	for (int i = offset; i < elementsCount; ++i){
		retVal.push_back(m_references[i].id);
	}

	return retVal;
}


bool CSimpleReferenceCollection::GetSubsetInfo(
			imtbase::ICollectionInfo& /*subsetInfo*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return false;
}


QVariant CSimpleReferenceCollection::GetElementInfo(const QByteArray& elementId, int infoType, ilog::IMessageConsumer* /*logPtr*/) const
{
	int index = FindReference(elementId);
	if (index >= 0){
		switch (infoType){
			case EIT_DESCRIPTION:
				return m_references[index].description;

			case EIT_NAME:
				return m_references[index].name;
		}
	}

	return QVariant();
}


idoc::MetaInfoPtr CSimpleReferenceCollection::GetElementMetaInfo(const Id& /*elementId*/, ilog::IMessageConsumer* /*logPtr*/) const
{
	return idoc::MetaInfoPtr();
}


bool CSimpleReferenceCollection::SetElementName(const Id& /*elementId*/, const QString& /*name*/, ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


bool CSimpleReferenceCollection::SetElementDescription(const Id& /*elementId*/, const QString& /*description*/, ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


bool CSimpleReferenceCollection::SetElementEnabled(const Id& /*elementId*/, bool /*isEnabled*/, ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


// reimplemented (iser::ISerializable)

bool CSimpleReferenceCollection::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag referencesTag("References", "List of references", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag referenceTag("Reference", "Single reference item", iser::CArchiveTag::TT_GROUP, &referencesTag);

	int referencesCount = m_references.size();
	retVal = retVal && archive.BeginMultiTag(referencesTag, referenceTag, referencesCount);

	if (!archive.IsStoring()){
		m_references.resize(referencesCount);
	}

	for (int i = 0; i < referencesCount; ++i){
		retVal = retVal && archive.BeginTag(referenceTag);

		static iser::CArchiveTag typeTag("Type", "Type-ID of the referenced object");
		retVal = retVal && archive.BeginTag(typeTag);
		retVal = retVal && archive.Process(m_references[i].typeId);
		retVal = retVal && archive.EndTag(typeTag);

		static iser::CArchiveTag idTag("ID", "ID of the referenced object");
		retVal = retVal && archive.BeginTag(idTag);
		retVal = retVal && archive.Process(m_references[i].id);
		retVal = retVal && archive.EndTag(idTag);

		retVal = retVal && archive.EndTag(referenceTag);
	}

	retVal = retVal && archive.EndTag(referencesTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CSimpleReferenceCollection::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const CSimpleReferenceCollection* sourcePtr = dynamic_cast<const CSimpleReferenceCollection*>(&object);
	if (sourcePtr != NULL){
		istd::CChangeNotifier changeNotifier(this);

		m_references = sourcePtr->m_references;

		return true;
	}

	return false;
}


bool CSimpleReferenceCollection::IsEqual(const istd::IChangeable& object) const
{
	const CSimpleReferenceCollection* sourcePtr = dynamic_cast<const CSimpleReferenceCollection*>(&object);
	if (sourcePtr != NULL){
		return (m_references == sourcePtr->m_references);
	}

	return false;
}


istd::IChangeableUniquePtr CSimpleReferenceCollection::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr( new CSimpleReferenceCollection);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return NULL;
}


bool CSimpleReferenceCollection::ResetData(CompatibilityMode /*mode*/)
{
	return RemoveAllReferences();
}


// protected methods

int CSimpleReferenceCollection::FindReference(const QByteArray& id) const
{
	for (int i = 0; i < m_references.count(); ++i){
		if (m_references[i].id == id){
			return i;
		}
	}

	return -1;
}


} // namespace imtbase


