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
	Assignment of a tag to an entity.
	The entity is addressed by its type (the ID of its collection) and its ID.
	\ingroup imttag
*/
class ITagAssignment: virtual public iser::IObject
{
public:
	virtual QByteArray GetTagId() const = 0;
	virtual void SetTagId(const QByteArray& tagId) = 0;
	virtual QByteArray GetEntityType() const = 0;
	virtual void SetEntityType(const QByteArray& entityType) = 0;
	virtual QByteArray GetEntityId() const = 0;
	virtual void SetEntityId(const QByteArray& entityId) = 0;
	virtual QDateTime GetCreatedAt() const = 0;
	virtual void SetCreatedAt(const QDateTime& createdAt) = 0;
	virtual QByteArray GetCreatedByUserId() const = 0;
	virtual void SetCreatedByUserId(const QByteArray& userId) = 0;
};


typedef istd::TUniqueInterfacePtr<ITagAssignment> ITagAssignmentUniquePtr;
typedef istd::TSharedInterfacePtr<ITagAssignment> ITagAssignmentSharedPtr;


} // namespace imttag


