#include <imtgql/CGqlObjectCollectionDelegateCompBase.h>


namespace imtgql
{


// reimplemented (IGqlObjectCollectionDelegate)

QByteArrayList CGqlObjectCollectionDelegateCompBase::GetSupportedObjectTypeIds() const
{
	QByteArrayList ids;
	if (m_objectTypeIdsAttrPtr.IsValid()) {
		for (int i = 0; m_objectTypeIdsAttrPtr.GetCount(); i++){
			QByteArray id = m_objectTypeIdsAttrPtr[i];
			if (!id.isEmpty()) {
				if (!ids.contains(id)) {
					ids.append(id);
				}
			}
		}
	}

	return ids;
}


imtcom::IFileTransfer* CGqlObjectCollectionDelegateCompBase::GetFileTransfer() const
{
	if (m_fileTransferCompPtr.IsValid()){
		return m_fileTransferCompPtr.GetPtr();
	}

	return nullptr;
}


} // namespace imtgql


