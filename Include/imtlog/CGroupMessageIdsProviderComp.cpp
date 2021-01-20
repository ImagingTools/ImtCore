#include <imtlog/CGroupMessageIdsProviderComp.h>


namespace imtlog
{


// reimplemented (imtlog::IGroupMessageIdsProvider)

bool CGroupMessageIdsProviderComp::GetGroupMessageIds(const QByteArray& groupId, QSet<int>& messageIds) const
{
	if (m_groupIdAttrPtr.IsValid() && *m_groupIdAttrPtr == groupId){
		messageIds.clear();

		if (m_idsAttrPtr.IsValid()){
			for (int i = 0; i < m_idsAttrPtr.GetCount(); i++){
				messageIds.insert(m_idsAttrPtr[i]);
			}
		}

		return true;
	}

	if (m_slaveAttrPtr.IsValid()){
		return m_slaveAttrPtr->GetGroupMessageIds(groupId, messageIds);
	}

	return false;
}


} // namespace imtlog


