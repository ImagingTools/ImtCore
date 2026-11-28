// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtCore/QUrl>


namespace imtassist
{


/**
	AWS Signature Version 4 for requests to AWS services other than S3.

	\ingroup imtassist
*/
class CAwsSigV4Signer
{
public:
	typedef QList<QPair<QByteArray, QByteArray>> Headers;

	struct Credentials
	{
		QByteArray accessKeyId;
		QByteArray secretAccessKey;
		QByteArray sessionToken;
	};

	/**
		Sign a request. Adds the headers "Host", "X-Amz-Date", "X-Amz-Security-Token" (with a
		session token) and "Authorization". All given headers are signed.
		\param url      URL whose path is already percent-encoded once.
	*/
	static void Sign(
				const QByteArray& method,
				const QUrl& url,
				Headers& headers,
				const QByteArray& payload,
				const QByteArray& region,
				const QByteArray& service,
				const Credentials& credentials,
				const QDateTime& timestamp);

	/**
		Create the canonical request (exposed for tests).
	*/
	static QByteArray CreateCanonicalRequest(
				const QByteArray& method,
				const QUrl& url,
				const Headers& headers,
				const QByteArray& payload,
				QByteArray& signedHeaders);

	/**
		Percent-encode as required by Signature Version 4 (RFC 3986 unreserved characters stay).
	*/
	static QByteArray UriEncode(const QByteArray& value, bool encodeSlash);
};


} // namespace imtassist


