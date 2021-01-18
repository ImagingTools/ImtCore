#include <imtloggui/CGroupViewComp.h>


namespace imtloggui
{


// reimplemented (imtloggui::IViewLayerProvider)

IRepresentationViewProvider* CGroupViewComp::GetRepresentationViewProvider(const QByteArray& id) const
{
	int index = GetIndex(id);
	if (index >= 0){
		return m_providerCompPtr[index];
	}

	return nullptr;
}


// reimplemented (imtbase::ICollectionInfo)

imtbase::ICollectionInfo::Ids CGroupViewComp::GetElementIds() const
{
	int count = qMin(m_idAttrPtr.GetCount(), m_providerCompPtr.GetCount());

	imtbase::ICollectionInfo::Ids retVal;
	for (int i = 0; i < count; i++){
		retVal.append(m_idAttrPtr[i]);
	}

	return retVal;
}


QVariant CGroupViewComp::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	return QVariant();
}


// private methods

int CGroupViewComp::GetCount() const
{
	return qMin(m_idAttrPtr.GetCount(), m_providerCompPtr.GetCount());
}


int CGroupViewComp::GetIndex(const QByteArray& id) const
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


