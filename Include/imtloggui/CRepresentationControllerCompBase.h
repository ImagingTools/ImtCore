#pragma once


// Acf includes
#include <imod/TModelWrap.h>
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlog/IEventFilter.h>
#include <imtlog/IEventProvider.h>
#include <imtlog/IMessageFilterParams.h>
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
		I_ASSIGN(m_eventFilterCompPtr, "EventFilter", "Event filter", false, "EventFilter");
		I_ASSIGN(m_messageFilterParamsCompPtr, "MessageFilterParams", "Message filter params", false, "MessageFilterParams");
	I_END_COMPONENT

protected:
	virtual void BuildRepresentation(
				istd::IChangeable& representation,
				const imtlog::IEventProvider& eventProvider,
				const imtlog::IEventFilter* eventFilterPtr,
				const imtlog::IMessageFilterParams* messageFilterParamsPtr) const = 0;

private:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	I_REF(imtlog::IEventProvider, m_eventProviderCompPtr);
	I_REF(imtlog::ITimeRangeProvider, m_timeRangeProviderCompPtr);
	I_REF(istd::IChangeable, m_representationCompPtr);
	I_REF(imtlog::IEventFilter, m_eventFilterCompPtr);
	I_REF(imtlog::IMessageFilterParams, m_messageFilterParamsCompPtr);
};


} // namespace imtloggui


