#pragma once


// ACF includes
#include <ilog/CMessageContainer.h>
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtlog/IEventFilter.h>


namespace imtlog
{


class IEventProvider: virtual public istd::IPolymorphic
{
public:
	enum ResultStatus
	{
		RS_OK = 100,
		RS_CANCELED
	};

	typedef istd::TSmartPtr<ilog::IMessageContainer> EventContainerPtr;

	virtual EventContainerPtr GetEvents(
				const IEventFilter* filterPtr = nullptr,
				const IMessageFilterParams* filterParamsPtr = nullptr) const = 0;
};


} // namespace imtlog

