// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CRemoteServiceInfoComp.h>


// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequest.h>
#include <GeneratedFiles/imtappsdl/SDL/1.0/CPP/Application.h>


namespace imtclientgql
{


// Bounds the tree, e.g. when two services depend on each other
static const int s_maxDependencyDepth = 8;


// public methods

// reimplemented (imtcom::IServiceInfoProvider)

imtcom::IServiceInfoProvider::ServiceInfo CRemoteServiceInfoComp::GetServiceInfo() const
{
	QMutexLocker locker(&m_serviceInfoMutex);

	return m_serviceInfo;
}


// protected methods

// reimplemented (imod::CMultiModelDispatcherBase)

void CRemoteServiceInfoComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& changeSet)
{
	// Attaching the model notifies with CF_NO_UNDO only, the first request is sent after it
	if (changeSet.ContainsExplicit(istd::IChangeable::CF_NO_UNDO, true)){
		return;
	}

	UpdateServiceInfo();
}


// reimplemented (icomp::CComponentBase)

void CRemoteServiceInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_serviceInfo.serviceId = *m_serviceIdAttrPtr;

	// An auto instance is created before QCoreApplication, the connection (e.g. its SSL setup) must not be created that early
	QMetaObject::invokeMethod(this, [this](){
		BaseClass2::RegisterModel(m_connectionStatusModelCompPtr.GetPtr());

		if (m_serverCredentialCompPtr.IsValid() && !m_serverCredentialCompPtr->GetServerToken().isEmpty()){
			connect(&m_credentialCheckTimer, &QTimer::timeout, this, [this](){ UpdateServiceInfo(); });
			m_credentialCheckTimer.start(*m_credentialCheckIntervalAttrPtr * 1000);
		}

		UpdateServiceInfo();
	}, Qt::QueuedConnection);
}


void CRemoteServiceInfoComp::OnComponentDestroyed()
{
	m_credentialCheckTimer.stop();

	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CRemoteServiceInfoComp::UpdateServiceInfo()
{
	const QByteArray serverToken = m_serverCredentialCompPtr.IsValid() ? m_serverCredentialCompPtr->GetServerToken() : QByteArray();
	if (m_serverCredentialCompPtr.IsValid() && serverToken.isEmpty()){
		SetCredentialStatus(CRS_MISSING);
	}

	const imtcom::IConnectionStatusProvider::ConnectionStatus connectionStatus = m_connectionStatusCompPtr->GetConnectionStatus();
	if (connectionStatus != imtcom::IConnectionStatusProvider::CS_CONNECTED){
		// Also drops the answer of a request still in flight when the connection is lost
		++m_lastRequestId;

		if (!serverToken.isEmpty()){
			SetCredentialStatus(CRS_UNAVAILABLE);
		}
		SetServiceStatus(connectionStatus);

		return;
	}

	// Unknown until the service answers, a periodic check of the token keeps what is known
	if (GetServiceInfo().status != imtcom::IConnectionStatusProvider::CS_CONNECTED){
		SetServiceStatus(imtcom::IConnectionStatusProvider::CS_UNKNOWN);
	}

	SendInfoRequest(!serverToken.isEmpty());
}


void CRemoteServiceInfoComp::SendInfoRequest(bool useToken)
{
	const int requestId = ++m_lastRequestId;

	imtgql::CGqlRequest request;
	sdl::V1_0::imtapp::GetApplicationInfoRequestArguments arguments;
	arguments.input.emplace();
	if (!sdl::V1_0::imtapp::CGetApplicationInfoGqlRequest::SetupGqlRequest(request, arguments)){
		return;
	}

	if (useToken){
		const QByteArray serverToken = m_serverCredentialCompPtr->GetServerToken();

		imtgql::IGqlContext::Headers headers;
		headers.insert(imtbase::s_authenticationTokenHeaderId, serverToken);

		imtgql::IGqlContextUniquePtr contextPtr(new imtgql::CGqlContext);
		contextPtr->SetToken(serverToken);
		contextPtr->SetHeaders(headers);
		request.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(contextPtr)));
	}

	m_asyncClientCompPtr->SendModelRequest<sdl::V1_0::imtapp::CApplicationInfo>(request).then(this, [this, requestId, useToken](const GqlModelResult<sdl::V1_0::imtapp::CApplicationInfo>& result) {
		OnRequestFinished(requestId, useToken, result);
	});
}


void CRemoteServiceInfoComp::OnRequestFinished(int requestId, bool isTokenUsed, const GqlModelResult<sdl::V1_0::imtapp::CApplicationInfo>& result)
{
	if (requestId != m_lastRequestId){
		return;
	}

	if (isTokenUsed && result.errorCategory == IAsyncGqlClient::EC_ACCESS_DENIED){
		SetCredentialStatus(CRS_REJECTED);

		// The service is there, its information does not depend on the token
		SendInfoRequest(false);

		return;
	}

	// The connection is up but the communication with the service does not work
	if (result.errorCategory != IAsyncGqlClient::EC_NONE){
		if (isTokenUsed){
			SetCredentialStatus(CRS_UNAVAILABLE);
		}
		SetServiceStatus(imtcom::IConnectionStatusProvider::CS_DISCONNECTED);

		return;
	}

	if (isTokenUsed){
		SetCredentialStatus(CRS_VALID);
	}

	ServiceInfo serviceInfo;
	serviceInfo.serviceId = *m_serviceIdAttrPtr;
	serviceInfo.status = imtcom::IConnectionStatusProvider::CS_CONNECTED;
	serviceInfo.credentialStatus = GetServiceInfo().credentialStatus;
	if (result.model.applicationName.HasValue()){
		serviceInfo.applicationName = *result.model.applicationName;
	}
	if (result.model.version.HasValue()){
		serviceInfo.version = *result.model.version;
	}
	if (result.model.versionKind.HasValue()){
		serviceInfo.versionKind = *result.model.versionKind;
	}
	if (result.model.buildTime.HasValue()){
		serviceInfo.buildTime = QDateTime::fromString(*result.model.buildTime, Qt::ISODate);
	}
	if (result.model.dependencies.HasValue()){
		ReadDependencies(*result.model.dependencies, 1, serviceInfo.dependencies);
	}

	SetServiceInfo(serviceInfo);
}


void CRemoteServiceInfoComp::SetServiceStatus(imtcom::IConnectionStatusProvider::ConnectionStatus status)
{
	QMutexLocker locker(&m_serviceInfoMutex);

	// The application name stays, it identifies the service that is not reachable
	ServiceInfo serviceInfo;
	serviceInfo.serviceId = m_serviceInfo.serviceId;
	serviceInfo.applicationName = m_serviceInfo.applicationName;
	serviceInfo.credentialStatus = m_serviceInfo.credentialStatus;
	serviceInfo.status = status;

	m_serviceInfo = serviceInfo;
}


void CRemoteServiceInfoComp::SetServiceInfo(const ServiceInfo& serviceInfo)
{
	QMutexLocker locker(&m_serviceInfoMutex);

	m_serviceInfo = serviceInfo;
}


void CRemoteServiceInfoComp::SetCredentialStatus(CredentialStatus status)
{
	QMutexLocker locker(&m_serviceInfoMutex);

	const bool isChanged = m_serviceInfo.credentialStatus != status;
	m_serviceInfo.credentialStatus = status;

	locker.unlock();

	if (isChanged && status == CRS_REJECTED){
		qCritical().noquote() << QStringLiteral("The service '%1' refuses the token of this server: it is wrong, revoked or issued for another product").arg(QString::fromUtf8(*m_serviceIdAttrPtr));
	}
}


void CRemoteServiceInfoComp::ReadDependencies(
			const imtsdl::TElementList<sdl::V1_0::imtapp::CServiceInfo>& dependencyList,
			int depth,
			QList<ServiceInfo>& dependencies)
{
	if (depth > s_maxDependencyDepth){
		return;
	}

	for (const istd::TNullableValue<sdl::V1_0::imtapp::CServiceInfo>& dependencyData : dependencyList){
		if (!dependencyData.HasValue()){
			continue;
		}

		ServiceInfo dependency;
		if (dependencyData->serviceId.HasValue()){
			dependency.serviceId = *dependencyData->serviceId;
		}
		if (dependencyData->status.HasValue()){
			switch (*dependencyData->status){
			case sdl::V1_0::imtapp::ServiceStatus::CONNECTED:
				dependency.status = imtcom::IConnectionStatusProvider::CS_CONNECTED;
				break;
			case sdl::V1_0::imtapp::ServiceStatus::DISCONNECTED:
				dependency.status = imtcom::IConnectionStatusProvider::CS_DISCONNECTED;
				break;
			default:
				dependency.status = imtcom::IConnectionStatusProvider::CS_UNKNOWN;
				break;
			}
		}
		if (dependencyData->credentialStatus.HasValue()){
			switch (*dependencyData->credentialStatus){
			case sdl::V1_0::imtapp::ServerCredentialStatus::VALID:
				dependency.credentialStatus = CRS_VALID;
				break;
			case sdl::V1_0::imtapp::ServerCredentialStatus::MISSING:
				dependency.credentialStatus = CRS_MISSING;
				break;
			case sdl::V1_0::imtapp::ServerCredentialStatus::REJECTED:
				dependency.credentialStatus = CRS_REJECTED;
				break;
			case sdl::V1_0::imtapp::ServerCredentialStatus::UNAVAILABLE:
				dependency.credentialStatus = CRS_UNAVAILABLE;
				break;
			default:
				break;
			}
		}
		if (dependencyData->applicationName.HasValue()){
			dependency.applicationName = *dependencyData->applicationName;
		}
		if (dependencyData->version.HasValue()){
			dependency.version = *dependencyData->version;
		}
		if (dependencyData->versionKind.HasValue()){
			dependency.versionKind = *dependencyData->versionKind;
		}
		if (dependencyData->buildTime.HasValue()){
			dependency.buildTime = QDateTime::fromString(*dependencyData->buildTime, Qt::ISODate);
		}
		if (dependencyData->dependencies.HasValue()){
			ReadDependencies(*dependencyData->dependencies, depth + 1, dependency.dependencies);
		}

		dependencies << dependency;
	}
}


} // namespace imtclientgql


