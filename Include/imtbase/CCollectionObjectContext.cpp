#include <imtbase/CCollectionObjectContext.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/TDelPtr.h>


namespace imtbase
{


// public methods

CCollectionObjectContext::CCollectionObjectContext()
	:m_objectCollectionPtr(nullptr)
{
}


void CCollectionObjectContext::SetObjectContext(const imtbase::IObjectCollection* objectCollectionPtr, const QByteArray& objectId)
{
	if ((m_objectCollectionPtr != objectCollectionPtr) || (m_objectId != objectId)){
		istd::CChangeNotifier notifier(this);

		m_objectCollectionPtr = objectCollectionPtr;
		m_objectId = objectId;
	}
}


// reimplemented (ICollectionObjectContext)

const imtbase::IObjectCollection* CCollectionObjectContext::GetObjectCollectionPtr() const
{
	return m_objectCollectionPtr;
}


QByteArray CCollectionObjectContext::GetObjectId() const
{
	return m_objectId;
}


// reimplemented (istd::IChangeable)

int CCollectionObjectContext::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_CLONE | SO_RESET;
}


bool CCollectionObjectContext::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	if (m_objectCollectionPtr == nullptr && m_objectId.isEmpty()){
		const CCollectionObjectContext* sourcePtr = dynamic_cast<const CCollectionObjectContext*>(&object);
		if (sourcePtr != nullptr){
			istd::CChangeNotifier notifier(this);

			m_objectCollectionPtr = sourcePtr->m_objectCollectionPtr;
			m_objectId = sourcePtr->m_objectId;

			return true;
		}
	}

	return false;
}


bool CCollectionObjectContext::IsEqual(const IChangeable& object) const
{
	const CCollectionObjectContext* sourcePtr = dynamic_cast<const CCollectionObjectContext*>(&object);
	if (sourcePtr != nullptr){
		if ((m_objectCollectionPtr == sourcePtr->m_objectCollectionPtr) && (m_objectId == sourcePtr->m_objectId)){
			return true;
		}
	}

	return false;
}


istd::IChangeable* CCollectionObjectContext::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CCollectionObjectContext> clonePtr(new CCollectionObjectContext);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CCollectionObjectContext::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);
	
	m_objectCollectionPtr = nullptr;
	m_objectId.clear();

	return true;
}


} // namespace imtbase


