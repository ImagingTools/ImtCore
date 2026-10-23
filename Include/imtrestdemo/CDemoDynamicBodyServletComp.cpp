// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CDemoDynamicBodyServletComp.h>


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtrest/CHttpTemporaryFileBodySink.h>
#include <imtrestdemo/DemoServletTools.h>


namespace imtrestdemo
{


// protected methods

// reimplemented (imtrest::CHttpServletCompBase)

imtrest::ConstResponsePtr CDemoDynamicBodyServletComp::OnRequestReceived(const imtrest::CHttpRequest& request) const
{
	typedef imtrest::IProtocolEngine Engine;

	QJsonObject result;
	result.insert("method", QString::fromLatin1(DemoServletTools::GetMethodName(request.GetMethodType())));
	result.insert("path", QString::fromUtf8(request.GetCommandId()));

	const std::shared_ptr<imtrest::IHttpRequestBodySink> sinkPtr = request.GetBodySink();
	if (!sinkPtr){
		// The policy has chosen the memory: the body is in the request
		const QByteArray body = request.GetBody();

		result.insert("storage", "memory");
		result.insert("size", double(body.size()));
		result.insert("sha256", QString::fromLatin1(QCryptographicHash::hash(body, QCryptographicHash::Sha256).toHex()));

		return DemoServletTools::CreateJsonResponse(request.GetProtocolEngine(), Engine::SC_OK, result);
	}

	// The policy has chosen a file: the request keeps the receiver, and the receiver deletes the file when the request is destroyed
	const std::shared_ptr<imtrest::CHttpTemporaryFileBodySink> fileSinkPtr = std::dynamic_pointer_cast<imtrest::CHttpTemporaryFileBodySink>(sinkPtr);
	if (!fileSinkPtr){
		return DemoServletTools::CreateTextResponse(request.GetProtocolEngine(), Engine::SC_INTERNAL_SERVER_ERROR, "The path is not configured for the dynamic policy of the body");
	}

	QFile file(fileSinkPtr->GetFilePath());
	if (!file.open(QIODevice::ReadOnly)){
		return DemoServletTools::CreateTextResponse(request.GetProtocolEngine(), Engine::SC_INTERNAL_SERVER_ERROR, "The file of the body cannot be read");
	}

	// The hash is calculated by the parts, the body is never in the memory as a whole
	QCryptographicHash hash(QCryptographicHash::Sha256);
	hash.addData(&file);

	result.insert("storage", "file");
	result.insert("size", double(fileSinkPtr->GetReceivedSize()));
	result.insert("sha256", QString::fromLatin1(hash.result().toHex()));
	result.insert("fileName", QFileInfo(fileSinkPtr->GetFilePath()).fileName());

	return DemoServletTools::CreateJsonResponse(request.GetProtocolEngine(), Engine::SC_OK, result);
}


} // namespace imtrestdemo
