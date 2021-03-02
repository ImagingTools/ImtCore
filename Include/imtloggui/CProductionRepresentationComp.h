#pragma once


// Qt includes
#include <QtCore/QDateTime.h>
#include <QtCore/QMap.h>

// ACF includes
#include <istd/IChangeable.h>
#include <ilog/IMessageConsumer.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IEventStatistics.h>


namespace imtloggui
{


class CProductionRepresentationComp:
			public icomp::CComponentBase,
			virtual public istd::IChangeable
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CProductionRepresentationComp)
		//I_REGISTER_INTERFACE(imtbase::IEventStatistics);
	I_END_COMPONENT

	CProductionRepresentationComp();

	typedef QMap<qint64, imtbase::IEventStatistics::EventsInfo> Timeline;
	struct MaxCounters;

	quint64 GetGranularity() const;
	void SetGranularity(quint64 granularity);
	const Timeline& GetTimeline() const;
	quint64 GetMaxCount() const;
	const MaxCounters& GetMaxCounters() const;
	void ClearStatistics();
	void AddMessage(const ilog::IMessageConsumer::MessagePtr& message);
	qint64 CalculateIntervalBeginTime(qint64 timeStamp);

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;

public:
	struct MaxCounters
	{
		quint64 oks;
		quint64 warnings;
		quint64 noks;
		quint64 errors;
		quint64 noksErrors;

		MaxCounters()
			:oks(0),
			warnings(0),
			noks(0),
			errors(0),
			noksErrors(0)
		{
		}
	};

private:
	quint64 m_maxCount;
	MaxCounters m_maxCounters;
	qint64 m_granularity;
	Timeline m_timeline;
};


} // namespace imtloggui


