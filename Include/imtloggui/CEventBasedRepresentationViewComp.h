#pragma once


// Acf includes
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

	// reimplemented (imtloggui::IRepresentationView)
	virtual GraphicsItemList GetItems() const override;
	virtual GraphicsItemList GetAddedItems() const override;
	virtual GraphicsItemList GetRemovedItems() const override;

private:
	I_REF(IEventItemFactory, m_eventItemFactoryCompPtr);

	mutable QMap<QByteArray, QMap<uint64_t, GraphicsItem>> m_items;
};


} // namespace imtloggui


