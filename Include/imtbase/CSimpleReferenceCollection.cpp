#include <imtbase/CSimpleReferenceCollection.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtbase
{


// public methods

// reimplemented (IReferenceCollection)

bool CSimpleReferenceCollection::InsertReference(const Id& resourceId)
{
	istd::CChangeNotifier changeNotifier(this);

	Reference reference;
	reference.id = resourceId;

	m_references.push_back(reference);

	return true;
}


bool CSimpleReferenceCollection::RemoveReference(const Id& resourceId)
{
	int index = FindReference(resourceId);
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


// reimplemented (imtbase::ICollectionInfo)

imtbase::ICollectionInfo::Ids CSimpleReferenceCollection::GetElementIds() const
{
	Ids retVal;

	for (const Reference& ref : m_references){
		retVal.push_back(ref.id);
	}

	return retVal;
}


QString CSimpleReferenceCollection::GetElementName(const QByteArray& elementId) const
{
	int index = FindReference(elementId);
	if (index >= 0){
		return m_references[index].name;
	}

	return QString();
}


QString CSimpleReferenceCollection::GetElementDescription(const QByteArray& elementId) const
{
	int index = FindReference(elementId);
	if (index >= 0){
		return m_references[index].description;
	}

	return QString();
}


bool CSimpleReferenceCollection::IsElementEnabled(const QByteArray& /*elementId*/) const
{
	return true;
}


// reimplemented (iser::ISerializable)

bool CSimpleReferenceCollection::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changePtr(archive.IsStoring() ? NULL : this);

	bool retVal = true;

	static iser::CArchiveTag referencesTag("References", "List of references", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag referenceTag("Reference", "Single reference item", iser::CArchiveTag::TT_GROUP, &referencesTag);

	int referencesCount = 0;
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
	istd::CChangeNotifier changeNotifier(this);

	m_references.clear();

	return true;
}


// protected methods

int CSimpleReferenceCollection::FindReference(const QByteArray & id) const
{
	for (int i = 0; i < m_references.count(); ++i){
		if (m_references[i].id == id){
			return i;
		}
	}

	return -1;
}


} // namespace imtbase


