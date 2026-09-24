// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QVector>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtbase
{
	class IObjectCollection;
	class IOperationContext;
}


namespace imttag
{


/**
	Manages assignments of tags to entities.

	Every mutating call is scoped by the tenant of the operation context: a tag must be
	a system tag or belong to that tenant, and every entity must be visible in that tenant.
	Tags are never created implicitly. Each effective change is recorded as a tag event.
	\ingroup imttag
*/
class ITagAssignmentManager: virtual public istd::IPolymorphic
{
public:
	struct EntityTagChange
	{
		QByteArray entityId;
		QByteArrayList addedTagIds;
		QByteArrayList removedTagIds;
	};

	typedef QVector<EntityTagChange> EntityTagChanges;

	/**
		Map of entity ID to the IDs of its active tags.
	*/
	typedef QMap<QByteArray, QByteArrayList> EntityTagIds;

	/**
		Add tags to entities, keeping the tags they already have.
	*/
	virtual bool AddTags(
				const QByteArray& entityType,
				const QByteArrayList& entityIds,
				const QByteArrayList& tagIds,
				const imtbase::IOperationContext* operationContextPtr,
				EntityTagChanges* changesPtr = nullptr,
				QString* errorMessagePtr = nullptr) = 0;

	/**
		Remove the given tags from entities.
	*/
	virtual bool RemoveTags(
				const QByteArray& entityType,
				const QByteArrayList& entityIds,
				const QByteArrayList& tagIds,
				const imtbase::IOperationContext* operationContextPtr,
				EntityTagChanges* changesPtr = nullptr,
				QString* errorMessagePtr = nullptr) = 0;

	/**
		Replace the whole tag set of an entity.
	*/
	virtual bool SetTags(
				const QByteArray& entityType,
				const QByteArray& entityId,
				const QByteArrayList& tagIds,
				const imtbase::IOperationContext* operationContextPtr,
				EntityTagChanges* changesPtr = nullptr,
				QString* errorMessagePtr = nullptr) = 0;

	/**
		Remove all tags from entities.
	*/
	virtual bool ClearTags(
				const QByteArray& entityType,
				const QByteArrayList& entityIds,
				const imtbase::IOperationContext* operationContextPtr,
				EntityTagChanges* changesPtr = nullptr,
				QString* errorMessagePtr = nullptr) = 0;

	/**
		Get the active tags of entities visible in the tenant of the operation context.
		Assignments of deleted tags are not returned.
	*/
	virtual EntityTagIds GetTagIds(
				const QByteArray& entityType,
				const QByteArrayList& entityIds,
				const imtbase::IOperationContext* operationContextPtr) const = 0;

	/**
		Get the number of entities each tag is assigned to.
	*/
	virtual QMap<QByteArray, int> GetUsageCounts(const QByteArrayList& tagIds) const = 0;

	/**
		Check whether tags can be assigned to entities of the given type.
	*/
	virtual bool IsTaggableEntityType(const QByteArray& entityType) const = 0;

	/**
		Get the collection that stores entities of the given type.
	*/
	virtual const imtbase::IObjectCollection* GetEntityCollection(const QByteArray& entityType) const = 0;
};


} // namespace imttag


