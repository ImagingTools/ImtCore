#include <imtbase/CSimpleReferenceCollection.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


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

bool CSimpleReferenceCollection::InsertReference(const Id& objectId)
{
	istd::CChangeNotifier changeNotifier(this);

	Reference reference;
	reference.id = objectId;

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

bool CSimpleReferenceCollection::GetCollectionItemMetaInfo(const QByteArray& /*objectId*/, idoc::IDocumentMetaInfo& /*metaInfo*/) const
{
	return false;
}


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


// reimplemented (imtbase::ICollectionInfo)

imtbase::ICollectionInfo::Ids CSimpleReferenceCollection::GetElementIds() const
{
	Ids retVal;

	for (const Reference& ref : m_references){
		retVal.push_back(ref.id);
	}

	return retVal;
}


QVariant CSimpleReferenceCollection::GetElementInfo(const QByteArray& elementId, int infoType) const
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


// reimplemented (iser::ISerializable)

bool CSimpleReferenceCollection::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag referencesTag("References", "List of references", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag referenceTag("Reference", "Single reference item", iser::CArchiveTag::TT_GROUP, &referencesTag);

	int referencesCount = m_references.count();
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


istd::IChangeable* CSimpleReferenceCollection::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CSimpleReferenceCollection> clonePtr( new CSimpleReferenceCollection);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return NULL;
}


bool CSimpleReferenceCollection::ResetData(CompatibilityMode /*mode*/)
{
	return RemoveAllReferences();
}


// protected methods

int CSimpleReferenceCollection::FindReference(const QByteArray&  id) const
{
	for (int i = 0; i < m_references.count(); ++i){
		if (m_references[i].id == id){
			return i;
		}
	}

	return -1;
}


} // namespace imtbase


