#pragma once


// ImtCore includes
#include <imtloggui/CRepresentationControllerCompBase.h>
#include <imtlog/IEventFilter.h>


namespace imtloggui
{


class CEventBasedRepresentationControllerComp: public CRepresentationControllerCompBase
{
public:
	typedef CRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CEventBasedRepresentationControllerComp)
	I_END_COMPONENT

protected:
	// reimplemented (CRepresentationControllerCompBase)
	virtual void BuildRepresentation(
				istd::IChangeable& representation,
				const imtlog::IEventProvider& eventProvider,
				const imtlog::ITimeRangeProvider* timeRangeProviderPtr) const override;

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

		// reimplemented (imtlog::IEventFilter)
		virtual bool IsMessageAccepted(const istd::IInformationProvider* messagePtr = nullptr) const override;
		virtual QList<int> GetGroupMessageIds() const override;

		// reimplemented (imtlog::ITimeRangeProvider)
		virtual imtlog::CTimeRange GetTimeRange() const override;

	private:
		imtlog::CTimeRange m_timeRange;
		QList<int> m_messageIdList;
		imtlog::IEventFilter::FilterMode m_filterMode;
	};
};


} // namespace imtloggui


