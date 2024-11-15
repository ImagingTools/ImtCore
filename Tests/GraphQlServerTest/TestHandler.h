// ImtCore includes
#include <graphqlserver/graphqlserver.h>

// Qt includes
#include <QtCore/QJsonObject>


class TestHandler: virtual public graphqlserver::IGqlRequestHandler
{
protected:
	virtual QJsonObject CreateResponse(
				const QByteArray& commandId,
				const graphqlserver::RequestType requestType,
				const QJsonObject& inputParams,
				const graphqlserver::ResultKeys& resultKeys,
				QString& errorMessage) const override;
};



