#pragma once


// Qt includes
#include <QtCore/QDateTime.h>

// ACF Includes
#include <istd/TSmartPtr.h>
#include <ilog/IMessageContainer.h>


namespace imtloggui
{


class IRepresentationFactory: virtual public istd::IPolymorphic
{
public:
	typedef istd::TSmartPtr<istd::IChangeable> RepresentationObjectPtr;
	struct TimeRange
	{
		QDateTime beginTime;
		QDateTime endTime;
	};

	virtual RepresentationObjectPtr CreateRepresentationObject(
				const ilog::IMessageContainer::Messages& messages,
				const TimeRange& timeRange) const = 0;
};


} // namespace imtloggui


