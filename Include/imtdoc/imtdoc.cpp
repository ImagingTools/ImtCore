// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/imtdoc.h>


// Qt includes
#include <QtCore/QStringList>


namespace imtdoc
{


// public methods

QUrl ObjectIdToUrl(const QByteArray& objectId)
{
	return QUrl(QStringLiteral("collection:///") + objectId);
}


QByteArray UrlToObjectId(const QUrl& url)
{
	if (url.scheme() != QStringLiteral("collection")){
		return QByteArray();
	}

	const QStringList parts = url.path().split('/', Qt::SkipEmptyParts);
	if (parts.count() != 1){
		return QByteArray();
	}

	return parts.first().toUtf8();
}


} // namespace imtdoc


