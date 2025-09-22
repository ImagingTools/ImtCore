#pragma once


// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>


namespace imtauth
{


/**
* @brief The IUserRecentAction class
* @details
* Interface for storing and accessing information about user actions in the system.
*/
class IUserRecentAction : virtual public iser::IObject
{
public:
	/**
	* @brief Enumeration of possible user action types.
	*/
	enum ActionType
	{
		AT_UNKNOWN,	///< Unknown action (default).
		AT_CREATE,	///< Entity creation.
		AT_UPDATE,	///< Updating an existing entity.
		AT_DELETE,	///< Deleting an entity.
		AT_USER		///< Custom user-defined action.
	};
	I_DECLARE_ENUM(ActionType, AT_UNKNOWN, AT_CREATE, AT_UPDATE, AT_UPDATE, AT_DELETE, AT_USER);

	/**
	* @brief Enumeration of metadata types related to the action.
	*/
	enum MetaInfoTypes
	{
		MIT_USER_ID = idoc::IDocumentMetaInfo::MIT_USER + 1,
		MIT_USER_NAME,
		MIT_TARGET_ID,
		MIT_TARGET_NAME,
		MIT_TARGET_SOURCE,
		MIT_TARGET_TYPE_ID,
		MIT_ACTION_TYPE
	};

	/**
	* @brief Information about the action target entity.
	*/
	struct TargetInfo
	{
		QByteArray id;		///< Target entity identifier.
		QByteArray typeId;	///< Target entity type identifier (e.g., "Order", "Task").
		QByteArray source;	///< Source or origin of the target entity (e.g., module or subsystem).
		QString name;		///< Human-readable target entity name.

		bool operator == (const TargetInfo& other) const
		{
			return	(id == other.id) &&
					(typeId == other.typeId) &&
					(source == other.source) &&
					(name == other.name);
		}

		bool operator != (const TargetInfo& other) const
		{
			return !(*this == other);
		}
	};

	/**
	* @brief Returns the user identifier.
	* @return QByteArray containing the user ID.
	*/
	virtual QByteArray GetUserId() const = 0;

	/**
	* @brief Sets the user identifier.
	* @param userId User ID to assign.
	*/
	virtual void SetUserId(const QByteArray& userId) = 0;

	/**
	* @brief Returns the type of action.
	* @return ActionType describing the action.
	*/
	virtual ActionType GetActionType() const = 0;

	/**
	* @brief Sets the type of action.
	* @param actionType Value from the ActionType enumeration.
	*/
	virtual void SetActionType(ActionType actionType) = 0;

	/**
	* @brief Returns detailed information about the target entity.
	* @return TargetInfo structure with target details.
	*/
	virtual TargetInfo GetTargetInfo() const = 0;

	/**
	* @brief Sets detailed information about the target entity.
	* @param targetInfo TargetInfo structure with target details.
	*/
	virtual void SetTargetInfo(TargetInfo targetInfo) = 0;

	/**
	* @brief Returns the timestamp of the action.
	* @return QDateTime representing when the action was performed.
	*/
	virtual QDateTime GetTimestamp() const = 0;

	/**
	* @brief Sets the timestamp of the action.
	* @param timestamp Date and time of the action.
	*/
	virtual void SetTimestamp(const QDateTime& timestamp) = 0;
};


} // namespace imtauth


