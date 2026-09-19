// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CServletRequestDispatcher.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QMetaObject>
#include <QtCore/QThread>


namespace imtrest
{


CServletRequestDispatcher::CServletRequestDispatcher(QObject& requestOwner)
	:m_taskQueuePtr(nullptr),
	m_requestOwnerPtr(&requestOwner),
	m_responseDispatcherPtr(nullptr)
{
}


void CServletRequestDispatcher::SetTaskQueue(IWorkerTaskQueue* taskQueuePtr)
{
	m_taskQueuePtr = taskQueuePtr;
}


void CServletRequestDispatcher::SetResponseDispatcher(IResponseDispatcher* responseDispatcherPtr)
{
	m_responseDispatcherPtr = responseDispatcherPtr;
}


bool CServletRequestDispatcher::ProcessRequest(const IRequest& request, const QByteArray& subCommandId)
{
	OwnedRequestPtr ownedRequestPtr = MakeOwnedRequest(request);

	if (m_taskQueuePtr == nullptr){
		Q_ASSERT_X(false, "CServletRequestDispatcher", "No worker pool was set");

		return false;
	}

	// An empty ordering key: requests carry no ordering constraint of their own. A transport
	// that needs its requests serialised - per connection, per subscription - is what a
	// non-empty key here would express.
	const bool isPosted = m_taskQueuePtr->PostTask(
				QByteArray(),
				[this, ownedRequestPtr, subCommandId](){
					auto* servletPtr = dynamic_cast<IRequestServlet*>(m_taskQueuePtr->GetWorkerContext());
					if (servletPtr == nullptr){
						Q_ASSERT_X(false, "CServletRequestDispatcher", "Worker context is not a request servlet");

						return;
					}

					QByteArray body = ownedRequestPtr->GetBody();
					if (body.size() > 100){
						body.resize(100);
						body += "...";
					}

					qDebug() << "Start process " << ownedRequestPtr->GetCommandId() << body;

					ConstResponsePtr responsePtr = servletPtr->ProcessRequest(*ownedRequestPtr, subCommandId);
					if (!responsePtr.IsValid()){
						Q_ASSERT_X(false, __FILE__, "Request result invalid");

						return;
					}

					if (m_responseDispatcherPtr != nullptr){
						m_responseDispatcherPtr->SendResponse(ownedRequestPtr->GetRequestId(), responsePtr);
					}
				});

	// When the pool refuses the task, the closure is never created and this scope holds the
	// last reference, so the request is released here.
	return isPosted;
}


// private methods

CServletRequestDispatcher::OwnedRequestPtr CServletRequestDispatcher::MakeOwnedRequest(const IRequest& request) const
{
	QPointer<QObject> requestOwnerPtr = m_requestOwnerPtr;

	return OwnedRequestPtr(
				&request,
				[requestOwnerPtr](const IRequest* requestPtr){
					QObject* ownerPtr = requestOwnerPtr.data();
					if ((ownerPtr == nullptr) || (QThread::currentThread() == ownerPtr->thread())){
						delete requestPtr;

						return;
					}

					// ~IRequest may notify subscription bookkeeping under a publisher's own
					// mutex; it keeps the thread it has always run on.
					QMetaObject::invokeMethod(
								ownerPtr,
								[requestPtr](){
									delete requestPtr;
								},
								Qt::QueuedConnection);
				});
}


} // namespace imtrest
