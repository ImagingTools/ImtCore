#pragma once

// ImtCore includes


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QMutex>


namespace imthttp
{

class CWorkerManagerComp;
class CWorkerThread;


class CWorker: public QObject
{
	Q_OBJECT
public:
	CWorker(imthttp::IRequestServletPtr&& requestServletPtr, CWorkerThread* workerThread);

Q_SIGNALS:
	void FinishProcess(const IRequest* request, const QByteArray& subCommandId);

public Q_SLOTS:
	void ProcessRequest(const IRequest* request, const QByteArray& subCommandId);

private:
	IRequestServletPtr m_requestServletPtr;
	mutable QMutex m_processMutex;

	CWorkerThread* m_workerThread;
};


} // namespace imthttp


