// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>


namespace imtdesk
{


/**
	Interface for storing and retrieving entity references.

	Entity references allow linking external domain objects (users, devices,
	products, etc.) to tickets. Each reference is stored with metadata
	(entity type, entity ID, display name, navigation URL) and assigned
	a unique ID that can be referenced from tickets via a junction table.

	Follows the same pattern as imtchat::IAttachmentStorage for attachments.

	\ingroup imtdesk
*/
class IEntityReferenceStorage: virtual public istd::IPolymorphic
{
public:
	/**
		Create a new entity reference and return its generated ID.

		\param entityType Type name of the referenced entity (e.g. "Devices").
		\param entityId Unique identifier of the referenced entity.
		\param displayName Human-readable display name for this reference.
		\param entityUrl Navigation URL path (e.g. "collectionId/typeId/entityId").
		\return Generated entity reference ID, or empty QByteArray on failure.
	*/
	virtual QByteArray CreateEntityReference(
				const QString& entityType,
				const QByteArray& entityId,
				const QString& displayName,
				const QString& entityUrl) = 0;

	/**
		Retrieve an entity reference by its ID.

		\param refId Unique entity reference identifier.
		\param[out] entityType Type name of the referenced entity.
		\param[out] entityId Unique identifier of the referenced entity.
		\param[out] displayName Human-readable display name.
		\param[out] entityUrl Navigation URL path.
		\return true if the entity reference was found and loaded.
	*/
	virtual bool GetEntityReference(
				const QByteArray& refId,
				QString& entityType,
				QByteArray& entityId,
				QString& displayName,
				QString& entityUrl) const = 0;
};


} // namespace imtdesk
