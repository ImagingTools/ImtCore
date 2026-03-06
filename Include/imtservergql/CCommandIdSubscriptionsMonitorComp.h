#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QByteArrayList>
#include <QtCore/QMap>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtservergql/CGqlPublisherCompBase.h>
#include <imtservergql/ICommandIdSubscriptionsMonitor.h>


namespace imtservergql
{


class CCommandIdSubscriptionsMonitorComp : public QObject,
										 public ilog::CLoggerComponentBase,
										 virtual public imtservergql::ICommandIdSubscriptionsMonitor
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCommandIdSubscriptionsMonitorComp)
		I_REGISTER_INTERFACE(imtservergql::ICommandIdSubscriptionsMonitor);
	I_END_COMPONENT;

	// reimplemented (ICommandIdSubscriptionsMonitor)
	void SubscriptionAdded(
		const QByteArray& subscriptionId, const QByteArray& commandId, const QByteArray& subscribedInputId) override;
	void SubscriptionRemoved(const QByteArray& subscriptionId) override;

protected:
	virtual void SendNotification(int id, const QByteArray& changeNotifier, QVariant changeInfo);

private:
	/*
	{
		"commandId": {
			"subscribedInputId": [
				"subscriptionId"
			]
		},
		"OnSessionStateChanged": {
			"S:aa5d1bde-1932-426c-b3ec-dd0cbd17e1af#D:X-Rite i1Pro 2 1004146#A:Agent_GMGA-JakeDell": [
				"ad62771e-8a77-4f95-85e3-af64b0127489"
			]
		}
	}
	*/
	QMap<QByteArray /*commandId*/, QMap<QByteArray /*subscribedInputId*/, QByteArrayList /*subscribedClients (List of subscriptionId)*/>>
		m_subscribers;
};


} // namespace imtservergql
