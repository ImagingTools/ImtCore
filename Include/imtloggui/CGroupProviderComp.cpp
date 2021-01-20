#include <imtloggui/CGroupProviderComp.h>


namespace imtloggui
{


// public methods

// reimplemented (imtlog::IGroupMessageIdsProvider)

bool CGroupProviderComp::GetGroupMessageIds(const QByteArray& groupId, QSet<int>& messageIds) const
{
	if (m_groupMessageIdsProviderCompPtr.IsValid()){
		return m_groupMessageIdsProviderCompPtr->GetGroupMessageIds(groupId, messageIds);
	}

	return false;
}


// reimplemented (imtbase::IObjectCollection)

const istd::IChangeable* CGroupProviderComp::GetObjectPtr(const QByteArray& objectId) const
{
	int index = GetIndex(objectId);
	if (index >= 0){
		return m_groupCompPtr[index];
	}

	return nullptr;
}


// reimplemented (imtbase::ICollectionInfo)

imtbase::ICollectionInfo::Ids CGroupProviderComp::GetElementIds() const
{
	int count = GetCount();

	imtbase::ICollectionInfo::Ids retVal;
	for (int i = 0; i < count; i++){
		retVal.append(m_idAttrPtr[i]);
	}		

	return retVal;
}


QVariant CGroupProviderComp::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	int index = GetIndex(elementId);

	QVariant retVal;

	if (index >= 0){
		switch (infoType){
		case EIT_NAME:
			retVal = m_nameAttrPtr[index];
			break;
		}
	}

	return retVal;
}


// private methods

int CGroupProviderComp::GetCount() const
{
	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	count = qMin(count, m_groupCompPtr.GetCount());

	return count;
}


int CGroupProviderComp::GetIndex(const QByteArray& id) const
{
	int count = GetCount();

	for (int i = 0; i < count; i++){
		if (m_idAttrPtr[i] == id){
			return i;
		}
	}

	return -1;
}


} // namespace imtloggui


