// ImtCore includes
#include <graphqlserver/graphqlserver.h>

// Qt includes
#include <QtCore/QJsonObject>


class TestHandler: virtual public graphqlserver::IGqlRequestHandler
{
protected:
	virtual bool IsRequestSupported(const graphqlserver::IGqlRequest& gqlRequest) const override
	{
		return true;
	}

	virtual QJsonObject CreateResponse(const graphqlserver::IGqlRequest& gqlRequest, QString& errorMessage) const override
	{
		QJsonObject retVal;
		QJsonObject data;
		data.insert("message", "hellow world!");
		retVal.insert("data", data);

		return retVal;
	}
};



