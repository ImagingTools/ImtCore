#include <imtloggui/CRepresentationIntervalStatisticsFactoryComp.h>


// ACF includes
#include <ilog/CMessageContainer.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IRepresentationFactory)

IRepresentationFactory::RepresentationObjectPtr CRepresentationIntervalStatisticsFactoryComp::CreateRepresentationObject(
			const imtlog::CTimeRange& timeRange,
			const QList<int>& messageIdList,
			imtlog::IEventMessageIdFilter::Mode mode) const
{
	return IRepresentationFactory::RepresentationObjectPtr();
}


} // namespace imtloggui


