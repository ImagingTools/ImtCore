#pragma once


// Qt includes
#include <QtWidgets/QGraphicsScene>

// Acf includes
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtloggui/IEventScenePositionProvider.h>

namespace imtloggui
{


class IEventItemController
{
public:
	virtual void SetScene(QGraphicsScene* scenePtr) = 0;
	virtual void SetTimeAxis(const IEventScenePositionProvider* timeAxisPtr) = 0;
	
	virtual bool CreateGroupItem() = 0;
	virtual bool DestroyGroupItem() = 0;

	virtual QByteArray GetGroupId() const = 0;

	virtual QString GetGroupName() const = 0;
	virtual void SetGroupName(QString name) = 0;

	virtual QGraphicsItem* AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr) = 0;
	virtual void ClearEvents() = 0;

	virtual void SetVisible(bool isVisible) const = 0;

	virtual void TimeAxisChanged() = 0;
	virtual void ViewPortChanged() = 0;
};


} // namespace imtloggui


