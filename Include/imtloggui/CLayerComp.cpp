#include <imtloggui/CLayerComp.h>


namespace imtloggui
{


// reimplemented (imtloggui::IGroup)

QByteArray CLayerComp::GetId() const
{
	return *m_idAttrPtr;
}


QString CLayerComp::GetName() const
{
	return *m_nameAttrPtr;
}


QString CLayerComp::GetDescription() const
{
	return *m_descriptionAttrPtr;
}


quint64 CLayerComp::GetMinimumVisualTimeSpan() const
{
	return *m_minTimeSpanAttrPtr;
}


quint64 CLayerComp::GetMaximumVisualTimeSpan() const
{
	return *m_maxTimeSpanAttrPtr;
}


// reimplemented (imtloggui::IRepresentationProvider)

IRepresentation* CLayerComp::GetRepresentation(const QByteArray& id) const
{
	if (m_representations.contains(id)){
		return m_representations[id];
	}

	return nullptr;
}


// reimplemented (imtbase::ICollectionInfo)

imtbase::ICollectionInfo::Ids CLayerComp::GetElementIds() const
{
	return m_representations.keys().toVector();
}


QVariant CLayerComp::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	QVariant retVal;

	if (m_representations.contains(elementId)){
		switch (infoType){
		case EIT_NAME:
			retVal = m_representations[elementId]->GetName();
			break;
		}
	}

	return retVal;
}


// reimplemented (icomp::CComponentBase)

void CLayerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	for (int i = 0; i < m_representationsCompPtr.GetCount(); i++){
		m_representations[m_representationsCompPtr[i]->GetId()] = m_representationsCompPtr[i];
	}
}


} // namespace imtloggui


