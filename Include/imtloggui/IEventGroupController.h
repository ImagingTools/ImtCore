#pragma once


// Acf includes
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtloggui/IEventItemController.h>
#include <imtloggui/IGroupProvider.h>
#include <imtloggui/CEventGraphicsView.h>
#include <imtloggui/IGroup.h>


namespace imtloggui
{


class IEventGroupController: virtual public istd::IPolymorphic
{
public:
	virtual void SetTimeAxis(const IEventScenePositionProvider* timeAxisPtr) = 0;

	virtual IEventItemController* AddGroup(const QByteArray& groupId) = 0;
	virtual bool RemoveGroup(const QByteArray& groupId) = 0;
	virtual QByteArrayList GetAvailableGroupList() const = 0;
	virtual QByteArrayList GetActiveGroupList() const = 0;
	virtual IGroup* GetGroup(const QByteArray& groupId) const = 0;

	virtual IEventItem* AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr) = 0;
};


} // namespace imtloggui


