#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsScene>

// Acf includes
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtloggui/IEventItem.h>
#include <imtloggui/IEventScenePositionProvider.h>

namespace imtloggui
{


class IEventItemController
{
public:
	typedef QMultiMap<QDateTime, IItemBase*> EventMap;

	virtual void SetScene(QGraphicsScene* scenePtr) = 0;
	virtual void SetTimeAxis(const IEventScenePositionProvider* timeAxisPtr) = 0;
	virtual bool CreateGraphicsItem() = 0;
	virtual bool DestroyGraphicsItem() = 0;
	virtual QGraphicsItem* GetGraphicsItem() = 0;

	virtual QByteArray GetGroupId() const = 0;
	virtual int GetGroupHeight() const = 0;
	virtual QString GetGroupName() const = 0;
	virtual void SetGroupName(const QString& name) = 0;

	virtual QVector<int> GetSupportedMessageIds() const = 0;
	virtual const EventMap* GetEvents() const = 0;
	virtual int GetEventCount(istd::IInformationProvider::InformationCategory category) = 0;
	virtual IEventItem* AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr) = 0;
	virtual void ClearEvents() = 0;

	virtual void SetVisible(bool isVisible) const = 0;

	virtual void OnAxisBeginTimeChanged(const QDateTime& oldTime, const QDateTime& newTime) = 0;
	virtual void OnAxisEndTimeChanged(const QDateTime& oldTime, const QDateTime& newTime) = 0;
	virtual void OnViewPortChanged() = 0;
};


} // namespace imtloggui


