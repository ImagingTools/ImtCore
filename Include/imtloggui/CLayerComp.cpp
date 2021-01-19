#include <imtloggui/CLayerComp.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::ILayer)

uint64_t CLayerComp::GetMinimumTimespan() const
{
	if (*m_minTimespanAttrPtr > 0){
		return *m_minTimespanAttrPtr;
	}
	else{
		return 1000;
	}
}

// reimplemented (imtbase::IObjectCollection)

const istd::IChangeable* CLayerComp::GetObjectPtr(const QByteArray& objectId) const
{
	int index = GetIndex(objectId);
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
	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	return qMin(count, m_representationCompPtr.GetCount());
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


