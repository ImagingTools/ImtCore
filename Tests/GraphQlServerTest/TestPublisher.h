// ImtCore includes
#include <graphqlserver/graphqlserver.h>

// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QTimer>

#include <QtCore/QDateTime>
#include <QtCore/QJsonObject>


class TestPublisher: public QObject, virtual public graphqlserver::IGqlPublisher
{
	Q_OBJECT
public:
	TestPublisher();

protected:
	// reimplemented (graphqlserver::IGqlPublisher)
	virtual bool IsRequestSupported(const graphqlserver::IGqlRequest& gqlRequest) const override;
	virtual bool RegisterSubscription(
				const QByteArray& subscriptionId,
				const graphqlserver::IGqlRequest& gqlRequest,
				QString& errorMessage) override;
	virtual bool UnRegisterSubscription(const QByteArray& subscriptionId, QString& errorMessage) override;

protected Q_SLOTS:
	void slotTimer();
	// void slotTimer()
	// {
	// 	for (const QByteArray& subscriptionId: m_subscriptionIdList){
	// 		QJsonObject jsonData;
	// 		jsonData.insert("currentData", QDateTime::currentDateTime().toString());

	// 		graphqlserver::SendSubscription(subscriptionId, jsonData);
	// 	}
	// }
private:
	QList<QByteArray> m_subscriptionIdList;
	QTimer m_dateTimer;
};


