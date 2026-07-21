// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QWaitCondition>

// stdlib
#include <functional>

// ImtCore includes
#include <imtclientgql/IAsyncGqlRequestToken.h>


namespace imtclientgql
{


/**
	Reusable reference-counted token for in-flight async GraphQL requests.
	Shared by CAsyncApiClientComp, CSubscriptionManagerComp, CWebSocketClientComp.

	Wait() uses QWaitCondition (not QEventLoop/QTimer). Nested QEventLoop on a
	CWorkerThread was a source of Qt affinity noise:
	  QObject: Cannot create children for a parent that is in a different thread
	  (Parent is QCoreApplication..., current thread is CWorkerThread...)
	and is unnecessary: completion is signaled from MarkTerminal on any thread.
*/
class CAsyncGqlRequestToken: virtual public IAsyncGqlRequestToken
{
public:
	CAsyncGqlRequestToken():
		m_state(RS_PENDING)
	{
	}

	virtual RequestState GetState() const override
	{
		QMutexLocker lock(&m_mutex);
		return m_state;
	}

	virtual void Cancel() override
	{
		std::function<void()> cancelCb;
		{
			QMutexLocker lock(&m_mutex);
			if (m_state != RS_PENDING){
				return;
			}
			cancelCb = m_cancelCb;
		}
		if (cancelCb){
			cancelCb();
		}
		else{
			MarkTerminal(RS_CANCELLED);
		}
	}

	virtual bool Wait(int timeoutMs = -1) override
	{
		QMutexLocker lock(&m_mutex);
		if (m_state != RS_PENDING){
			return true;
		}

		if (timeoutMs < 0){
			m_waitCondition.wait(&m_mutex);
			return true;
		}

		return m_waitCondition.wait(&m_mutex, static_cast<unsigned long>(timeoutMs));
	}

	void SetCancelCallback(std::function<void()> cancelCb)
	{
		QMutexLocker lock(&m_mutex);
		m_cancelCb = std::move(cancelCb);
	}

	void MarkCompleted()
	{
		MarkTerminal(RS_COMPLETED);
	}

	void MarkCancelled()
	{
		MarkTerminal(RS_CANCELLED);
	}

	void MarkFailed()
	{
		MarkTerminal(RS_FAILED);
	}

private:
	void MarkTerminal(RequestState newState)
	{
		QMutexLocker lock(&m_mutex);
		if (m_state != RS_PENDING){
			return;
		}
		m_state = newState;
		m_cancelCb = nullptr;
		m_waitCondition.wakeAll();
	}

	mutable QMutex m_mutex;
	QWaitCondition m_waitCondition;
	RequestState m_state;
	std::function<void()> m_cancelCb;
};


} // namespace imtclientgql
