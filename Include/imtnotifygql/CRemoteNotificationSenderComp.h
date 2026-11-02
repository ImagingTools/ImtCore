// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <imtnotify/INotificationSender.h>


namespace imtnotifygql
{


/**
	Sender of a product server: hands the notifications to the notification center of the authorization server.
	It sends with the session of the user whose request made the change; a change made outside of a user request is not
	sent. A product server has no identity of its own yet (ImagingTools/ImtCore#1059).
	Sending is queued: Send and Withdraw return at once and an own thread delivers the requests one by one in their order.
	A request the authorization server did not answer is repeated a few times, then dropped; a refused one is dropped
	at once. A notification without a dedup key gets one, so a repeated request does not deliver it twice.
*/
class CRemoteNotificationSenderComp:
			public imtclientgql::TClientRequestManagerCompWrap<ilog::CLoggerComponentBase>,
			virtual public imtnotify::INotificationSender
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<ilog::CLoggerComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteNotificationSenderComp);
		I_REGISTER_INTERFACE(imtnotify::INotificationSender);
	I_END_COMPONENT;

	// reimplemented (imtnotify::INotificationSender)
	virtual bool Send(const Audience& audience, const Message& message) override;
	virtual bool Withdraw(const Audience& audience, const QByteArray& groupKey) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	struct Request
	{
		bool isWithdraw = false;
		Audience audience;
		Message message;

		// The session of the user whose request made the change
		QByteArray accessToken;
	};

	enum DeliveryResult
	{
		DR_DELIVERED,
		DR_NOT_ANSWERED,
		DR_REFUSED
	};

	bool Enqueue(const Request& request);
	void RunQueue();
	DeliveryResult Deliver(const Request& request) const;

	/**
		Take the session of the current user request; false outside of one.
	*/
	bool TakeCallerSession(Request& request) const;

private:
	QMutex m_queueMutex;
	QWaitCondition m_queueCondition;
	QList<Request> m_queue;
	bool m_isStopping = false;

	QThread* m_queueThreadPtr = nullptr;
};


} // namespace imtnotifygql


