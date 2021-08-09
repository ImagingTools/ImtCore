#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

// ACF includes
#include <istd/IChangeable.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtrest/IHttpClientRequest.h>


namespace imtrest
{


/**
	\brief The CHttpClientRequest class is a reimplementation of \class IHttpClientRequest
	\sa IHttpClientRequest
*/
class CHttpClientRequest: public IHttpClientRequest
{
public:
	CHttpClientRequest();
	CHttpClientRequest(const QNetworkRequest& request, QNetworkAccessManager::Operation requestType);

	// reimplemented (IHttpClientRequest)
	virtual QVariant GetAttribute(QNetworkRequest::Attribute code) const override;
	virtual QString GetErrorString() const override;
	virtual QNetworkReply::NetworkError GetErrorType() const override;
	virtual QByteArray GetHeader(const QByteArray& headerName) const override;
	virtual QList<QByteArray> GetHeaderList() const override;
	QNetworkReply* GetReply() const override;
	virtual QByteArray GetReplyBody() const override;
	virtual QNetworkRequest GetRequest() const override;
	virtual QByteArray GetRequestBody() const override;
	virtual QNetworkAccessManager::Operation GetRequestType() const override;
	int GetState() const override;
	virtual QUrl GetUrl() const override;

	virtual void SetReply(QNetworkReply* reply) override;
	virtual void SetRequest(const QNetworkRequest& request) override;
	virtual void SetRequestBody(const QByteArray& body) override;
	virtual void SetRequestType(QNetworkAccessManager::Operation requestType) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	bool CopyFrom(const IChangeable& object, CompatibilityMode mode) override;

private:
	bool CheckForReady();
	void OnReplyFinished();

private:
	int m_state;
	QNetworkReply* m_networkReply;
	QNetworkRequest *m_networkRequest;
	QByteArray m_requestBody;
	QNetworkAccessManager::Operation m_requestType;
};


} // namespace imtrest


