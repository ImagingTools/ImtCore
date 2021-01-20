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
	class CEventFilter: public imtlog::IEventFilter
	{
	public:
		// reimplemented (imtlog::IEventFilter)
		virtual bool IsMessageAccepted(
					const istd::IInformationProvider& message,
					const imtlog::CTimeRange* timeRangePtr,
					const imtlog::IMessageFilterParams* filterParamsPtr) const override;
	};
};


} // namespace imtloggui


