// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QUrl>


namespace imtdoc
{


/**
	Returns the URL of the collection object \a objectId: \c collection:///<objectId>.
	If \a revision is not negative, the URL addresses this revision of the object:
	\c collection:///<objectId>/<revision>.
*/
QUrl ObjectIdToUrl(const QByteArray& objectId, int revision = -1);

/**
	Extracts the collection object ID from a \c collection:///<objectId> or
	\c collection:///<objectId>/<revision> URL.
	Returns an empty ID for unsaved documents (empty URL) or invalid URLs.
*/
QByteArray UrlToObjectId(const QUrl& url);

/**
	Extracts the object revision from a \c collection:///<objectId>/<revision> URL.
	Returns -1 if the URL does not address a revision or is invalid.
*/
int UrlToRevision(const QUrl& url);


} // namespace imtdoc


