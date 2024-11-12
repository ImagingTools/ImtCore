#pragma once


// ImtCore includes
#include <graphqlserver/graphqlserver.h>
#include <graphqlserver/CGqlContext.h>

// Qt includes
#include <QtCore/QSharedPointer>
#include <QtCore/QUrl>


namespace graphqlserver {


class CGqlRequest: virtual public IGqlRequest
{
public:
	void SetCommandId(const QByteArray& commandId);
	void SetRequestType(const RequestType& requestType);
	void SetQuery(const QByteArray& query);

	// reimplemented (graphqlserver::IHttpRequest)
	virtual QByteArray GetCommandId() const override;
	virtual RequestType GetRequestType() const override;
	virtual QByteArray GetQuery() const override;
	virtual const IGqlContext* GetRequestContext() const override;

private:
	QByteArray m_commandId;
	RequestType m_requestType;
	QByteArray m_query;
	CGqlContext m_gqlContext;
};

} //namespace graphqlserver
