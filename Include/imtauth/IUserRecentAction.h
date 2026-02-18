// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <iprm/IParamsSet.h>
#include <idoc/IDocumentMetaInfo.h>


namespace imtauth
{


class IUserRecentAction : virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		MIT_USER_ID = idoc::IDocumentMetaInfo::MIT_USER + 1,
		MIT_USER_NAME,
		MIT_TARGET_ID,
		MIT_TARGET_NAME,
		MIT_TARGET_SOURCE,
		MIT_TARGET_TYPE_ID,
		MIT_TARGET_TYPE_NAME,
		MIT_ACTION_TYPE_ID,
		MIT_ACTION_TYPE_NAME,
		MIT_ACTION_TYPE_DESCRIPTION
	};

	typedef std::function<iser::ISerializableSharedPtr(const QByteArray& actionTypeId)> ActionDataFactoryFunction;

	struct TargetInfo
	{
		TargetInfo(
				const QByteArray& id = QByteArray(),
				const QByteArray& typeId = QByteArray(),
				const QString& typeName = QString(),
				const QString& source = QString(),
				const QString& name = QString()):
			id(id),
			typeId(typeId),
			typeName(typeName),
			source(source),
			name(name){}

		QByteArray id;		///< Target entity identifier.
		QByteArray typeId;	///< Target entity type identifier (e.g., "Order", "Task").
		QString typeName;	///< Target entity type name
		QString source;		///< Source or origin of the target entity (e.g., module or subsystem).
		QString name;		///< Human-readable target entity name.

		bool operator == (const TargetInfo& other) const
		{
			return	(id == other.id) &&
					(typeId == other.typeId) &&
					(typeName == other.typeName) &&
					(source == other.source) &&
					(name == other.name);
		}

		bool operator != (const TargetInfo& other) const
		{
			return !(*this == other);
		}
	};

	struct ActionTypeInfo
	{
		ActionTypeInfo(
				const QByteArray& id = QByteArray(),
				const QString& name = QString(),
				const QString& description = QString()):
			id(id),
			description(description),
			name(name){}

		QByteArray id;
		QString name;
		QString description;

		bool operator == (const ActionTypeInfo& other) const
		{
			return	(id == other.id) &&
					(description == other.description) &&
					(name == other.name);
		}

		bool operator != (const ActionTypeInfo& other) const
		{
			return !(*this == other);
		}
	};

	struct UserInfo
	{
		UserInfo(const QByteArray& id = QByteArray(), const QString& name = QString()): id(id), name(name){}

		QByteArray id;
		QString name;

		bool operator == (const UserInfo& other) const
		{
			return	(id == other.id) &&
					(name == other.name);
		}

		bool operator != (const UserInfo& other) const
		{
			return !(*this == other);
		}
	};

	virtual UserInfo GetUserInfo() const = 0;
	virtual void SetUserInfo(UserInfo userInfo) = 0;

	virtual ActionTypeInfo GetActionTypeInfo() const = 0;
	virtual void SetActionTypeInfo(ActionTypeInfo actionTypeInfo) = 0;

	virtual TargetInfo GetTargetInfo() const = 0;
	virtual void SetTargetInfo(TargetInfo targetInfo) = 0;

	virtual QDateTime GetTimestamp() const = 0;
	virtual void SetTimestamp(const QDateTime& timestamp) = 0;

	virtual iser::ISerializableSharedPtr GetActionData() const = 0;
	virtual void SetActionData(const iser::ISerializableSharedPtr& actionDataPtr) = 0;

	virtual void SetActionDataFactory(const ActionDataFactoryFunction& factory) = 0;
};


typedef istd::TUniqueInterfacePtr<IUserRecentAction> IUserActionInfoUniquePtr;
typedef istd::TSharedInterfacePtr<IUserRecentAction> IUserActionInfoSharedPtr;


} // namespace imtauth


