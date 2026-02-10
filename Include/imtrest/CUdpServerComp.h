// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QNetworkDatagram>

// ACF includes
#include <istd/TPointerVector.h>
#include <ibase/TRuntimeStatusHanderCompWrap.h>
#include <ilog/TLoggerCompWrap.h>
#include <istd/TDelPtr.h>
#include <iprm/IParamsSet.h>
#include <ifile/IFileNameParam.h>
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imtcom/IServerConnectionInterface.h>
#include <imtrest/IRequest.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/IRequestManager.h>
#include <imtrest/IServer.h>
#include <imtrest/CUdpRequest.h>

namespace imtrest
{


/**
	Udp-based communication server.
	The server uses the underlaying protocol engine for creation of requests and responses.
*/
class CUdpServerComp:
			public QObject,
			public ibase::TRuntimeStatusHanderCompWrap<ilog::CLoggerComponentBase>,
			private imod::CMultiModelDispatcherBase,
			virtual public IRequestManager,
			virtual public IServer
{
	Q_OBJECT
public:
	typedef ibase::TRuntimeStatusHanderCompWrap<ilog::CLoggerComponentBase> BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CUdpServerComp);
		I_REGISTER_INTERFACE(IRequestManager)
		I_REGISTER_INTERFACE(IServer)
		I_ASSIGN(m_requestHandlerCompPtr, "RequestHandler", "Request handler registered for the server", true, "RequestHandler");
		I_ASSIGN(m_protocolEngineCompPtr, "ProtocolEngine", "Protocol engine used in the server", true, "ProtocolEngine");
		I_ASSIGN(m_startServerOnCreateAttrPtr, "StartServerOnCreate", "If enabled, the server will be started on after component creation", true, true);
		I_ASSIGN(m_serverConnnectionInterfaceCompPtr, "ServerInterface", "Parameter providing the server connection interface to be listened", true, "ServerInterface");
	I_END_COMPONENT

	CUdpServerComp();
	~CUdpServerComp();

	imtrest::IRequestServlet* GetRequestServlet();
	int GetThreadsLimit();

protected:
	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (ibase::TRuntimeStatusHanderCompWrap)
	virtual void OnSystemShutdown() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimplemented (icomp::IRequestManager)
	virtual const ISender* GetSender(const QByteArray& requestId) const override;

	// reimplemented (imtrest::IServer)
	virtual bool StartServer() override;
	virtual bool StopServer() override;
	virtual ServerStatus GetServerStatus() const override;

private:
	bool StartListening(const QHostAddress& address = QHostAddress::Any, quint16 port = 0);
	bool EnsureServerStarted();

private Q_SLOTS:
	void ReadPendingDatagrams();
    void SendedResponse(QByteArray);

private:
	I_REF(imtrest::IRequestServlet, m_requestHandlerCompPtr);
	I_REF(IProtocolEngine, m_protocolEngineCompPtr);
	I_REF(imtcom::IServerConnectionInterface, m_serverConnnectionInterfaceCompPtr);

	I_ATTR(bool, m_startServerOnCreateAttrPtr);

	QUdpSocket m_udpSocket;

	typedef istd::TPointerVector<CUdpRequest> Requests;
	Requests m_requests;

	bool m_isInitialized = false;
};


} // namespace imtrest


