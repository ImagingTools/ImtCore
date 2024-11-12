#pragma once


// ImtCore includes
#include <graphqlserver/graphqlserver.h>

// Qt includes
#include <QtCore/QSharedPointer>
#include <QtCore/QUrl>


namespace graphqlserver {


class CHttpRequest: virtual public IHttpRequest
{
public:
	typedef QMap<QByteArray,QByteArray> Headers;

	void SetHeaders(const Headers& headers);
	void SetUrl(const QUrl& url);
	void SetMethodType(const MethodType& methodType);
	void SetCommandId(const QByteArray& commandId);
	void SetCommandParams(const CommandParams& commandParams);
	void SetBody(const QByteArray& body);
	void SetRequestId(const QByteArray& requestId);

	// reimplemented (graphqlserver::IHttpRequest)
	virtual QByteArrayList GetHeaders() const override;
	virtual QByteArray GetHeaderValue(const QByteArray& headerType) const override;
	virtual QUrl GetUrl() const override;
	virtual MethodType GetMethodType() const override;
	// virtual QHostAddress GetRemoteAddress() const override;

	virtual QByteArray GetCommandId() const override;
	virtual CommandParams GetCommandParams() const override;
	virtual QByteArray GetBody() const override;
	virtual QByteArray GetRequestId() const override;

private:
	Headers m_headers;
	QUrl m_url;
	MethodType m_methodType;
	QByteArray m_commandId;
	CommandParams m_commandParams;
	QByteArray m_body;
	QByteArray m_requestId;
};

} //namespace graphqlserver
