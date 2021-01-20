#pragma once


// ImtCore includes
#include <imtloggui/CRepresentationControllerCompBase.h>


namespace imtloggui
{


class CProductionRepresentationControllerComp: public CRepresentationControllerCompBase
{
public:
	typedef CRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductionRepresentationControllerComp)
		I_ASSIGN(m_granularityAttrPtr, "Granularity", "Statistics time granularity in seconds", true, 60);
	I_END_COMPONENT

protected:
	// reimplemented (CRepresentationControllerCompBase)
	virtual void BuildRepresentation(
				istd::IChangeable& representation,
				const imtlog::IEventProvider& eventProvider,
				const imtlog::IEventFilter* eventFilterPtr,
				const imtlog::IMessageFilterParams* messageFilterParamsPtr) const override;

private:
	I_ATTR(int, m_granularityAttrPtr);
};


} // namespace imtloggui


