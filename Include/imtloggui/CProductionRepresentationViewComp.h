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

	// reimplemented (imtloggui::IRepresentationView)
	virtual GraphicsItemList GetItems() const override;
	virtual GraphicsItemList GetAddedItems() const override;
	virtual GraphicsItemList GetRemovedItems() const override;

protected:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet);

private:
	mutable GraphicsItem m_item;
};


} // namespace imtloggui


