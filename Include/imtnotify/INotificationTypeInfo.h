// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtnotify
{


/**
	Description of a notification type, or of a type of the objects of notifications: what a client shows for it.
	A notification carries only the id of its type; the server the client talks to gives the label in the language of
	the client.
*/
class INotificationTypeInfo: virtual public istd::IPolymorphic
{
public:
	virtual QByteArray GetTypeId() const = 0;

	/**
		The label in the source language; the server translates it for the client.
	*/
	virtual QString GetName() const = 0;

	/**
		Path of the icon of the type, empty for a type of objects.
	*/
	virtual QByteArray GetIconPath() const = 0;

	/**
		The message the client shows, in the source language (imtnotify::NotificationMessageFormat); the server
		translates it for the client. Empty: the client shows the subject with the type of its object.
	*/
	virtual QString GetMessage() const = 0;

	/**
		The arguments the message uses besides the standard ones (NotificationMessageFormat::s_standardArgumentIds).
	*/
	virtual QByteArrayList GetParameterIds() const = 0;
};


} // namespace imtnotify


