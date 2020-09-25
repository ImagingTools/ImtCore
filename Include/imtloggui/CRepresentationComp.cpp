#include <imtloggui/CRepresentationComp.h>


namespace imtloggui
{


// reimplemented (imtloggui::IGroup)

QByteArray CRepresentationComp::GetId() const
{
	return *m_idAttrPtr;
}


QString CRepresentationComp::GetName() const
{
	return *m_nameAttrPtr;
}


QString CRepresentationComp::GetDescription() const
{
	return *m_descriptionAttrPtr;
}


IRepresentation::RepresentationObjectPtr CRepresentationComp::GetRepresentation(
			const ilog::IMessageContainer::Messages& messages,
			const IRepresentation::TimeRange& timeRange) const
{
	return IRepresentation::RepresentationObjectPtr();
}


} // namespace imtloggui


