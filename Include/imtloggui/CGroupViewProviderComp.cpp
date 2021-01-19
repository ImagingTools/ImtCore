#include <imtloggui/CGroupViewProviderComp.h>


namespace imtloggui
{


// reimplemented (imtloggui::IGroupProvider)

const istd::IChangeable* CGroupViewProviderComp::GetObjectPtr(const QByteArray& objectId) const
{
	int index = GetIndex(objectId);
	if (index >= 0){
		return m_groupViewCompPtr[index];
	}

	return nullptr;
}


// reimplemented (imtbase::ICollectionInfo)

imtbase::ICollectionInfo::Ids CGroupViewProviderComp::GetElementIds() const
{
	int count = GetCount();

	imtbase::ICollectionInfo::Ids retVal;
	for (int i = 0; i < count; i++){
		retVal.append(m_idAttrPtr[i]);
	}		

	return retVal;
}


QVariant CGroupViewProviderComp::GetElementInfo(const QByteArray& elementId, int infoType) const
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

int CGroupViewProviderComp::GetCount() const
{
	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	count = qMin(count, m_groupViewCompPtr.GetCount());

	return count;
}


int CGroupViewProviderComp::GetIndex(const QByteArray& id) const
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


