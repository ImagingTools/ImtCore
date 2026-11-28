// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QPointer>

// ImtCore includes
#include <imtrest/IHttpClientRequest.h>


namespace imtrest
{


/**
	Outgoing HTTP request and the data of its reply.

	When a finished reply is set, its status, headers, error and body are copied into the
	request, so the request keeps them after the reply object has been deleted.
	\see CHttpClientRequestSender
*/
class CHttpClientRequest: virtual public IHttpClientRequest
{
public:
	CHttpClientRequest();

	/**
		Set the state, e.g. RS_FINISHED | RS_ERROR for a request that ended without reply.
	*/
	void SetState(int state);

	/**
		Set the error of a request that ended without a finished reply, e.g. a timeout.
	*/
	void SetError(QNetworkReply::NetworkError errorType, const QString& errorString);

	// reimplemented (imtrest::IHttpClientRequest)
	virtual QVariant GetAttribute(QNetworkRequest::Attribute code) const override;
	virtual QString GetErrorString() const override;
	virtual QNetworkReply::NetworkError GetErrorType() const override;
	virtual QByteArray GetHeader(const QByteArray& headerName) const override;
	virtual QList<QByteArray> GetHeaderList() const override;
	virtual QNetworkReply* GetReply() const override;
	virtual QByteArray GetReplyBody() const override;
	virtual QNetworkRequest GetRequest() const override;
	virtual QByteArray GetRequestBody() const override;
	virtual QNetworkAccessManager::Operation GetRequestType() const override;
	virtual int GetState() const override;
	virtual QUrl GetUrl() const override;
	virtual void SetReply(QNetworkReply* reply) override;
	virtual void SetRequest(const QNetworkRequest& request) override;
	virtual void SetRequestBody(const QByteArray& body) override;
	virtual void SetRequestType(QNetworkAccessManager::Operation requestType) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	void UpdateReadyState();

private:
	QNetworkRequest m_request;
	QByteArray m_requestBody;
	QNetworkAccessManager::Operation m_requestType;
	int m_state;
	QPointer<QNetworkReply> m_replyPtr;
	QByteArray m_replyBody;
	QMap<QNetworkRequest::Attribute, QVariant> m_replyAttributes;
	QList<QByteArray> m_replyHeaderNames;
	QMap<QByteArray, QByteArray> m_replyHeaders;
	QNetworkReply::NetworkError m_errorType;
	QString m_errorString;
};


} // namespace imtrest


