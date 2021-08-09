#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

// ACF includes
#include <istd/IChangeable.h>
#include <istd/CChangeNotifier.h>


namespace imtrest
{


/// The IHttpClientRequest class is a container that containing information about http requests (is an unit of model).
class IHttpClientRequest: virtual public istd::IChangeable
{
public:
	enum RequestState
	{
		RS_CREATED				= 0x0000, ///< Just created, and not ready to be proceed
		RS_READY_TO_PROCESSING	= 0x0001, ///< ready to proceed (When all properties has been set)
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


