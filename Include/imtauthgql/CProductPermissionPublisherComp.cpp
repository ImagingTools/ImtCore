// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CProductPermissionPublisherComp.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QDeadlineTimer>

// ImtCore includes
#include <imtauthgql/imtauthgql.h>
#include <imtcom/IConnectionStatusProvider.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequest.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/ProductPermissionCatalog.h>


namespace imtauthgql
{


namespace
{


const int s_firstDelay = 5000;
const int s_retryDelay = 30000;


} // namespace


// protected methods

// reimplemented (icomp::CComponentBase)

void CProductPermissionPublisherComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// An auto instance is created before QCoreApplication, the client (e.g. its SSL setup) must wait for the event loop
	QMetaObject::invokeMethod(this, [this](){
		// Without a token there is nothing to do; the credential provider has reported that
		if (m_serverCredentialCompPtr->GetServerToken().isEmpty()){
			return;
		}

		m_threadPtr = QThread::create([this](){ Run(); });
		m_threadPtr->setObjectName(QStringLiteral("ProductPermissionPublisher"));
		m_threadPtr->start();

		if (m_connectionStatusModelCompPtr.IsValid()){
			m_connectionStatusModelCompPtr->AttachObserver(this);
		}
	}, Qt::QueuedConnection);
}


// reimplemented (imod::CSingleModelObserverBase)

void CProductPermissionPublisherComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!changeSet.Contains(imtcom::IConnectionStatusProvider::CF_CONNECTED)){
		return;
	}

	QMutexLocker locker(&m_mutex);
	m_isPublishNeeded = true;
	m_condition.wakeAll();
}


void CProductPermissionPublisherComp::OnComponentDestroyed()
{
	EnsureModelDetached();

	{
		QMutexLocker locker(&m_mutex);
		m_isStopping = true;
		m_condition.wakeAll();
	}

	if (m_threadPtr != nullptr){
		m_threadPtr->wait();
		delete m_threadPtr;
		m_threadPtr = nullptr;
	}

	BaseClass::OnComponentDestroyed();
}


// private methods

bool CProductPermissionPublisherComp::Publish() const
{
	const QByteArray productId = m_productInfoCompPtr->GetProductId();

	sdl::V1_0::imtauth::PublishProductPermissionsRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->productId = productId;
	arguments.input->productVersion = GetProductVersion();

	sdl::V1_0::imtauth::CPermissionTree tree;
	BuildPermissionGroups<sdl::V1_0::imtauth::CPermissionGroup, sdl::V1_0::imtauth::CPermissionEntry>(
				m_productInfoCompPtr.GetPtr(),
				nullptr,
				tree.groups.Emplace(),
				QByteArray(),
				nullptr);
	arguments.input->tree = tree;

	imtgql::IGqlContextUniquePtr contextPtr(new imtgql::CGqlContext);
	contextPtr->SetToken(m_serverCredentialCompPtr->GetServerToken());
	contextPtr->SetProductId(productId);

	imtgql::CGqlRequest gqlRequest;
	gqlRequest.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(contextPtr)));
	if (!sdl::V1_0::imtauth::CPublishProductPermissionsGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		SendErrorMessage(0, QStringLiteral("Unable to create the request publishing the permissions"), "CProductPermissionPublisherComp");

		return false;
	}

	imtclientgql::IGqlClient::GqlRequestPtr requestPtr;
	requestPtr.MoveCastedPtr(gqlRequest.CloneMe());
	if (!requestPtr.IsValid()){
		return false;
	}

	const imtclientgql::IGqlClient::GqlResponsePtr responsePtr = m_apiClientCompPtr->SendRequest(requestPtr);
	if (!responsePtr.IsValid()){
		return false;
	}

	QString errorMessage;
	imtclientgql::CClientRequestModelHelpers::ParseError parseError = imtclientgql::CClientRequestModelHelpers::PE_NONE;
	const sdl::V1_0::imtauth::CPublishProductPermissionsPayload payload =
				imtclientgql::CClientRequestModelHelpers::ParseModelResponse<sdl::V1_0::imtauth::CPublishProductPermissionsPayload>(
							responsePtr->GetResponseData(),
							gqlRequest.GetCommandId(),
							errorMessage,
							&parseError);
	if (parseError != imtclientgql::CClientRequestModelHelpers::PE_NONE || !payload.success || !*payload.success){
		SendWarningMessage(0, QStringLiteral("The authorization server did not accept the permissions: %1").arg(errorMessage), "CProductPermissionPublisherComp");

		return false;
	}

	return true;
}


QString CProductPermissionPublisherComp::GetProductVersion() const
{
	if (!m_applicationInfoCompPtr.IsValid()){
		return QString();
	}

	const iser::IVersionInfo& versionInfo = m_applicationInfoCompPtr->GetVersionInfo();
	const int mainVersion = m_applicationInfoCompPtr->GetMainVersionId();

	quint32 versionNumber = 0;

	return versionInfo.GetVersionNumber(mainVersion, versionNumber) ? versionInfo.GetEncodedVersionName(mainVersion, versionNumber) : QString();
}


void CProductPermissionPublisherComp::Run()
{
	int delay = s_firstDelay;

	QMutexLocker locker(&m_mutex);
	while (!m_isStopping){
		if (!m_isPublishNeeded){
			m_condition.wait(&m_mutex);

			delay = s_firstDelay;

			continue;
		}

		const QDeadlineTimer deadline(delay);
		while (!m_isStopping && !deadline.hasExpired()){
			m_condition.wait(&m_mutex, deadline);
		}
		if (m_isStopping){
			break;
		}

		m_isPublishNeeded = false;

		locker.unlock();
		const bool isPublished = Publish();
		locker.relock();

		if (!isPublished){
			m_isPublishNeeded = true;
			delay = s_retryDelay;
		}
	}
}


} // namespace imtauthgql
