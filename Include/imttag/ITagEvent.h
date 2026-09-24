// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <istd/TInterfacePtr.h>
#include <iser/IObject.h>


namespace imttag
{


/**
	Immutable record of a tag being added to or removed from an entity.
	The tag name and color are snapshots taken when the event occurred,
	so the history stays readable after the tag is renamed or deleted.
	\ingroup imttag
*/
class ITagEvent: virtual public iser::IObject
{
public:
	enum Action
	{
		A_TAGGED,
		A_UNTAGGED
	};

	virtual Action GetAction() const = 0;
	virtual void SetAction(Action action) = 0;
	virtual QByteArray GetTagId() const = 0;
	virtual void SetTagId(const QByteArray& tagId) = 0;
	virtual QString GetTagName() const = 0;
	virtual void SetTagName(const QString& tagName) = 0;
	virtual QString GetTagColor() const = 0;
	virtual void SetTagColor(const QString& tagColor) = 0;
	virtual QByteArray GetEntityType() const = 0;
	virtual void SetEntityType(const QByteArray& entityType) = 0;
	virtual QByteArray GetEntityId() const = 0;
	virtual void SetEntityId(const QByteArray& entityId) = 0;

	/**
		Revision of the entity at the moment of the event, or -1 if the entity collection has no revisions.
	*/
	virtual qlonglong GetEntityRevision() const = 0;
	virtual void SetEntityRevision(qlonglong revision) = 0;
	virtual QByteArray GetActorId() const = 0;
	virtual void SetActorId(const QByteArray& actorId) = 0;
	virtual QString GetActorName() const = 0;
	virtual void SetActorName(const QString& actorName) = 0;
	virtual QByteArray GetTenantId() const = 0;
	virtual void SetTenantId(const QByteArray& tenantId) = 0;
	virtual QDateTime GetTimestamp() const = 0;
	virtual void SetTimestamp(const QDateTime& timestamp) = 0;
};


typedef istd::TUniqueInterfacePtr<ITagEvent> ITagEventUniquePtr;
typedef istd::TSharedInterfacePtr<ITagEvent> ITagEventSharedPtr;


} // namespace imttag


