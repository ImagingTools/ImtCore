#pragma once


// ImtCore includes
#include <graphqlserver/graphqlserver.h>

// Qt includes
#include <QtCore/QSharedPointer>
#include <QtCore/QUrl>


namespace graphqlserver {


class CGqlContext: virtual public graphqlserver::IGqlContext
{
public:

	// reimplemented (graphqlserver::IHttpRequest)
	virtual QByteArray GetProductId() const override;
	virtual void SetProductId(const QByteArray& productId) override;

	virtual QByteArray GetLanguageId() const override;
	virtual void SetLanguageId(const QByteArray& languageId) override;

	virtual QByteArray GetDesignScheme() const override;
	virtual void SetDesignScheme(const QByteArray& designScheme) override;

	virtual QByteArray GetToken() const override;
	virtual void SetToken(const QByteArray& token) override;

	virtual Headers GetHeaders() const override;
	virtual void SetHeaders(const Headers& headers) override;

private:
	QByteArray m_productId;
	QByteArray m_languageId;
	QByteArray m_designScheme;
	QByteArray m_token;
	Headers m_headers;
};

} //namespace graphqlserver
