#pragma once


// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>


namespace imtauth
{


class IUserRecentAction: virtual public iser::IObject
{
public:
	enum ActionType
	{
		AT_UNKNOWN,
		AT_CREATE,
		AT_UPDATE,
		AT_DELETE,
		AT_USER
	};
	I_DECLARE_ENUM(ActionType, AT_UNKNOWN, AT_CREATE, AT_UPDATE, AT_UPDATE, AT_DELETE, AT_USER);

	enum MetaInfoTypes
	{
		MIT_USER_ID = idoc::IDocumentMetaInfo::MIT_USER + 1,
		MIT_TARGET_ID,
		MIT_TARGET_TYPE_ID,
		MIT_ACTION_TYPE
	};

	virtual QByteArray GetUserId() const = 0;
	virtual void SetUserId(const QByteArray& userId) = 0;

	virtual ActionType GetActionType() const = 0;
	virtual void SetActionType(ActionType actionType) = 0;

	virtual QByteArray GetTargetId() const = 0;
	virtual void SetTargetId(const QByteArray& targetId) = 0;

	virtual QByteArray GetTargetTypeId() const = 0;
	virtual void SetTargetTypeId(const QByteArray& typeId) = 0;

	virtual QDateTime GetTimestamp() const = 0;
	virtual void SetTimestamp(const QDateTime& timestamp) = 0;
};


} // namespace imtauth


