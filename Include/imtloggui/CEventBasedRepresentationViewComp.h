#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtloggui/IEventItemFactory.h>
#include <imtloggui/CRepresentationViewCompBase.h>


namespace imtloggui
{


class CEventBasedRepresentationViewComp: public imtloggui::CRepresentationViewCompBase
{
public:
	typedef CRepresentationViewCompBase BaseClass;

	I_BEGIN_COMPONENT(CEventBasedRepresentationViewComp)
		I_ASSIGN(m_eventItemFactoryCompPtr, "EventItemFactory", "Event item factory component", false, "");
	I_END_COMPONENT

protected:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet);

private:
	I_REF(IEventItemFactory, m_eventItemFactoryCompPtr);
};


} // namespace imtloggui


