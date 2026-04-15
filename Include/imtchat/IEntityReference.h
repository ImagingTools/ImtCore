// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <iser/ISerializable.h>


namespace imtchat
{


/**
	Interface for an entity reference embedded in a chat message.

	Entity references allow linking any ImtCore domain object (user, license,
	product, etc.) directly within a message body.

	\ingroup imtchat
*/
class IEntityReference: virtual public istd::IChangeable
{
public:
	/**
		Get the type name of the referenced entity (e.g. "User", "License").
	*/
	virtual QString GetEntityType() const = 0;

	/**
		Set the entity type name.
	*/
	virtual void SetEntityType(const QString& entityType) = 0;

	/**
		Get the unique identifier of the referenced entity.
	*/
	virtual QByteArray GetEntityId() const = 0;

	/**
		Set the entity identifier.
	*/
	virtual void SetEntityId(const QByteArray& entityId) = 0;

	/**
		Get the human-readable display name for this reference.
	*/
	virtual QString GetDisplayName() const = 0;

	/**
		Set the display name.
	*/
	virtual void SetDisplayName(const QString& displayName) = 0;

	/**
		Get the URL path to navigate to the referenced entity.
	*/
	virtual QString GetEntityUrl() const = 0;

	/**
		Set the entity URL path.
	*/
	virtual void SetEntityUrl(const QString& entityUrl) = 0;

};

typedef istd::TUniqueInterfacePtr<IEntityReference> IEntityReferenceUniquePtr;

} // namespace imtchat
