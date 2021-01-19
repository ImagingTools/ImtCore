#include <imtloggui/CLayerViewComp.h>


namespace imtloggui
{


// reimplemented (imtloggui::ILayerProvider)

// reimplemented (imtbase::IObjectCollection)

const istd::IChangeable* CLayerViewComp::GetObjectPtr(const QByteArray& objectId) const
{
	int index = GetIndex(objectId);
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

int CLayerViewComp::GetCount() const
{
	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	return qMin(count, m_representationViewCompPtr.GetCount());
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


