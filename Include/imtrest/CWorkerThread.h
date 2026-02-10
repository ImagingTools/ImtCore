// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QThread>

// ACF includes
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/ISender.h>
#include <imtrest/CWorker.h>


namespace imtrest
{

class CWorkerManagerComp;


class CWorkerThread: public QThread
{
	Q_OBJECT
public:
	CWorkerThread(const CWorkerManagerComp* workerManager, const QByteArray& subCommandId);

	enum Status
	{
		ST_PROCESS,
		ST_CLOSE
	};

	Status GetStatus();
	void SetStatus(Status status);
	void SetRequestPtr(const IRequest* requestPtr);
	const ISender* GetSender(const QByteArray& requestId);

	//reimplemented (QThread)
	virtual void run() override;

Q_SIGNALS:
	void StartProcess(const IRequest* request, const QByteArray& subCommandId);
	void FinishProcess(const IRequest* request, const QByteArray& subCommandId);

protected Q_SLOTS:
	void OnStarted();
	void OnFinishProcess(const IRequest* request, const QByteArray& subCommandId);

private:
	Status m_status;
	mutable CWorkerManagerComp* m_workerManager;
	istd::TDelPtr<CWorker> m_workerPtr;
	const IRequest* m_requestPtr;
	mutable QMutex m_statusMutex;
	QByteArray m_subCommandId;
};


} // namespace imtrest


