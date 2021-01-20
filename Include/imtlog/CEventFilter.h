#pragma once


// ImtCore includes
#include <imtlog/IEventFilter.h>
#include <imtlog/IMessageFilterParams.h>


namespace imtlog
{


class CEventFilter: public imtlog::IEventFilter
{
public:
	// reimplemented (imtlog::IEventFilter)
	virtual bool IsMessageAccepted(
				const istd::IInformationProvider& message,
				const imtlog::IMessageFilterParams* filterParamsPtr) const override;
};


} // namespace imtlog


