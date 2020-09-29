#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>

// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IRepresentationViewFactory.h>
#include <imtloggui/IEventItemFactory.h>
#include <imtloggui/IEventScenePositionProvider.h>


namespace imtloggui
{


class CRepresentationViewFactoryComp:
			public icomp::CComponentBase,
			virtual public IRepresentationViewFactory
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationViewFactoryComp)
		I_REGISTER_INTERFACE(IRepresentationViewFactory);
		I_ASSIGN(m_positionProviderCompPtr, "ScenePositionProvider", "Scene position provider", true, "ScenePositionProvider");
		I_ASSIGN(m_eventItemFactoryCompPtr, "EventItemFactory", "Event item factory component", false, "");
	I_END_COMPONENT

	// reimplemented (imtloggui::IRepresentationViewFactory)
	virtual GraphicsItemList CreateGraphicItems(const istd::IChangeable* objectPtr) const override;

private:
	I_REF(IEventScenePositionProvider, m_positionProviderCompPtr);
	I_REF(IEventItemFactory, m_eventItemFactoryCompPtr);
};


} // namespace imtloggui


