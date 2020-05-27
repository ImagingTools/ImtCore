#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsScene>

// Acf includes
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtloggui/CEventItemBase.h>
#include <imtloggui/IEventScenePositionProvider.h>

namespace imtloggui
{


class IEventItemController
{
public:
	typedef QMultiMap<QDateTime, CEventItemBase*> EventMap;

	virtual void SetScene(QGraphicsScene* scenePtr) = 0;
	virtual void SetTimeAxis(const IEventScenePositionProvider* timeAxisPtr) = 0;

	virtual bool CreateGraphicsItem() = 0;
	virtual bool DestroyGraphicsItem() = 0;
	virtual QGraphicsItem* GetGraphicsItem() = 0;

	virtual QByteArray GetGroupId() const = 0;
	virtual int GetGroupHeight() const = 0;

	virtual QString GetGroupName() const = 0;
	virtual void SetGroupName(const QString& name) = 0;

	virtual const EventMap* GetEvents() const = 0;
	virtual CEventItemBase* AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr) = 0;
	virtual void ClearEvents() = 0;

	virtual void SetVisible(bool isVisible) const = 0;

	virtual void OnTimeAxisChanged() = 0;
	virtual void OnViewPortChanged() = 0;
};


} // namespace imtloggui


