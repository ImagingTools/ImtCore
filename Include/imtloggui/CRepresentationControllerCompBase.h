#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <imod/TModelWrap.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlog/IEventProvider.h>
#include <imtlog/ITimeRangeProvider.h>


namespace imtloggui
{


class CRepresentationControllerCompBase:
			public icomp::CComponentBase,
			public imod::TSingleModelObserverBase<imtlog::ITimeRangeProvider>
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CRepresentationControllerCompBase)
		I_REGISTER_INTERFACE(imod::IObserver);
		I_ASSIGN(m_eventProviderCompPtr, "EventProvider", "Event provider", true, "EventProvider");
		I_ASSIGN(m_timeRangeProviderCompPtr, "TimeRangeProvider", "TimeRangeProvider", false, "TimeRangeProvider");
		I_ASSIGN(m_representationCompPtr, "RepresentationData", "Representation data model", true, "RepresentationData");
	I_END_COMPONENT

protected:
	virtual void BuildRepresentation(
				istd::IChangeable& representation,
				const imtlog::IEventProvider& eventProvider,
				const imtlog::ITimeRangeProvider* timeRangeProviderPtr) const = 0;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	I_REF(imtlog::IEventProvider, m_eventProviderCompPtr);
	I_REF(imtlog::ITimeRangeProvider, m_timeRangeProviderCompPtr);
	I_REF(istd::IChangeable, m_representationCompPtr);
};


} // namespace imtloggui


