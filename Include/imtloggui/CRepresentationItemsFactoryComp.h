#pragma once


// Acf includes
#include <icomp/CComponentBase.h>
#include <ilog/IMessageContainer.h> 

// ImtCore includes
#include <imtloggui/IRepresentationFactory.h>
#include <imtlog/IEventTimeRangeFilter.h>


namespace imtloggui
{


class CRepresentationItemsFactoryComp:
			public icomp::CComponentBase,
			virtual public IRepresentationFactory
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationItemsFactoryComp)
		I_REGISTER_INTERFACE(IRepresentationFactory);
		I_ASSIGN(m_messageContainerCompPtr, "MessageContainer", "Message container", true, "");
		I_ASSIGN(m_timeRangeFilterCompPtr, "TimeRangeFilter", "Time range filter", true, "");
	I_END_COMPONENT

	// reimplemented (imtloggui::IRepresentationViewFactory)
	virtual RepresentationObjectPtr CreateRepresentationObject(const imtlog::CTimeRange& timeRange) const override;

private:
	I_REF(ilog::IMessageContainer, m_messageContainerCompPtr);
	I_REF(imtlog::IEventTimeRangeFilter, m_timeRangeFilterCompPtr);
};


} // namespace imtloggui


