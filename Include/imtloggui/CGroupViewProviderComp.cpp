#include <imtloggui/CGroupViewProviderComp.h>


namespace imtloggui
{


// reimplemented (imtloggui::IGroupProvider)

ILayerViewProvider* CGroupViewProviderComp::GetLayerViewProvider(const QByteArray& id) const
{
	int index = GetIndex(id);
	if (index >= 0){
		return m_providersCompPtr[index];
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
	return QVariant();
}


// private methods

int CGroupViewProviderComp::GetCount() const
{
	int count = qMin(m_idAttrPtr.GetCount(), m_colourAttrPtr.GetCount());
	count = qMin(count, m_providersCompPtr.GetCount());

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


