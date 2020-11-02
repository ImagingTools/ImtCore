#include <imtloggui/CLayerProviderComp.h>


namespace imtloggui
{


// reimplemented (imtloggui::ILayerProvider)

IRepresentationFactoryProvider* CLayerProviderComp::GetRepresentationFactoryProvider(const QByteArray& id) const
{
	int index = GetIndex(id);
	if (index >= 0){
		return m_factoryProviderCompPtr[index];
	}

	return nullptr;
}


QList<int> CLayerProviderComp::GetMessageIdList() const
{
	QList<int> retVal;

	int count = m_messageIdListAttrPtr.GetCount();
	for (int i = 0; i < count; i++){
		retVal.append(m_messageIdListAttrPtr[i]);
	}

	return retVal;
}


// reimplemented (imtbase::ICollectionInfo)

imtbase::ICollectionInfo::Ids CLayerProviderComp::GetElementIds() const
{
	int count = GetCount();

	imtbase::ICollectionInfo::Ids retVal;
	for (int i = 0; i < count; i++){
		retVal.append(m_idAttrPtr[i]);
	}

	return retVal;
}


QVariant CLayerProviderComp::GetElementInfo(const QByteArray& elementId, int infoType) const
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

int CLayerProviderComp::GetCount() const
{
	return qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
}


int CLayerProviderComp::GetIndex(const QByteArray& id) const
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


