#pragma once


// Acf includes
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtlog/IMessageGroupInfoProvider.h>
#include <imtloggui/IEventItemController.h>


namespace imtloggui
{


class IEventGroupController
{
public:
	virtual void SetScene(QGraphicsScene* scenePtr) = 0;
	virtual void SetView(QGraphicsView* viewPtr) = 0;
	virtual void SetTimeAxis(const IEventScenePositionProvider* timeAxisPtr) = 0;

	virtual bool CreateGraphicsItem() = 0;
	virtual bool DestroyGraphicsItem() = 0;
	virtual QGraphicsItem* GetGraphicsItem() = 0;

	virtual IEventItemController* AddGroup(const QByteArray& groupId, const QString& groupName) = 0;
	virtual void AddGroups(const imtlog::IMessageGroupInfoProvider::GroupInfos& groupInfos) = 0;
	virtual bool RemoveGroup(const QByteArray& groupId) = 0;

	virtual QByteArrayList GetActiveGroupList() const = 0;
	virtual QByteArrayList GetAvailableGroupList() const = 0;
	virtual IEventItemController* GetGroup(const QByteArray& groupId) const = 0;
	virtual QString GetGroupName(const QByteArray& groupId) const = 0;

	virtual bool SetVisible(const QByteArray& groupId, bool isVisible) const = 0;

	virtual void OnTimeAxisChanged() = 0;
	virtual double OnViewPortChanged() = 0;
};


} // namespace imtloggui


