#pragma once


// Acf includes
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtloggui/IEventItemController.h>
#include <imtloggui/CEventGraphicsView.h>


namespace imtloggui
{


class IEventGroupController
{
public:
	virtual void SetScene(QGraphicsScene* scenePtr) = 0;
	virtual void SetView(CEventGraphicsView* viewPtr) = 0;
	virtual void SetTimeAxis(const IEventScenePositionProvider* timeAxisPtr) = 0;
	virtual bool CreateGraphicsItem() = 0;
	virtual bool DestroyGraphicsItem() = 0;
	virtual QGraphicsItem* GetGraphicsItem() = 0;

	virtual IEventItemController* AddGroup(const QByteArray& groupId) = 0;
	virtual bool RemoveGroup(const QByteArray& groupId) = 0;
	virtual QByteArrayList GetAvailableGroupList() const = 0;
	virtual QByteArrayList GetActiveGroupList() const = 0;
	virtual IEventItemController* GetGroup(const QByteArray& groupId) const = 0;

	virtual CEventItemBase* AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr) = 0;

	virtual void OnAxisPosChanged(const QPointF& oldPos, const QPointF& newPos) = 0;
	virtual void OnAxisBeginTimeChanged(const QDateTime& oldTime, const QDateTime& newTime) = 0;
	virtual void OnAxisEndTimeChanged(const QDateTime& oldTime, const QDateTime& newTime) = 0;
	virtual void OnViewPortChanged() = 0;
};


} // namespace imtloggui


