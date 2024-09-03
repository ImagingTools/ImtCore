#pragma once

// ImtCore includes
#include <imtrest/IRequestServlet.h>

// Qt includes
#include <QtCore/QObject>


namespace imtrest
{

class CWorkerManagerComp;
class CWorkerThread;


class CWorker: public QObject
{
	Q_OBJECT
public:
	CWorker(const imtrest::IRequestServlet* requestServletPtr, CWorkerThread* workerThread);

Q_SIGNALS:
	void FinishProcess(const IRequest* request, const QByteArray& subCommandId);

public Q_SLOTS:
	void ProcessRequest(const IRequest* request, const QByteArray& subCommandId);

private:
	const imtrest::IRequestServlet* m_requestServletPtr;

	CWorkerThread* m_workerThread;
};


} // namespace imtrest


