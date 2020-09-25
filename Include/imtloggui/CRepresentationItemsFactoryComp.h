#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>

// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IRepresentationItemsFactory.h>
#include <imtloggui/IEventItemFactory.h>
#include <imtloggui/IEventScenePositionProvider.h>


namespace imtloggui
{


class CRepresentationItemsFactoryComp:
			public icomp::CComponentBase,
			virtual public IRepresentationItemsFactory
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationItemsFactoryComp)
		I_REGISTER_INTERFACE(IRepresentationItemsFactory);
		I_ASSIGN(m_positionProviderCompPtr, "ScenePositionProvider", "Scene position provider", true, "ScenePositionProvider");
		I_ASSIGN(m_eventItemFactoryCompPtr, "EventItemFactory", "Event item factory component", false, "");
	I_END_COMPONENT

	// reimplemented (imtloggui::IRepresentationItemsFactory)
	virtual GraphicsItemList CreateGraphicItems(const istd::IChangeable* objectPtr) const override;

private:
	I_REF(IEventScenePositionProvider, m_positionProviderCompPtr);
	I_REF(IEventItemFactory, m_eventItemFactoryCompPtr);
};


} // namespace imtloggui


