#include <imtrest/CTcpServerComp.h>


// Qt includes
#include <QtCore/QFileInfo>
#include <QtNetwork/QSslConfiguration>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IEnableableParam.h>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponse.h>
#include <imtrest/ISender.h>
#include <imtrest/CMultiThreadServer.h>


namespace imtrest
{


// public methods

CTcpServerComp::CTcpServerComp()
	:m_serverPtr(new CMultiThreadServer(this))
{
}


CTcpServerComp::~CTcpServerComp()
{
	m_serverPtr.Reset();
}


imtrest::IRequestServlet* CTcpServerComp::GetRequestServlet()
{
	if (!m_requestHandlerCompPtr.IsValid()){
		return nullptr;
	}

	return m_requestHandlerCompPtr.GetPtr();
}


imtrest::IProtocolEngine* CTcpServerComp::GetProtocolEngine()
{
	if (!m_protocolEngineCompPtr.IsValid()){
		return nullptr;
	}

	return m_protocolEngineCompPtr.GetPtr();
}


int CTcpServerComp::GetThreadsLimit()
{
	return *m_threadsLimitAttrPtr;
}


// protected methods

void CTcpServerComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	Q_ASSERT_X(m_sslConfigurationCompPtr.IsValid() && m_sslConfigurationManagerCompPtr.IsValid(), "Update server's SSL configuration", "SSL configuration or manager is not set!");
	if (m_serverPtr->isListening()){
		SendInfoMessage(0, "SSL configuration updated. Stopping server...");
		m_serverPtr->close();
	}

	QSslConfiguration sslConfiguration;
	iprm::TParamsPtr<iprm::IEnableableParam> sslEnableParamPtr(m_sslConfigurationCompPtr.GetPtr(), imtcom::ISslConfigurationManager::ParamKeys::s_enableSslModeParamKey);
	if (sslEnableParamPtr.IsValid() && sslEnableParamPtr->IsEnabled()){
		if (m_sslConfigurationManagerCompPtr->CreateSslConfiguration(*m_sslConfigurationCompPtr, sslConfiguration)){
			m_serverPtr->EnableSecureConnection();
			m_serverPtr->SetSslConfiguration(sslConfiguration);
		}
	}

	if (!m_serverPtr->isListening()){
		SendInfoMessage(0, "SSL configuration applied. Starting server...");
		if (m_startServerOnCreateAttrPtr.IsValid() && *m_startServerOnCreateAttrPtr){
			if (m_serverPortCompPtr.IsValid()){
				int port = m_serverPortCompPtr->GetUrl().port();

				StartListening(QHostAddress::Any, port);
			}
			else{
				StartListening();
			}
		}
	}

}


// reimplemented (ibase::TRuntimeStatusHanderCompWrap)

void CTcpServerComp::OnSystemShutdown()
{
	BaseClass2::UnregisterAllModels();
}


// reimplemented (icomp::CComponentBase)

void CTcpServerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_serverPtr->SetLogPtr(GetLogPtr());

	if (m_sslConfigurationModelCompPtr.IsValid() && m_sslConfigurationManagerCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_sslConfigurationModelCompPtr.GetPtr());
	}

	if (!m_serverPtr->isListening()){
		if (m_startServerOnCreateAttrPtr.IsValid() && *m_startServerOnCreateAttrPtr){
			if (m_serverPortCompPtr.IsValid()){
				int port = m_serverPortCompPtr->GetUrl().port();

				StartListening(QHostAddress::Any, port);
			}
			else{
				StartListening();
			}
		}
	}
}


const ISender* CTcpServerComp::GetSender(const QByteArray& requestId) const
{
	const ISender* senderPtr = m_serverPtr->GetSender(requestId);

	return senderPtr;
}


// private methods

bool CTcpServerComp::StartListening(const QHostAddress &address, quint16 port)
{
	if (m_serverPtr->listen(address, port)){
		QString message = QString("Server successfully started on port %1").arg(port);

		qDebug() << message;
		SendInfoMessage(0, message);

		connect(m_serverPtr.GetPtr(), &CMultiThreadServer::NewThreadConnection, this, &CTcpServerComp::OnNewThreadConnection);

		return true;
	}
	else{
		QString errorMessage = QString("Server could not be started on port %1. Error: %2").arg(port).arg(m_serverPtr->errorString());
		qDebug() << errorMessage;

		SendErrorMessage(0, errorMessage);
	}

	return false;
}


void CTcpServerComp::OnNewThreadConnection(const IRequest* request, const QByteArray& subCommandId)
{
	if (m_requestHandlerCompPtr.IsValid()){
		m_requestHandlerCompPtr->ProcessRequest(*request, subCommandId);
	}
}


} // namespace imtrest


