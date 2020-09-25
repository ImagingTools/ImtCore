#pragma once


// Qt includes
#include <QtCore/QDateTime.h>

// ACF Includes
#include <istd/TSmartPtr.h>
#include <ilog/IMessageContainer.h>


namespace imtloggui
{


class IRepresentation: virtual public istd::IPolymorphic
{
public:
	typedef istd::TSmartPtr<istd::IChangeable> RepresentationObjectPtr;
	struct TimeRange
	{
		QDateTime beginTime;
		QDateTime endTime;
	};

	virtual QByteArray GetId() const = 0;
	virtual QString GetName() const = 0;
	virtual QString GetDescription() const = 0;

	virtual RepresentationObjectPtr GetRepresentation(
				const ilog::IMessageContainer::Messages& messages,
				const TimeRange& timeRange) const = 0;
};


} // namespace imtloggui


