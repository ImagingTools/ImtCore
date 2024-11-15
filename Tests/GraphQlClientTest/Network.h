#include <graphqlclient/graphqlclient.h>

// Qt includes
#include <QtCore/QDebug>


class Network: public graphqlclient::GraphQlRequestManager
{
public:
	Network(const graphqlclient::ConnectionSettings& connectionSettings):
				graphqlclient::GraphQlRequestManager(connectionSettings){};

protected:
	virtual void OnResponse(const QByteArray& data) override
	{
			qDebug() << "OnResponse data: " << data;
	}
};


