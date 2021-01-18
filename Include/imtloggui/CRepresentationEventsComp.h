#pragma once


// Acf includes
#include <ilog/IMessageContainer.h> 
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/CRepresentationCompBase.h>
#include <imtlog/IEventFilter.h>
#include <imtlog/IEventProvider.h>
#include <imtlog/IStorage.h>


namespace imtloggui
{


class CRepresentationEventsComp: public CRepresentationCompBase
{
public:
	typedef CRepresentationCompBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationEventsComp)
	I_END_COMPONENT

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = nullptr) const override;
	virtual void AddMessage(const MessagePtr& messagePtr) override;

	// reimplemented (imtloggui::IRepresentation)

	// reimplemented (icomp::CComponentBase)
	//virtual void OnComponentCreated() override;

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
		virtual bool IsMessageAccepted(const istd::IInformationProvider* messagePtr = nullptr) const override;
		virtual QList<int> GetGroupMessageIds() const override;

		// reimplemented (imtlog::ITimeRangeProvider)
		virtual imtlog::CTimeRange GetTimeRange() const override;

	private:
		imtlog::CTimeRange m_timeRange;
		QList<int> m_messageIdList;
		imtlog::IEventFilter::FilterMode m_filterMode;
	};

private:
	mutable istd::TSmartPtr<istd::IChangeable> m_modelPtr;
};


} // namespace imtloggui


