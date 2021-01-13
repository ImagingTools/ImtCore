#pragma once


// Qt includes
#include <QtCore/QDateTime.h>

// Acf includes
#include <ilog/IMessageContainer.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IRepresentationFactory.h>
#include <imtlog/IEventFilter.h>
#include <imtlog/IEventProvider.h>
#include <imtlog/IStorage.h>
#include <imtlog/ITimeRangeProvider.h>
#include <imtloggui/CRepresentationProductionModel.h>


namespace imtloggui
{


class CRepresentationProductionFactoryComp:
			public icomp::CComponentBase,
			virtual public IRepresentationFactory,
			virtual public ilog::IMessageConsumer
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationProductionFactoryComp)
		I_REGISTER_INTERFACE(IRepresentationFactory);
		I_REGISTER_INTERFACE(IMessageConsumer);
		I_ASSIGN(m_granularityAttrPtr, "Granularity", "Statistics time granularity in seconds", true, 60);
		I_ASSIGN(m_messageHistoryProviderCompPtr, "MessageHistoryProvider", "Message history provider", true, "");
		I_ASSIGN_TO(m_timeRangeProviderCompPtr, m_messageHistoryProviderCompPtr, true);
		I_ASSIGN(m_storageCompPtr, "Storage", "Storage", true, "Storage");
	I_END_COMPONENT

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = nullptr) const override;
	virtual void AddMessage(const MessagePtr& messagePtr) override;

	// reimplemented (imtloggui::IRepresentationViewFactory)
	virtual RepresentationObjectPtr CreateRepresentationObject(
				const imtlog::CTimeRange& timeRange,
				const QList<int>& messageIdList,
				imtlog::IEventFilter::FilterMode filterMode) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	class Filter: public imtlog::IEventFilter
	{
	public:
		Filter(const imtlog::CTimeRange& timeRange, const QList<int>& messageIdList, FilterMode filterMode)
			:m_timeRange(timeRange),
			m_messageIdList(messageIdList),
			m_filterMode(filterMode)
		{
		}

		// reimplemented (imtlog::IMessageFilter)
		bool IsMessageAccepted(const istd::IInformationProvider* messagePtr = nullptr) const override;

		// reimplemented (imtlog::ITimeRangeProvider)
		imtlog::CTimeRange GetTimeRange() const override;

	private:
		imtlog::CTimeRange m_timeRange;
		QList<int> m_messageIdList;
		imtlog::IEventFilter::FilterMode m_filterMode;
	};

private:
	I_ATTR(int, m_granularityAttrPtr);
	I_REF(imtlog::IEventProvider, m_messageHistoryProviderCompPtr);
	I_REF(imtlog::ITimeRangeProvider, m_timeRangeProviderCompPtr);
	I_REF(imtlog::IStorage, m_storageCompPtr);

	istd::TSmartPtr<istd::IChangeable> m_modelPtr;
};


} // namespace imtloggui


