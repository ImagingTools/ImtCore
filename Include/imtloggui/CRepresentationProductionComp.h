#pragma once


// Qt includes
#include <QtCore/QDateTime.h>

// Acf includes
#include <ilog/IMessageContainer.h>

// ImtCore includes
#include <imtloggui/IRepresentation.h>
#include <imtlog/IEventFilter.h>
#include <imtlog/IEventProvider.h>
#include <imtlog/IStorage.h>
#include <imtlog/ITimeRangeProvider.h>
#include <imtloggui/CRepresentationCompBase.h>


namespace imtloggui
{


class CRepresentationProductionComp: public CRepresentationCompBase
{
public:
	typedef CRepresentationCompBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationProductionComp)
		I_ASSIGN(m_granularityAttrPtr, "Granularity", "Statistics time granularity in seconds", true, 60);
	I_END_COMPONENT

	// reimplemented (imtloggui::IRepresentation)
	virtual imod::IModel* GetRepresentationModel() override;
	virtual imod::IObserver* GetTimeRangeObserver() override;

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = nullptr) const override;
	virtual void AddMessage(const MessagePtr& messagePtr) override;

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
		virtual QList<int> GetGroupMessageIds() const override;

		// reimplemented (imtlog::ITimeRangeProvider)
		virtual imtlog::CTimeRange GetTimeRange() const override;

	private:
		imtlog::CTimeRange m_timeRange;
		QList<int> m_messageIdList;
		imtlog::IEventFilter::FilterMode m_filterMode;
	};

private:
	I_ATTR(int, m_granularityAttrPtr);

	istd::TSmartPtr<istd::IChangeable> m_modelPtr;
};


} // namespace imtloggui


