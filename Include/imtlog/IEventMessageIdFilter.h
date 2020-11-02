#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtlog
{


class IEventMessageIdFilter: virtual public istd::IPolymorphic
{
public:
	enum Mode
	{
		M_ACCEPT = 0,
		M_EXCEPT
	};

	virtual Mode GetEventMessageIdFilterMode() const = 0;
	virtual void SetEventMessageIdFilterMode(Mode mode) = 0;
	virtual QList<int> GetEventMessageIdFilter() const = 0;
	virtual bool SetEventMessageIdFilter(const QList<int> & messageIdList) = 0;
	virtual void ClearEventMessageIdFilter() = 0;
};


} // namespace imtlog


