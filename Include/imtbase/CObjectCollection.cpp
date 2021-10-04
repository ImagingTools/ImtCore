#include <imtbase/CObjectCollection.h>


// ACF includes
#include <istd/TDelPtr.h>


namespace imtbase
{


// public methods

void CObjectCollection::SetOperationFlags(int flags, const QByteArray& objectId)
{
	if (objectId.isEmpty()){
		m_operationFlags = flags;
		return;
	}

	ObjectInfo* objectInfo = GetObjectInfo(objectId);
	if (objectInfo != nullptr){
		objectInfo->flags = flags;
	} else {
		m_operationFlags = flags;
	}
}


// reimplemented (imtbase::IObjectCollection)

int CObjectCollection::GetOperationFlags(const QByteArray& objectId) const
{
	ObjectInfo* objectInfo = GetObjectInfo(objectId);
	if (objectInfo != nullptr){
		return objectInfo->flags;
	}

	return m_operationFlags;
}


// reimplemented (istd::IChangeable)

int CObjectCollection::GetSupportedOperations() const
{
	return SO_CLONE | BaseClass::GetSupportedOperations();
}


istd::IChangeable* CObjectCollection::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CObjectCollection> clonePtr(new CObjectCollection());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


// protected methods

istd::IChangeable* CObjectCollection::CreateObjectInstance(const QByteArray& typeId) const
{
	return BaseClass2::CreateInstance(typeId);
}


} // namespace imtbase


