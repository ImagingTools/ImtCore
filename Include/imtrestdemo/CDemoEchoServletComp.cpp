// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CDemoEchoServletComp.h>


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtrestdemo/CDemoProtocol.h>
#include <imtrestdemo/DemoServletTools.h>


namespace imtrestdemo
{


// protected methods

// reimplemented (imtrest::CHttpServletCompBase)

imtrest::ConstResponsePtr CDemoEchoServletComp::OnRequestReceived(const imtrest::CHttpRequest& request) const
{
	QJsonObject headersObject;
	const QByteArrayList headerNames = request.GetHeaders();
	for (const QByteArray& name : headerNames){
		headersObject.insert(QString::fromLatin1(name), QString::fromLatin1(request.GetHeaderValue(name)));
	}

	QJsonObject queryObject;
	const imtrest::IRequest::CommandParams params = request.GetCommandParams();
	for (imtrest::IRequest::CommandParams::ConstIterator iter = params.constBegin(); iter != params.constEnd(); ++iter){
		queryObject.insert(QString::fromUtf8(iter.key()), QString::fromUtf8(iter.value()));
	}

	const QByteArray body = request.GetBody();

	QJsonObject result;
	result.insert("method", QString::fromLatin1(DemoServletTools::GetMethodName(request.GetMethodType())));
	result.insert("path", QString::fromUtf8(request.GetCommandId()));
	result.insert("url", request.GetUrl().toString(QUrl::FullyEncoded));
	result.insert("query", queryObject);
	result.insert("headers", headersObject);
	result.insert("bodySize", double(body.size()));
	result.insert("bodySha256", QString::fromLatin1(QCryptographicHash::hash(body, QCryptographicHash::Sha256).toHex()));

	if (body.size() <= DemoProtocol::s_echoMaxBodySize){
		result.insert("body", QString::fromUtf8(body));
	}

	return DemoServletTools::CreateJsonResponse(request.GetProtocolEngine(), imtrest::IProtocolEngine::SC_OK, result);
}


} // namespace imtrestdemo
