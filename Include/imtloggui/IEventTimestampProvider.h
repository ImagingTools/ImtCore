#pragma once


// Qt includes
#include <QtCore/QDateTime>

// Acf includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


class IEventTimestampProvider: virtual public istd::IPolymorphic
{
public:
	virtual QDateTime GetFirstEventTimestamp() const = 0;
	virtual QDateTime GetPreviousEventTimestamp(const QDateTime& timestamp) const = 0;
	virtual QDateTime GetNextEventTimestamp(const QDateTime& timestamp) const = 0;
	virtual QDateTime GetLastEventTimestamp() const = 0;
};


} // namespace imtloggui


