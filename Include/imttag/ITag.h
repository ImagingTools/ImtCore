// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/TInterfacePtr.h>
#include <iser/ISerializable.h>


namespace imttag
{


/**
	Interface representing a tag (like GitHub Labels).
	Tags have a name, hex color and optional description. They can be
	assigned to any taggable entity for categorization and filtering.
	\ingroup imttag
*/
class ITag: virtual public iser::ISerializable
{
public:
	/**
		Get the unique tag identifier.
	*/
	virtual QByteArray GetId() const = 0;

	/**
		Set the unique tag identifier.
	*/
	virtual void SetId(const QByteArray& id) = 0;

	/**
		Get the tag display name.
	*/
	virtual QString GetName() const = 0;

	/**
		Set the tag display name.
	*/
	virtual void SetName(const QString& name) = 0;

	/**
		Get the hex color string without the leading '#' (e.g. "d73a4a").
	*/
	virtual QString GetColor() const = 0;

	/**
		Set the hex color string.
	*/
	virtual void SetColor(const QString& color) = 0;

	/**
		Get the tag description.
	*/
	virtual QString GetDescription() const = 0;

	/**
		Set the tag description.
	*/
	virtual void SetDescription(const QString& description) = 0;

	/**
		Check whether the tag is a system tag.
		System tags are visible to every tenant and can be changed by the superuser only.
	*/
	virtual bool IsSystem() const = 0;

	/**
		Set the system flag.
	*/
	virtual void SetSystem(bool isSystem) = 0;
};


typedef istd::TUniqueInterfacePtr<ITag> ITagUniquePtr;
typedef istd::TSharedInterfacePtr<ITag> ITagSharedPtr;


} // namespace imttag


