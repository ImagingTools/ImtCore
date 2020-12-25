#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>

// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IRepresentationViewFactory.h>
#include <imtloggui/IEventItemFactory.h>
#include <imtloggui/IEventScenePositionProvider.h>
#include <imtloggui/CRepresentationViewFactoryCompBase.h>
#include <imtloggui/CProductionSpeedItem.h>


namespace imtloggui
{


class CRepresentationProductionViewFactoryComp: public imtloggui::CRepresentationViewFactoryCompBase
{
public:
	typedef CRepresentationViewFactoryCompBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationProductionViewFactoryComp)
	I_END_COMPONENT

	// reimplemented (imtloggui::IRepresentationViewFactory)
	virtual GraphicsItemList CreateGraphicItems(const istd::TSmartPtr<istd::IChangeable> objectPtr, const QByteArray& groupId) const override;

private:
	mutable QMap<QByteArray, GraphicsItem> m_items;
};


} // namespace imtloggui


