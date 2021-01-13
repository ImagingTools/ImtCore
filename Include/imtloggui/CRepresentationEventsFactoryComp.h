#pragma once


// Acf includes
#include <ilog/IMessageContainer.h> 
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IRepresentationFactory.h>
#include <imtlog/IEventFilter.h>
#include <imtlog/IEventProvider.h>
#include <imtlog/IStorage.h>


namespace imtloggui
{


class CRepresentationEventsFactoryComp:
			public icomp::CComponentBase,
			virtual public IRepresentationFactory
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationEventsFactoryComp)
		I_REGISTER_INTERFACE(IRepresentationFactory);
		I_ASSIGN(m_messageHistoryProviderCompPtr, "MessageHistoryProvider", "Message history provider", true, "");
		I_ASSIGN(m_storageCompPtr, "Storage", "Storage", true, "Storage");
	I_END_COMPONENT

	// reimplemented (imtloggui::IRepresentationViewFactory)
	virtual RepresentationObjectPtr CreateRepresentationObject(
				const imtlog::CTimeRange& timeRange,
				const QList<int>& messageIdList,
				imtlog::IEventFilter::FilterMode filterMode) const override;

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
	I_REF(imtlog::IEventProvider, m_messageHistoryProviderCompPtr);
	I_REF(imtlog::IStorage, m_storageCompPtr);
};


} // namespace imtloggui


