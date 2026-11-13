// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CAuthorizationServerProductInfoComp.h>


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QDeadlineTimer>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QSet>

// ImtCore includes
#include <imtauthgql/imtauthgql.h>
#include <imtcom/IConnectionStatusProvider.h>
#include <imtgql/imtgql.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequest.h>
#include <imtlic/CFeatureInfo.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/ProductPermissionCatalog.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/ServerTokens.h>


namespace imtauthgql
{


namespace
{


const int s_retryDelay = 30000;


void CopyServerFeature(const sdl::V1_0::imtauth::CServerFeature& serverFeature, imtlic::CFeatureInfo& featureInfo)
{
	featureInfo.SetFeatureId(serverFeature.featureId ? QByteArray(*serverFeature.featureId) : QByteArray());
	featureInfo.SetFeatureName(serverFeature.name ? QString(*serverFeature.name) : QString());
	featureInfo.SetFeatureDescription(serverFeature.description ? QString(*serverFeature.description) : QString());
	featureInfo.SetOptional(false);
	featureInfo.SetIsPermission(true);
	featureInfo.SetRequirements(serverFeature.requirements ? serverFeature.requirements->ToList() : QByteArrayList());

	if (serverFeature.subFeatures){
		for (const sdl::V1_0::imtauth::CServerFeature& serverSubFeature : serverFeature.subFeatures->ToList()){
			imtlic::CFeatureInfo* subFeatureInfoPtr = new imtlic::CFeatureInfo;
			CopyServerFeature(serverSubFeature, *subFeatureInfoPtr);
			featureInfo.InsertSubFeature(imtlic::IFeatureInfo::FeatureInfoPtr(subFeatureInfoPtr));
		}
	}
}


} // anonymous namespace


// public methods

// reimplemented (imtlic::IProductInfo)

const imtlic::IProductInfo* CAuthorizationServerProductInfoComp::GetProductFamilyInfo() const
{
	return GetCurrent().GetProductFamilyInfo();
}


QByteArray CAuthorizationServerProductInfoComp::GetProductId() const
{
	return m_productInfoCompPtr->GetProductId();
}


void CAuthorizationServerProductInfoComp::SetProductId(const QByteArray& productId)
{
	m_productInfoCompPtr->SetProductId(productId);

	Invalidate();
}


QString CAuthorizationServerProductInfoComp::GetProductDescription() const
{
	return GetCurrent().GetProductDescription();
}


void CAuthorizationServerProductInfoComp::SetProductDescription(const QString& description)
{
	m_productInfoCompPtr->SetProductDescription(description);

	Invalidate();
}


QByteArray CAuthorizationServerProductInfoComp::GetCategoryId() const
{
	return GetCurrent().GetCategoryId();
}


void CAuthorizationServerProductInfoComp::SetCategoryId(const QByteArray& categoryId)
{
	m_productInfoCompPtr->SetCategoryId(categoryId);

	Invalidate();
}


imtbase::IObjectCollection* CAuthorizationServerProductInfoComp::GetFeatures()
{
	return GetCurrent().GetFeatures();
}


bool CAuthorizationServerProductInfoComp::AddFeature(const QByteArray& featureId, const imtlic::IFeatureInfo& featureInfo)
{
	const bool retVal = m_productInfoCompPtr->AddFeature(featureId, featureInfo);

	Invalidate();

	return retVal;
}


bool CAuthorizationServerProductInfoComp::RemoveFeature(const QByteArray& featureId)
{
	const bool retVal = m_productInfoCompPtr->RemoveFeature(featureId);

	Invalidate();

	return retVal;
}


imtlic::IProductInfo::OptionalFeatureInfos CAuthorizationServerProductInfoComp::GetOptionalFeatures() const
{
	return GetCurrent().GetOptionalFeatures();
}


void CAuthorizationServerProductInfoComp::SetOptionalFeatures(const OptionalFeatureInfos& optionalFeatures)
{
	m_productInfoCompPtr->SetOptionalFeatures(optionalFeatures);

	Invalidate();
}


// reimplemented (iprm::INameParam)

const QString& CAuthorizationServerProductInfoComp::GetName() const
{
	return GetCurrent().GetName();
}


void CAuthorizationServerProductInfoComp::SetName(const QString& name)
{
	m_productInfoCompPtr->SetName(name);

	Invalidate();
}


bool CAuthorizationServerProductInfoComp::IsNameFixed() const
{
	return m_productInfoCompPtr->IsNameFixed();
}


// reimplemented (iser::ISerializable)

bool CAuthorizationServerProductInfoComp::Serialize(iser::IArchive& archive)
{
	// The served tree is derived: only the tree of the product is stored and restored
	const bool retVal = m_productInfoCompPtr->Serialize(archive);

	Invalidate();

	return retVal;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CAuthorizationServerProductInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// An auto instance is created before QCoreApplication, the client (e.g. its SSL setup) must wait for the event loop
	QMetaObject::invokeMethod(this, [this](){
		// The product filled its tree before the event loop: a tree served earlier misses it
		Invalidate();

		// Without a token only an authorization server that takes products without tokens gives its permissions, and takes none
		m_isPublishNeeded = m_serverCredentialCompPtr.IsValid() && !m_serverCredentialCompPtr->GetServerToken().isEmpty();
		m_isPublishEnabled = m_isPublishNeeded;

		m_threadPtr = QThread::create([this](){ Run(); });
		m_threadPtr->setObjectName(QStringLiteral("AuthorizationServerPermissions"));
		m_threadPtr->start();

		if (m_connectionStatusModelCompPtr.IsValid()){
			m_connectionStatusModelCompPtr->AttachObserver(this);
		}
	}, Qt::QueuedConnection);
}


void CAuthorizationServerProductInfoComp::OnComponentDestroyed()
{
	EnsureModelDetached();

	{
		QMutexLocker locker(&m_runMutex);
		m_isStopping = true;
		m_runCondition.wakeAll();
	}

	if (m_threadPtr != nullptr){
		m_threadPtr->wait();
		delete m_threadPtr;
		m_threadPtr = nullptr;
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CAuthorizationServerProductInfoComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!changeSet.Contains(imtcom::IConnectionStatusProvider::CF_CONNECTED)){
		return;
	}

	QMutexLocker locker(&m_runMutex);
	m_isPublishNeeded = m_isPublishEnabled;
	m_isFetchNeeded = true;
	m_isReconnected = true;
	m_runCondition.wakeAll();
}


// private methods

imtlic::CProductInfo& CAuthorizationServerProductInfoComp::GetCurrent() const
{
	QMutexLocker locker(&m_treeMutex);

	if (m_currentTreePtr == nullptr){
		const ProductInfoPtr treePtr = CreateTree();
		m_trees.append(treePtr);
		m_currentTreePtr = treePtr.get();
	}

	return *m_currentTreePtr;
}


CAuthorizationServerProductInfoComp::ProductInfoPtr CAuthorizationServerProductInfoComp::CreateTree() const
{
	ProductInfoPtr treePtr = std::make_shared<imtlic::CProductInfo>();
	treePtr->CopyFrom(*m_productInfoCompPtr);

	if (m_importedTreePtr == nullptr){
		return treePtr;
	}

	imtbase::IObjectCollection* importedCollectionPtr = m_importedTreePtr->GetFeatures();
	imtbase::IObjectCollection* ownCollectionPtr = treePtr->GetFeatures();
	if (importedCollectionPtr == nullptr || ownCollectionPtr == nullptr){
		return treePtr;
	}

	// A permission of the authorization server is given by it: a copy in the tree of the product gives way
	const imtbase::ICollectionInfo::Ids importedIds = importedCollectionPtr->GetElementIds();
	const QSet<QByteArray> importedFeatureIds(importedIds.cbegin(), importedIds.cend());
	for (const imtbase::ICollectionInfo::Id& elementId : ownCollectionPtr->GetElementIds()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		const imtlic::IFeatureInfo* featureInfoPtr = ownCollectionPtr->GetObjectData(elementId, dataPtr)
					? dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr())
					: nullptr;
		if (featureInfoPtr != nullptr && importedFeatureIds.contains(featureInfoPtr->GetFeatureId())){
			treePtr->RemoveFeature(elementId);
		}
	}

	for (const imtbase::ICollectionInfo::Id& elementId : importedIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		const imtlic::IFeatureInfo* featureInfoPtr = importedCollectionPtr->GetObjectData(elementId, dataPtr)
					? dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr())
					: nullptr;
		if (featureInfoPtr != nullptr){
			treePtr->AddFeature(elementId, *featureInfoPtr);
		}
	}

	return treePtr;
}


void CAuthorizationServerProductInfoComp::Invalidate()
{
	QMutexLocker locker(&m_treeMutex);

	m_currentTreePtr = nullptr;
}


bool CAuthorizationServerProductInfoComp::Publish() const
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
		return false;
	}

	QString errorMessage;
	const sdl::V1_0::imtauth::CPublishProductPermissionsPayload payload = SendModelRequest<sdl::V1_0::imtauth::CPublishProductPermissionsPayload>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty() || !payload.success || !*payload.success){
		SendWarningMessage(0, QStringLiteral("The authorization server did not accept the permissions: %1").arg(errorMessage), "CAuthorizationServerProductInfoComp");

		return false;
	}

	return true;
}


QString CAuthorizationServerProductInfoComp::GetProductVersion() const
{
	if (!m_applicationInfoCompPtr.IsValid()){
		return QString();
	}

	const iser::IVersionInfo& versionInfo = m_applicationInfoCompPtr->GetVersionInfo();
	const int mainVersion = m_applicationInfoCompPtr->GetMainVersionId();

	quint32 versionNumber = 0;

	return versionInfo.GetVersionNumber(mainVersion, versionNumber) ? versionInfo.GetEncodedVersionName(mainVersion, versionNumber) : QString();
}


bool CAuthorizationServerProductInfoComp::Fetch()
{
	sdl::V1_0::imtauth::GetServerTokenFeaturesRequestArguments arguments;

	// This server asks for itself: its token goes with every request
	imtgql::CGqlRequest gqlRequest;
	imtgql::SetServerRequestContext(gqlRequest);
	if (!sdl::V1_0::imtauth::CGetServerTokenFeaturesGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	QString errorMessage;
	const sdl::V1_0::imtauth::CServerFeatureList payload = SendModelRequest<sdl::V1_0::imtauth::CServerFeatureList>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		SendWarningMessage(0, QStringLiteral("The permissions of the authorization server were not received: %1").arg(errorMessage), "CAuthorizationServerProductInfoComp");

		return false;
	}

	QJsonObject payloadObject;
	if (!payload.WriteToJsonObject(payloadObject)){
		return false;
	}

	const QByteArray hash = QCryptographicHash::hash(QJsonDocument(payloadObject).toJson(QJsonDocument::Compact), QCryptographicHash::Sha256);

	ProductInfoPtr importedTreePtr = std::make_shared<imtlic::CProductInfo>();
	int featureCount = 0;
	if (payload.features){
		for (const sdl::V1_0::imtauth::CServerFeature& serverFeature : payload.features->ToList()){
			// A product takes only an identifiable feature at its root
			imtlic::CIdentifiableFeatureInfo featureInfo;
			CopyServerFeature(serverFeature, featureInfo);
			featureInfo.SetObjectUuid(featureInfo.GetFeatureId());
			if (importedTreePtr->AddFeature(featureInfo.GetFeatureId(), featureInfo)){
				featureCount++;
			}
		}
	}

	{
		QMutexLocker locker(&m_treeMutex);

		if (hash == m_importedHash){
			return true;
		}

		m_importedHash = hash;
		m_importedTreePtr = importedTreePtr;
		m_currentTreePtr = nullptr;
	}

	SendInfoMessage(0, QStringLiteral("Received %1 permission groups of the authorization server for the roles of this product").arg(featureCount), "CAuthorizationServerProductInfoComp");

	return true;
}


void CAuthorizationServerProductInfoComp::Run()
{
	QMutexLocker locker(&m_runMutex);
	while (!m_isStopping){
		if (!m_isPublishNeeded && !m_isFetchNeeded){
			m_runCondition.wait(&m_runMutex);

			continue;
		}

		const bool isPublishNeeded = m_isPublishNeeded;
		const bool isFetchNeeded = m_isFetchNeeded;
		m_isPublishNeeded = false;
		m_isFetchNeeded = false;
		m_isReconnected = false;

		locker.unlock();
		const bool isPublished = !isPublishNeeded || Publish();
		const bool isFetched = !isFetchNeeded || Fetch();
		locker.relock();

		if (isPublished && isFetched){
			continue;
		}

		// A failed step is repeated later, or at once when the connection comes back
		m_isPublishNeeded = m_isPublishNeeded || !isPublished;
		m_isFetchNeeded = m_isFetchNeeded || !isFetched;

		const QDeadlineTimer deadline(s_retryDelay);
		while (!m_isStopping && !m_isReconnected && !deadline.hasExpired()){
			m_runCondition.wait(&m_runMutex, deadline);
		}
	}
}


} // namespace imtauthgql


