#pragma once


// ImtCore includes
#include <imtloggui/CRepresentationControllerCompBase.h>


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
				imtlog::IEventProvider::EventContainerPtr containerPtr,
				const imtlog::CTimeRange& timeRange) const override;
};


} // namespace imtloggui


