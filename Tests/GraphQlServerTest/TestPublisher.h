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
	virtual bool RegisterSubscription(
				const QByteArray& subscriptionId,
				const graphqlserver::ResultKeys& resultKeys,
				QString& errorMessage) override;
	virtual bool UnRegisterSubscription(const QByteArray& subscriptionId, QString& errorMessage) override;

protected Q_SLOTS:
	void slotTimer();

Q_SIGNALS:
	void signalStartTimer();
	void signalStopTimer();

private:
	QList<QByteArray> m_subscriptionIdList;
	QTimer m_dateTimer;
};


