#pragma once


// Qt includes
#include <QtCore/QDateTime.h>

// Acf includes
#include <ilog/IMessageContainer.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlog/IEventTimeRangeFilter.h>
#include <imtlog/IEventMessageIdFilter.h>
#include <imtlog/ITimeRangeProvider.h>
#include <imtlog/IMessageHistoryContainer.h>
#include <imtlog/IMessageHistoryConsumer.h>
#include <imtlog/IStorage.h>
#include <imtloggui/IRepresentationFactory.h>
#include <imtloggui/CRepresentationProductionSpeedModel.h>


namespace imtloggui
{


class CRepresentationProductionSpeedFactoryComp:
			public icomp::CComponentBase,
			virtual public IRepresentationFactory,
			virtual public imtlog::IMessageHistoryConsumer
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationProductionSpeedFactoryComp)
		I_REGISTER_INTERFACE(IRepresentationFactory);
		I_REGISTER_INTERFACE(IMessageHistoryConsumer);
		I_ASSIGN(m_granularityAttrPtr, "Granularity", "Statistics time granularity in seconds", true, 60);
		I_ASSIGN(m_messageContainerCompPtr, "MessageContainer", "Message container", true, "MessageContainer");
		I_ASSIGN_TO(m_timeRangeFilterCompPtr, m_messageContainerCompPtr, true);
		I_ASSIGN_TO(m_messageIdFilterCompPtr, m_messageContainerCompPtr, true);
		I_ASSIGN_TO(m_timeRangeProviderCompPtr, m_messageContainerCompPtr, true);
		I_ASSIGN(m_storageCompPtr, "Storage", "Storage", false, "Storage");
	I_END_COMPONENT

	// reimplemented (imtlog::IMessageHistoryConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = NULL) const override;
	virtual void AddMessage(const MessagePtr& messagePtr) override;

	// reimplemented (imtloggui::IRepresentationViewFactory)
	virtual RepresentationObjectPtr CreateRepresentationObject(
				const imtlog::CTimeRange& timeRange,
				const QList<int>& messageIdList,
				imtlog::IEventMessageIdFilter::Mode mode) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(int, m_granularityAttrPtr);
	I_REF(imtlog::IMessageHistoryContainer, m_messageContainerCompPtr);
	I_REF(imtlog::IEventTimeRangeFilter, m_timeRangeFilterCompPtr);
	I_REF(imtlog::IEventMessageIdFilter, m_messageIdFilterCompPtr);
	I_REF(imtlog::ITimeRangeProvider, m_timeRangeProviderCompPtr);
	I_REF(imtlog::IStorage, m_storageCompPtr);

	istd::TSmartPtr<istd::IChangeable> m_modelPtr;
};


} // namespace imtloggui


