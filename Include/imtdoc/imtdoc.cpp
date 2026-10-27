// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/imtdoc.h>


// Qt includes
#include <QtCore/QStringList>


namespace imtdoc
{


// private methods

static bool ParseUrl(const QUrl& url, QByteArray& objectId, int& revision)
{
	objectId.clear();
	revision = -1;

	if (url.scheme() != QStringLiteral("collection")){
		return false;
	}

	const QStringList parts = url.path().split('/', Qt::SkipEmptyParts);
	if (parts.isEmpty() || parts.count() > 2){
		return false;
	}

	if (parts.count() == 2){
		bool isOk = false;
		const int parsedRevision = parts[1].toInt(&isOk);
		if (!isOk || parsedRevision < 0){
			return false;
		}

		revision = parsedRevision;
	}

	objectId = parts[0].toUtf8();

	return true;
}


// public methods

QUrl ObjectIdToUrl(const QByteArray& objectId, int revision)
{
	if (revision < 0){
		return QUrl(QStringLiteral("collection:///") + objectId);
	}

	return QUrl(QStringLiteral("collection:///") + objectId + '/' + QString::number(revision));
}


QByteArray UrlToObjectId(const QUrl& url)
{
	QByteArray objectId;
	int revision = -1;
	if (!ParseUrl(url, objectId, revision)){
		return QByteArray();
	}

	return objectId;
}


int UrlToRevision(const QUrl& url)
{
	QByteArray objectId;
	int revision = -1;
	if (!ParseUrl(url, objectId, revision)){
		return -1;
	}

	return revision;
}


} // namespace imtdoc


