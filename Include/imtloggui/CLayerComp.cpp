#include <imtloggui/CLayerComp.h>


namespace imtloggui
{


// reimplemented (imtloggui::ILayerProvider)

IRepresentation* CLayerComp::GetRepresentation(const QByteArray& id) const
{
	int index = GetIndex(id);
	if (index >= 0){
		return m_representationCompPtr[index];
	}

	return nullptr;
}


// reimplemented (imtbase::ICollectionInfo)

imtbase::ICollectionInfo::Ids CLayerComp::GetElementIds() const
{
	int count = GetCount();

	imtbase::ICollectionInfo::Ids retVal;
	for (int i = 0; i < count; i++){
		retVal.append(m_idAttrPtr[i]);
	}

	return retVal;
}


QVariant CLayerComp::GetElementInfo(const QByteArray& elementId, int infoType) const
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

int CLayerComp::GetCount() const
{
	return qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
}


int CLayerComp::GetIndex(const QByteArray& id) const
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


