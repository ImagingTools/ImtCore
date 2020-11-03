#pragma once


// Acf includes
#include <icomp/CComponentBase.h>
#include <ilog/IMessageContainer.h> 

// ImtCore includes
#include <imtloggui/IRepresentationFactory.h>
#include <imtlog/IEventTimeRangeFilter.h>
#include <imtlog/IEventMessageIdFilter.h>
#include <imtlog/IStorage.h>


namespace imtloggui
{


class CRepresentationIntervalStatisticsFactoryComp:
			public icomp::CComponentBase,
			virtual public IRepresentationFactory
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationIntervalStatisticsFactoryComp)
		I_REGISTER_INTERFACE(IRepresentationFactory);
		I_ASSIGN(m_intervalAttrPtr, "TimeInterval", "Time interval in seconds", true, 60);
		I_ASSIGN(m_messageContainerCompPtr, "MessageContainer", "Message container", true, "MessageContainer");
		I_ASSIGN_TO(m_timeRangeFilterCompPtr, m_messageContainerCompPtr, true);
		I_ASSIGN_TO(m_messageIdFilterCompPtr, m_messageContainerCompPtr, true);
		I_ASSIGN(m_storageCompPtr, "Storage", "Storage", true, "Storage");
	I_END_COMPONENT

	// reimplemented (imtloggui::IRepresentationViewFactory)
	virtual RepresentationObjectPtr CreateRepresentationObject(
				const imtlog::CTimeRange& timeRange,
				const QList<int>& messageIdList,
				imtlog::IEventMessageIdFilter::Mode mode) const override;

private:
	I_ATTR(int, m_intervalAttrPtr);
	I_REF(ilog::IMessageContainer, m_messageContainerCompPtr);
	I_REF(imtlog::IEventTimeRangeFilter, m_timeRangeFilterCompPtr);
	I_REF(imtlog::IEventMessageIdFilter, m_messageIdFilterCompPtr);
	I_REF(imtlog::IStorage, m_storageCompPtr);
};


} // namespace imtloggui


