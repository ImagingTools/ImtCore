// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtrest/IRequestServlet.h>

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QMutex>


namespace imtrest
{

class CWorkerManagerComp;
class CWorkerThread;


class CWorker: public QObject
{
	Q_OBJECT
public:
	CWorker(imtrest::IRequestServletPtr&& requestServletPtr, CWorkerThread* workerThread);

Q_SIGNALS:
	void FinishProcess(const IRequest* request, const QByteArray& subCommandId);

public Q_SLOTS:
	void ProcessRequest(const IRequest* request, const QByteArray& subCommandId);

private:
	IRequestServletPtr m_requestServletPtr;
	mutable QMutex m_processMutex;

	CWorkerThread* m_workerThread;
};


} // namespace imtrest


