// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QUrl>


namespace imtdoc
{


/**
	Returns the \c collection:///<objectId> URL of the collection object \a objectId.
*/
QUrl ObjectIdToUrl(const QByteArray& objectId);

/**
	Extracts the collection object ID from a \c collection:///<objectId> URL.
	Returns an empty ID for unsaved documents (empty URL) or non-collection URLs.
*/
QByteArray UrlToObjectId(const QUrl& url);


} // namespace imtdoc


