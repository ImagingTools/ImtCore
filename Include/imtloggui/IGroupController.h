#pragma once


// ImtCore includes
#include <imtloggui/IEventController.h>
#include <istd/IPolymorphic.h>


namespace imtloggui
{


/**
	Interface for group controller
*/
class IGroupController: virtual public IGraphicsItemController
{
public:
	/**
		Get group ids connected to group controller
	*/
	virtual QByteArrayList GetGroupList() const = 0;

	/**
		Get group for gived id
	*/
	virtual IEventController* GetGroup(const QByteArray& groupId) const = 0;

	/**
		Change group order
		Index ordered from bottom to top on the screen
	*/
	virtual bool MoveGroup(int fromIndex, int toIndex) = 0;

	/**
		Get visibility of group for gived id
	*/
	virtual bool GetVisible(const QByteArray& groupId) = 0;

	/**
		Set visibility of group for gived id
	*/
	virtual bool SetVisible(const QByteArray& groupId, bool isVisible) = 0;
};


} // namespace imtloggui


