#include <imtloggui/CGroupComp.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IGroup)

QByteArray CGroupComp::GetLayerIdForTimespan(uint64_t timespan) const
{
	QByteArray retVal;

	if (m_arrangedIds.isEmpty()){
		return retVal;
	}

	if (timespan < m_arrangedIds.firstKey()){
		return m_arrangedIds.first();
	}

	QList<uint64_t> keys = m_arrangedIds.keys();
	uint64_t foundKey = m_arrangedIds.firstKey();
	for (int i = 0; i < keys.count(); i++){
		if (keys[i] < timespan){
			foundKey = keys[i];
		}
	}

	return m_arrangedIds[foundKey];
}


// reimplemented (imtbase::IObjectCollection)

const istd::IChangeable* CGroupComp::GetObjectPtr(const QByteArray& objectId) const
{
	int index = GetIndex(objectId);

	if (index >= 0){
		return m_layerCompPtr[index];
	}

	return nullptr;
}


// reimplemented (imtbase::ICollectionInfo)

imtbase::ICollectionInfo::Ids CGroupComp::GetElementIds() const
{
	int count = GetCount();

	imtbase::ICollectionInfo::Ids retVal;
	for (int i = 0; i < count; i++){
		retVal.append(m_idAttrPtr[i]);
	}

	return retVal;
}


QVariant CGroupComp::GetElementInfo(const QByteArray& elementId, int infoType) const
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


// reimplemented (icomp::CComponentBase)

void CGroupComp::OnComponentCreated()
{
	for (int i = 0; i < GetCount(); i++){
		Q_ASSERT(!m_arrangedIds.contains(m_minTimespanAttrPtr[i]));

		m_arrangedIds[m_minTimespanAttrPtr[i]] = m_idAttrPtr[i];
	}

	if (!m_arrangedIds.isEmpty()){
		QByteArray id = m_arrangedIds.first();
		m_arrangedIds.remove(m_arrangedIds.firstKey());
		m_arrangedIds[0] = id;
	}
}


// private methods

int CGroupComp::GetCount() const
{
	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	return qMin(count, m_layerCompPtr.GetCount());
}


int CGroupComp::GetIndex(const QByteArray& id) const
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


