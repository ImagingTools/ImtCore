#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtloggui/IEventItemFactory.h>
#include <imtloggui/CRepresentationViewCompBase.h>
#include <imtloggui/CProductionSpeedItem.h>


namespace imtloggui
{


class CRepresentationProductionViewComp: public CRepresentationViewCompBase
{
public:
	typedef CRepresentationViewCompBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationProductionViewComp)
	I_END_COMPONENT

	// reimplemented (imtloggui::IRepresentationView)
	virtual imod::IObserver* GetRepresentationObserver() override;

	// reimplemented (imtloggui::IRepresentationView)
	virtual GraphicsItemList GetItems() const override;
	virtual GraphicsItemList GetAddedItems() const override;
	virtual GraphicsItemList GetRemovedItems() const override;

private:
	mutable QMap<QByteArray, GraphicsItem> m_items;
};


} // namespace imtloggui


