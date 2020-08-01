#include <imtbase/CObjectCollectionEventBase.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtbase
{


// public methods

CObjectCollectionEventBase::CObjectCollectionEventBase()
{
}

CObjectCollectionEventBase::CObjectCollectionEventBase(const QByteArray& itemId)
	:m_itemId(itemId)
{
}


// reimplemented (imtbase::IObjectCollectionEvent)

QByteArray CObjectCollectionEventBase::GetItemId() const
{
	return m_itemId;
}


// reimplemented (istd::IChangeable)

int CObjectCollectionEventBase::GetSupportedOperations() const
{
	return (SO_COPY | SO_COMPARE | SO_CLONE);
}


bool CObjectCollectionEventBase::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	const CObjectCollectionEventBase* sourcePtr = dynamic_cast<const CObjectCollectionEventBase*>(&object);
	if (sourcePtr != nullptr){
		if (m_itemId != sourcePtr->m_itemId){
			istd::CChangeNotifier notifier(this);

			m_itemId = sourcePtr->m_itemId;
		}

		return true;
	}

	return false;
}


bool CObjectCollectionEventBase::IsEqual(const IChangeable& object) const
{
	const CObjectCollectionEventBase* sourcePtr = dynamic_cast<const CObjectCollectionEventBase*>(&object);
	if (sourcePtr != NULL){
		if (m_itemId != sourcePtr->m_itemId){
			return false;
		}

		return true;
	}

	return false;
}


istd::IChangeable* CObjectCollectionEventBase::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CObjectCollectionEventBase> clonePtr(new CObjectCollectionEventBase);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtbase


