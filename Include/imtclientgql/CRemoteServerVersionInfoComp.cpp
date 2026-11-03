// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CRemoteServerVersionInfoComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <GeneratedFiles/imtappsdl/SDL/1.0/CPP/Application.h>


namespace imtclientgql
{


// public methods

// reimplemented (imtcom::IServerVersionInfoProvider)

bool CRemoteServerVersionInfoComp::GetServerVersionInfo(ServerVersionInfo& versionInfo) const
{
	QMutexLocker locker(&m_versionInfoMutex);

	if (m_hasVersionInfo){
		versionInfo = m_versionInfo;
	}

	return m_hasVersionInfo;
}


// protected methods

// reimplemented (imod::CMultiModelDispatcherBase)

void CRemoteServerVersionInfoComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& changeSet)
{
	// Attaching the model notifies with CF_NO_UNDO only, the first request is sent after it
	if (changeSet.ContainsExplicit(istd::IChangeable::CF_NO_UNDO, true)){
		return;
	}

	UpdateVersionInfo();
}


// reimplemented (icomp::CComponentBase)

void CRemoteServerVersionInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	BaseClass2::RegisterModel(m_connectionStatusModelCompPtr.GetPtr());

	UpdateVersionInfo();
}


void CRemoteServerVersionInfoComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CRemoteServerVersionInfoComp::UpdateVersionInfo()
{
	// Also drops the answer of a request still in flight when the connection is lost
	const int requestId = ++m_lastRequestId;

	if (m_connectionStatusCompPtr->GetConnectionStatus() != imtcom::IConnectionStatusProvider::CS_CONNECTED){
		SetVersionInfo(nullptr);

		return;
	}

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


void CRemoteServerVersionInfoComp::OnRequestFinished(int requestId, const GqlModelResult<sdl::V1_0::imtapp::CApplicationInfo>& result)
{
	if (requestId != m_lastRequestId){
		return;
	}

	if (result.errorCategory != IAsyncGqlClient::EC_NONE){
		SetVersionInfo(nullptr);

		return;
	}

	ServerVersionInfo versionInfo;
	if (result.model.version.HasValue()){
		versionInfo.version = *result.model.version;
	}
	if (result.model.versionKind.HasValue()){
		versionInfo.versionKind = *result.model.versionKind;
	}
	if (result.model.buildTime.HasValue()){
		versionInfo.buildTime = QDateTime::fromString(*result.model.buildTime, Qt::ISODate);
	}

	SetVersionInfo(&versionInfo);
}


void CRemoteServerVersionInfoComp::SetVersionInfo(const ServerVersionInfo* versionInfoPtr)
{
	QMutexLocker locker(&m_versionInfoMutex);

	m_hasVersionInfo = (versionInfoPtr != nullptr);
	m_versionInfo = m_hasVersionInfo ? *versionInfoPtr : ServerVersionInfo();
}


} // namespace imtclientgql


