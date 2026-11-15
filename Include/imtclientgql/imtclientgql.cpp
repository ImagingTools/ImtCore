// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/imtclientgql.h>


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>


namespace imtclientgql
{


QString GetReplyErrorMessage(const QNetworkReply& reply, const QByteArray& body)
{
	const QJsonArray errors = QJsonDocument::fromJson(body).object().value(QStringLiteral("errors")).toArray();
	const QString message = errors.isEmpty() ? QString() : errors.at(0).toObject().value(QStringLiteral("message")).toString();
	if (!message.isEmpty()){
		return message;
	}

	const int status = reply.attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (status > 0){
		return QStringLiteral("The remote server refused the request: %1 %2").arg(QString::number(status), reply.attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString());
	}

	return reply.errorString();
}


} // namespace imtclientgql
