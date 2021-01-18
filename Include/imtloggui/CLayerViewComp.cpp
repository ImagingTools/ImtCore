#include <imtloggui/CLayerViewComp.h>


namespace imtloggui
{


// reimplemented (imtloggui::ILayerProvider)

IRepresentationView* CLayerViewComp::GetRepresentationView(const QByteArray& id) const
{
	int index = GetIndex(id);
	if (index >= 0){
		return m_representationViewCompPtr[index];
	}

	return nullptr;
}


// reimplemented (imtbase::ICollectionInfo)

imtbase::ICollectionInfo::Ids CLayerViewComp::GetElementIds() const
{
	int count = GetCount();

	imtbase::ICollectionInfo::Ids retVal;
	for (int i = 0; i < count; i++){
		retVal.append(m_idAttrPtr[i]);
	}

	return retVal;
}


QVariant CLayerViewComp::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	return QVariant();
}


// private methods

int CLayerViewComp::GetCount() const
{
	return qMin(m_idAttrPtr.GetCount(), m_representationViewCompPtr.GetCount());
}


int CLayerViewComp::GetIndex(const QByteArray& id) const
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


