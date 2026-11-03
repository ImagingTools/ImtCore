// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CRemoteServiceInfoComp.h>


// ImtCore includes
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

	BaseClass2::RegisterModel(m_connectionStatusModelCompPtr.GetPtr());

	UpdateServiceInfo();
}


void CRemoteServiceInfoComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CRemoteServiceInfoComp::UpdateServiceInfo()
{
	// Also drops the answer of a request still in flight when the connection is lost
	const int requestId = ++m_lastRequestId;

	const imtcom::IConnectionStatusProvider::ConnectionStatus connectionStatus = m_connectionStatusCompPtr->GetConnectionStatus();
	if (connectionStatus != imtcom::IConnectionStatusProvider::CS_CONNECTED){
		SetServiceStatus(connectionStatus);

		return;
	}

	// Unknown until the service answers
	SetServiceStatus(imtcom::IConnectionStatusProvider::CS_UNKNOWN);

	imtgql::CGqlRequest request;
	sdl::V1_0::imtapp::GetApplicationInfoRequestArguments arguments;
	arguments.input.emplace();
	if (!sdl::V1_0::imtapp::CGetApplicationInfoGqlRequest::SetupGqlRequest(request, arguments)){
		return;
	}

	m_asyncClientCompPtr->SendModelRequest<sdl::V1_0::imtapp::CApplicationInfo>(request).then(this, [this, requestId](const GqlModelResult<sdl::V1_0::imtapp::CApplicationInfo>& result) {
		OnRequestFinished(requestId, result);
	});
}


void CRemoteServiceInfoComp::OnRequestFinished(int requestId, const GqlModelResult<sdl::V1_0::imtapp::CApplicationInfo>& result)
{
	if (requestId != m_lastRequestId){
		return;
	}

	// The connection is up but the communication with the service does not work
	if (result.errorCategory != IAsyncGqlClient::EC_NONE){
		SetServiceStatus(imtcom::IConnectionStatusProvider::CS_DISCONNECTED);

		return;
	}

	ServiceInfo serviceInfo;
	serviceInfo.serviceId = *m_serviceIdAttrPtr;
	serviceInfo.status = imtcom::IConnectionStatusProvider::CS_CONNECTED;
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
	serviceInfo.status = status;

	m_serviceInfo = serviceInfo;
}


void CRemoteServiceInfoComp::SetServiceInfo(const ServiceInfo& serviceInfo)
{
	QMutexLocker locker(&m_serviceInfoMutex);

	m_serviceInfo = serviceInfo;
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


