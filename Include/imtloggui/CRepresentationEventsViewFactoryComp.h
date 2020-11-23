#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>

// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IEventItemFactory.h>
#include <imtloggui/CRepresentationViewFactoryCompBase.h>


namespace imtloggui
{


class CRepresentationEventsViewFactoryComp: public imtloggui::CRepresentationViewFactoryCompBase
{
public:
	typedef CRepresentationViewFactoryCompBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationEventsViewFactoryComp)
		I_ASSIGN(m_eventItemFactoryCompPtr, "EventItemFactory", "Event item factory component", false, "");
	I_END_COMPONENT

	// reimplemented (imtloggui::IRepresentationViewFactory)
	virtual GraphicsItemList CreateGraphicItems(const istd::TSmartPtr<istd::IChangeable> objectPtr, const QByteArray& groupId) const override;

private:
	I_REF(IEventItemFactory, m_eventItemFactoryCompPtr);

	mutable QMap<QByteArray, QMap<uint64_t, IEventItem*>> m_items;
};


} // namespace imtloggui


