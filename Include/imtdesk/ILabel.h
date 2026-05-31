// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtdesk
{


/**
	Interface representing a label (like GitHub Labels).

	Labels have a name, hex color, and optional description. They can be
	assigned to tickets for categorization and filtering.

	\ingroup imtdesk
*/
class ILabel: virtual public iser::ISerializable
{
public:
	/**
		Get the unique label identifier.
	*/
	virtual QByteArray GetId() const = 0;

	/**
		Set the unique label identifier.
	*/
	virtual void SetId(const QByteArray& id) = 0;

	/**
		Get the label display name.
	*/
	virtual QString GetName() const = 0;

	/**
		Set the label display name.
	*/
	virtual void SetName(const QString& name) = 0;

	/**
		Get the hex color string (e.g. "ff0000" for red).
	*/
	virtual QString GetColor() const = 0;

	/**
		Set the hex color string.
	*/
	virtual void SetColor(const QString& color) = 0;

	/**
		Get the label description.
	*/
	virtual QString GetDescription() const = 0;

	/**
		Set the label description.
	*/
	virtual void SetDescription(const QString& description) = 0;
};

typedef istd::TUniqueInterfacePtr<ILabel> ILabelUniquePtr;


} // namespace imtdesk


