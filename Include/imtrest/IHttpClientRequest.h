// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtNetwork/QNetworkReply>

// ACF includes
#include <istd/IChangeable.h>


namespace imtrest
{


/**
	Interface for a container that containing information about HTTP requests(is a unit of model).
*/
class IHttpClientRequest: virtual public istd::IChangeable
{
public:
	enum RequestState
	{
		RS_CREATED				= 0x0000, ///< Just created, and not ready to be proceeded
		RS_READY_TO_PROCESSING	= 0x0001, ///< ready to proceed (When all properties have been set)
		RS_PROCESSING			= 0x0002, ///< Processing of the request started
		RS_FINISHED				= 0x0004, ///< Processing of the request has been finished
		RS_NO_ERROR				= 0x0008, ///< Processing of the request has been finished with normal result
		RS_ERROR				= 0x0010 ///< Processing of the request has been finished with error
	};

	virtual QVariant GetAttribute(QNetworkRequest::Attribute code) const = 0;
	virtual QString GetErrorString() const = 0;
	virtual QNetworkReply::NetworkError GetErrorType() const = 0;
	virtual QByteArray GetHeader(const QByteArray &headerName) const = 0;
	virtual QList<QByteArray> GetHeaderList() const = 0;
	virtual QNetworkReply* GetReply() const = 0;
	virtual QByteArray GetReplyBody() const = 0;
	virtual QNetworkRequest GetRequest() const = 0;
	virtual QByteArray GetRequestBody() const = 0;
	virtual QNetworkAccessManager::Operation GetRequestType() const = 0;
	virtual int GetState() const = 0;
	virtual QUrl GetUrl() const = 0;

	virtual void SetReply(QNetworkReply* reply) = 0;
	virtual void SetRequest(const QNetworkRequest& request) = 0;
	virtual void SetRequestBody(const QByteArray& body) = 0;
	virtual void SetRequestType(QNetworkAccessManager::Operation requestType) = 0;
};


} // namespace imtrest


