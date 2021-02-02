#pragma once


// Acf includes
#include <icomp/CComponentBase.h>
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtloggui/IEventItemFactory.h>
#include <imtloggui/CRepresentationViewCompBase.h>
#include <imtloggui/CProductionSpeedItem.h>


namespace imtloggui
{


class CProductionRepresentationViewComp: public CRepresentationViewCompBase
{
public:
	typedef CRepresentationViewCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductionRepresentationViewComp)
	I_END_COMPONENT

protected:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet);

protected:
	// reimplemented (imtloggui::CRepresentationViewCompBase)
	virtual void OnRepresentationUpdated() override;
};


} // namespace imtloggui


