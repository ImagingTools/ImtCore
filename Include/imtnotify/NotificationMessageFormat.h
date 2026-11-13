// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QSet>
#include <QtCore/QString>


namespace imtnotify
{


/**
	The messages of the notification types: the subset of ICU MessageFormat the clients show
	(Qml/imtnotifygui/NotificationMessageFormat.js renders it, this checks it on the server).

	- ids and branch names are ASCII letters, digits and '_';
	- {id} is the value of the argument id: the standard ones (s_standardArgumentIds) or those the type declares;
	- {id, plural, one {...} few {...} many {...} other {...}} chooses by the number of id in the language of the
	  client; =N matches the number exactly and # in a branch is the number;
	- {id, select, value {...} other {...}} chooses by the text of id;
	- '' is an apostrophe, '{', '}' and '#' after an apostrophe start a quoted text up to the next apostrophe.
	Every plural and select needs the branch "other".
*/
namespace NotificationMessageFormat
{


/**
	The arguments of every notification: the name of the subject, the name of the newest actor, the names of the
	latest actors of a group and the number of events of a group.
*/
static const inline QSet<QByteArray> s_standardArgumentIds = {"subject", "actor", "actors", "count"};


/**
	Check the syntax of a message and collect the ids of the arguments it uses.
	\return false with the reason if the message is not valid.
*/
bool GetArgumentIds(const QString& message, QSet<QByteArray>& argumentIds, QString& errorMessage);


} // namespace NotificationMessageFormat


} // namespace imtnotify


