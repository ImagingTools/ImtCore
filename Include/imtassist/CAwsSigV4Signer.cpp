// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CAwsSigV4Signer.h>


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QMap>
#include <QtCore/QMessageAuthenticationCode>
#include <QtCore/QUrlQuery>


namespace imtassist
{


namespace
{


QByteArray Hmac(const QByteArray& key, const QByteArray& message)
{
	return QMessageAuthenticationCode::hash(message, key, QCryptographicHash::Sha256);
}


QByteArray Sha256Hex(const QByteArray& data)
{
	return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}


QByteArray TrimHeaderValue(const QByteArray& value)
{
	return value.simplified();
}


} // namespace


void CAwsSigV4Signer::Sign(
			const QByteArray& method,
			const QUrl& url,
			Headers& headers,
			const QByteArray& payload,
			const QByteArray& region,
			const QByteArray& service,
			const Credentials& credentials,
			const QDateTime& timestamp)
{
	QDateTime utc = timestamp.toUTC();
	QByteArray amzDate = utc.toString("yyyyMMdd'T'HHmmss'Z'").toLatin1();
	QByteArray dateStamp = amzDate.left(8);

	QByteArray host = url.host().toLatin1();
	if (url.port() > 0){
		host += ":" + QByteArray::number(url.port());
	}

	headers.append(qMakePair(QByteArray("Host"), host));
	headers.append(qMakePair(QByteArray("X-Amz-Date"), amzDate));
	if (!credentials.sessionToken.isEmpty()){
		headers.append(qMakePair(QByteArray("X-Amz-Security-Token"), credentials.sessionToken));
	}

	QByteArray signedHeaders;
	QByteArray canonicalRequest = CreateCanonicalRequest(method, url, headers, payload, signedHeaders);

	QByteArray scope = dateStamp + "/" + region + "/" + service + "/aws4_request";
	QByteArray stringToSign = "AWS4-HMAC-SHA256\n" + amzDate + "\n" + scope + "\n" + Sha256Hex(canonicalRequest);

	QByteArray signingKey = Hmac("AWS4" + credentials.secretAccessKey, dateStamp);
	signingKey = Hmac(signingKey, region);
	signingKey = Hmac(signingKey, service);
	signingKey = Hmac(signingKey, "aws4_request");

	QByteArray signature = Hmac(signingKey, stringToSign).toHex();

	QByteArray authorization =
				"AWS4-HMAC-SHA256 Credential=" + credentials.accessKeyId + "/" + scope +
				", SignedHeaders=" + signedHeaders +
				", Signature=" + signature;

	headers.append(qMakePair(QByteArray("Authorization"), authorization));
}


QByteArray CAwsSigV4Signer::CreateCanonicalRequest(
			const QByteArray& method,
			const QUrl& url,
			const Headers& headers,
			const QByteArray& payload,
			QByteArray& signedHeaders)
{
	// Services other than S3 encode the (already encoded) path once more.
	QByteArray path = url.path(QUrl::FullyEncoded).toLatin1();
	if (path.isEmpty()){
		path = "/";
	}

	QByteArray canonicalUri = UriEncode(path, false);

	QMap<QByteArray, QByteArray> queryItems;
	QUrlQuery query(url);
	for (const QPair<QString, QString>& item: query.queryItems(QUrl::FullyDecoded)){
		queryItems.insert(UriEncode(item.first.toUtf8(), true), UriEncode(item.second.toUtf8(), true));
	}

	QByteArrayList queryParts;
	for (QMap<QByteArray, QByteArray>::const_iterator iter = queryItems.constBegin(); iter != queryItems.constEnd(); ++iter){
		queryParts.append(iter.key() + "=" + iter.value());
	}

	QMap<QByteArray, QByteArray> canonicalHeaders;
	for (const QPair<QByteArray, QByteArray>& header: headers){
		QByteArray name = header.first.toLower().trimmed();
		if (canonicalHeaders.contains(name)){
			canonicalHeaders[name] += "," + TrimHeaderValue(header.second);
		}
		else{
			canonicalHeaders.insert(name, TrimHeaderValue(header.second));
		}
	}

	QByteArray headerBlock;
	QByteArrayList headerNames;
	for (QMap<QByteArray, QByteArray>::const_iterator iter = canonicalHeaders.constBegin(); iter != canonicalHeaders.constEnd(); ++iter){
		headerBlock += iter.key() + ":" + iter.value() + "\n";
		headerNames.append(iter.key());
	}

	signedHeaders = headerNames.join(';');

	return method + "\n" +
				canonicalUri + "\n" +
				queryParts.join('&') + "\n" +
				headerBlock + "\n" +
				signedHeaders + "\n" +
				Sha256Hex(payload);
}


QByteArray CAwsSigV4Signer::UriEncode(const QByteArray& value, bool encodeSlash)
{
	QByteArray retVal;
	for (char character: value){
		bool isUnreserved =
					((character >= 'a') && (character <= 'z')) ||
					((character >= 'A') && (character <= 'Z')) ||
					((character >= '0') && (character <= '9')) ||
					(character == '-') || (character == '_') || (character == '.') || (character == '~');
		if (isUnreserved || ((character == '/') && !encodeSlash)){
			retVal.append(character);
		}
		else{
			retVal.append('%');
			retVal.append(QByteArray(1, character).toHex().toUpper());
		}
	}

	return retVal;
}


} // namespace imtassist


