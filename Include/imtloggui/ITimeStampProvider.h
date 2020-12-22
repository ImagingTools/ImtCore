#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


class IEventItem: virtual public istd::IPolymorphic
{
public:
	virtual const QDateTime GetPrevTimeStamp(const QDateTime& timeStamp ) const = 0;
	virtual const QDateTime GetNextTimeStamp(const QDateTime& timeStamp) const = 0;
};


} // namespace imtloggui


