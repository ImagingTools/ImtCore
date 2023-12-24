#pragma once


// Qt includes
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

// ACF includes
#include <istd/TPointerVector.h>
#include <ibase/TRuntimeStatusHanderCompWrap.h>
#include <ilog/TLoggerCompWrap.h>
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtbase/IUrlParam.h>
#include <imtrest/IRequest.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/IRequestManager.h>


namespace imtrest
{


class CMultiThreadServer;


/**
	TCP-based communication server.
	The server uses the underlaying protocol engine for creation of requests and responses.
*/
class CTcpServerComp:
			public QObject,
			public ibase::TRuntimeStatusHanderCompWrap<ilog::CLoggerComponentBase>,
			virtual public IRequestManager
{
	Q_OBJECT
public:
	typedef ibase::TRuntimeStatusHanderCompWrap<ilog::CLoggerComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CTcpServerComp);
		I_REGISTER_INTERFACE(IRequestManager)
		I_ASSIGN(m_requestHandlerCompPtr, "RequestHandler", "Request handler registered for the server", true, "RequestHandler");
		I_ASSIGN(m_protocolEngineCompPtr, "ProtocolEngine", "Protocol engine used in the server", true, "ProtocolEngine");
		I_ASSIGN(m_threadsLimitAttrPtr, "ThreadsLimit", "Limit of threads", true, 5);
		I_ASSIGN(m_startServerOnCreateAttrPtr, "StartServerOnCreate", "If enabled, the server will be started on after component creation", true, true);
		I_ASSIGN(m_serverPortCompPtr, "ServerPort", "Parameter providing the server port to be listened", true, "ServerPort");
	I_END_COMPONENT

	CTcpServerComp();
	~CTcpServerComp();

	imtrest::IRequestServlet* GetRequestServlet();
	imtrest::IProtocolEngine* GetProtocolEngine();
	int GetThreadsLimit();

protected:
	// reimplemented (ibase::TRuntimeStatusHanderCompWrap)
	virtual void OnSystemShutdown() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimplemented (icomp::IRequestManager)
	virtual const IRequest* GetRequest(const QByteArray& requestId) const override;
	virtual const ISender* GetSender(const QByteArray& requestId) const override;

private:
	bool StartListening(const QHostAddress& address = QHostAddress::Any, quint16 port = 0);

private Q_SLOTS:
	void OnNewThreadConnection(const IRequest* request);

private:
	I_REF(imtrest::IRequestServlet, m_requestHandlerCompPtr);
	I_REF(IProtocolEngine, m_protocolEngineCompPtr);
	I_REF(imtbase::IUrlParam, m_serverPortCompPtr);
	I_ATTR(int, m_threadsLimitAttrPtr);
	I_ATTR(bool, m_startServerOnCreateAttrPtr);
	I_ATTR(bool, m_isMultiThreadingAttrPtr);

	istd::TDelPtr<CMultiThreadServer> m_server;

	typedef istd::TPointerVector<IRequest> Requests;
	Requests m_requests;
};


} // namespace imtrest


