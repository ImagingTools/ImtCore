#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlog/IEventProvider.h>
#include <imtlog/ITimeRangeProvider.h>
#include <imtlog/IStorage.h>
#include <imtloggui/IRepresentation.h>
#include <imtloggui/IEventScenePositionProvider.h>


namespace imtloggui
{


class CRepresentationCompBase:
			public icomp::CComponentBase,
			virtual public ilog::IMessageConsumer,
			virtual public IRepresentation
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CRepresentationCompBase)
		I_REGISTER_INTERFACE(IRepresentation);
		I_ASSIGN(m_eventProviderCompPtr, "EventProvider", "Event provider", true, "EventProvider");
		I_ASSIGN_TO(m_timeRangeProviderCompPtr, m_eventProviderCompPtr, true);
		I_ASSIGN(m_storageCompPtr, "Storage", "Storage", false, "");
	I_END_COMPONENT

protected:
	I_REF(imtlog::IEventProvider, m_eventProviderCompPtr);
	I_REF(imtlog::ITimeRangeProvider, m_timeRangeProviderCompPtr);
	I_REF(imtlog::IStorage, m_storageCompPtr);
};


} // namespace imtloggui


