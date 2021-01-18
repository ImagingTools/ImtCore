#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IRepresentationView.h>
#include <imtloggui/IEventItemFactory.h>
#include <imtloggui/IEventScenePositionProvider.h>
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

	// reimplemented (imtloggui::IRepresentationViewFactory)
	virtual bool SetModelPtr(imod::IModel* modelPtr) override;
	virtual GraphicsItemList GetGraphicItems() const override;

private:
	mutable QMap<QByteArray, GraphicsItem> m_items;
};


} // namespace imtloggui


