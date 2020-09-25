#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsScene>

// Acf includes
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtloggui/IEventItem.h>
#include <imtloggui/ILayerProvider.h>
#include <imtloggui/IEventScenePositionProvider.h>
#include <imtloggui/CEventItemBase.h>

namespace imtloggui
{


class IEventItemController: virtual public istd::IPolymorphic
{
public:
	typedef QMultiMap<QDateTime, const ilog::IMessageConsumer::MessagePtr> EventMap;

	virtual void SetTimeAxis(const IEventScenePositionProvider* timeAxisPtr) = 0;

	virtual QByteArray GetGroupId() const = 0;
	virtual int GetGroupHeight() const = 0;
	virtual QString GetGroupName() const = 0;
	virtual void SetGroupName(const QString& name) = 0;

	virtual QVector<int> GetSupportedMessageIds() const = 0;
	virtual const EventMap* GetEvents() const = 0;
	virtual int GetEventCount(istd::IInformationProvider::InformationCategory category) = 0;
	virtual bool AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr) = 0;
	virtual void ClearEvents() = 0;
};


} // namespace imtloggui


