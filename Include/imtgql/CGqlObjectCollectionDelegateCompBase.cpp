#include <imtgql/CGqlObjectCollectionDelegateCompBase.h>


namespace imtgql
{


// reimplemented (IGqlObjectCollectionDelegate)

QByteArray CGqlObjectCollectionDelegateCompBase::GetSupportedObjectTypeId() const
{
	return *m_objectTypeIdAttrPtr;
}


imtcom::IFileTransfer* CGqlObjectCollectionDelegateCompBase::GetFileTransfer() const
{
	if (m_fileTransferCompPtr.IsValid()){
		return m_fileTransferCompPtr.GetPtr();
	}

	return nullptr;
}


} // namespace imtgql


