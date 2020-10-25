#pragma once


// Qt includes
#include <QtCore/QByteArray.h>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtlog
{


class IEventMessageIdFilter: virtual public istd::IPolymorphic
{
public:
	virtual QList<int> GetEventMessageIdFilter() const = 0;
	virtual bool SetEventMessageIdFilter(const QList<int> & messageIdList) = 0;
	virtual void ClearEventMessageIdFilter() = 0;
};


} // namespace imtlog


