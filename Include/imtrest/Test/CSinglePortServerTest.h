// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <icomp/CComponentBase.h>
#include <icomp/TSimComponentWrap.h>
#include <icomp/TSimComponentsFactory.h>
#include <iprm/CComposedParamsSetComp.h>
#include <iprm/CEnableableParamComp.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtcom/CServerConnectionInterfaceParamComp.h>
#include <imtcom/ISslConfigurationManager.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/CHttpProtocolEngineComp.h>
#include <imtrest/CTcpServerComp.h>
#include <imtrest/CWebSocketProtocolEngineComp.h>
#include <imtrest/CWebSocketServerComp.h>
#include <imtrest/CWorkerManagerComp.h>


/**
	HTTP servlet answering every request with 200 "OK".
*/
class CTestHttpServletComp: public icomp::CComponentBase, virtual public imtrest::IRequestServlet
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestHttpServletComp);
		I_REGISTER_INTERFACE(imtrest::IRequestServlet);
	I_END_COMPONENT;

	// reimplemented (imtrest::IRequestServlet)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual imtrest::ConstResponsePtr ProcessRequest(const imtrest::IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;
};


/**
	WebSocket servlet answering connection_init with connection_ack.
*/
class CTestWebSocketServletComp: public icomp::CComponentBase, virtual public imtrest::IRequestServlet
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestWebSocketServletComp);
		I_REGISTER_INTERFACE(imtrest::IRequestServlet);
	I_END_COMPONENT;

	// reimplemented (imtrest::IRequestServlet)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual imtrest::ConstResponsePtr ProcessRequest(const imtrest::IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;
};


/**
	SSL configuration of the test server: the self-signed certificate of 127.0.0.1.
*/
class CTestSslConfigurationManagerComp: public icomp::CComponentBase, virtual public imtcom::ISslConfigurationManager
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestSslConfigurationManagerComp);
		I_REGISTER_INTERFACE(imtcom::ISslConfigurationManager);
	I_END_COMPONENT;

	// reimplemented (imtcom::ISslConfigurationManager)
	virtual bool CreateSslConfiguration(const iprm::IParamsSet& params, QSslConfiguration& output) const override;
};


/**
	HTTP and WebSocket servers wired like in HttpServerFramework and WebSocketServerFramework.
*/
class CTestServers
{
public:
	/**
		\param	isSecure	The HTTP server accepts only TLS connections (HTTPS and WSS on its port).
	*/
	CTestServers(bool isSinglePortMode, bool listenWebSocketPort = true, bool isSecure = false);
	~CTestServers();

	quint16 GetHttpPort() const;
	quint16 GetWebSocketPort() const;
	imtrest::CWebSocketServerComp& GetWebSocketServer();
	const imtrest::IProtocolEngine& GetWebSocketProtocolEngine();

private:
	quint16 m_httpPort;
	quint16 m_webSocketPort;

	icomp::TSimComponentsFactory<CTestHttpServletComp> m_httpServletFactory;
	icomp::TSimSharedComponentPtr<imtcom::CServerConnectionInterfaceParamComp> m_serverInterfaceCompPtr;
	icomp::TSimSharedComponentPtr<imtrest::CHttpProtocolEngineComp> m_httpProtocolEngineCompPtr;
	icomp::TSimSharedComponentPtr<imtrest::CWebSocketProtocolEngineComp> m_webSocketProtocolEngineCompPtr;
	icomp::TSimSharedComponentPtr<CTestWebSocketServletComp> m_webSocketServletCompPtr;
	icomp::TSimSharedComponentPtr<imtrest::CWorkerManagerComp> m_workerManagerCompPtr;
	icomp::TSimSharedComponentPtr<imtrest::CWebSocketServerComp> m_webSocketServerCompPtr;
	icomp::TSimSharedComponentPtr<imtrest::CTcpServerComp> m_tcpServerCompPtr;
	icomp::TSimSharedComponentPtr<iprm::CEnableableParamComp> m_sslEnableCompPtr;
	icomp::TSimSharedComponentPtr<iprm::CComposedParamsSetComp> m_sslParamsCompPtr;
	icomp::TSimSharedComponentPtr<CTestSslConfigurationManagerComp> m_sslManagerCompPtr;
};


class CSinglePortServerTest: public QObject
{
	Q_OBJECT

private slots:
	void HttpRequestOnSinglePortTest();
	void WebSocketSubscriptionOnSinglePortTest();
	void WebSocketAbortOnSinglePortTest();
	void SeparateWebSocketPortDisabledTest();
	void SecureWebSocketOnSinglePortTest();
	void TwoPortModeTest();
};


