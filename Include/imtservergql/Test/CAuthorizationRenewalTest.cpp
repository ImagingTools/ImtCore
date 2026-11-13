// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CAuthorizationRenewalTest.h"


#include <functional>
#include <memory>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <icomp/TSimComponentWrap.h>
#include <ilog/CMessage.h>
#include <iprm/TParamsPtr.h>
#include <iser/CJsonMemReadArchive.h>
#include <iser/CJsonMemWriteArchive.h>
#include <itest/CStandardTestExecutor.h>
#include <imtauth/CJwtSessionControllerComp.h>
#include <imtauth/CPersonalAccessTokenManagerComp.h>
#include <imtauth/CRole.h>
#include <imtauth/CSessionInfo.h>
#include <imtauth/CTenantInfo.h>
#include <imtauth/CTenantInfoComp.h>
#include <imtauth/CTenantManagerComp.h>
#include <imtauth/CTenantMembership.h>
#include <imtauth/CUserGroupInfo.h>
#include <imtauth/CUserInfo.h>
#include <imtauth/IJwtSessionController.h>
#include <imtauth/IRoleInfoProvider.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/IUserGroupInfoProvider.h>
#include <imtauth/ITenantEntityBindingManager.h>
#include <imtauth/IDelegatedAccess.h>
#include <imtauthgql/CGqlJwtSessionControllerComp.h>
#include <imtauthgql/CPersonalAccessTokenControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/PersonalAccessTokens.h>
#include <imtauthgql/imtauthgql.h>
#include <imtauthgql/CRemoteJwtSessionControllerComp.h>
#include <imtauthgql/CUserCollectionControllerComp.h>
#include <imtauth/CUserInfoComp.h>
#include <imtlic/CProductInfoComp.h>
#include <imtcore/Version.h>
#include <imtbase/CCachedObjectCollectionComp.h>
#include <imtbase/CCollectionInfo.h>
#include <imtbase/CObjectCollection.h>
#include <imtbase/IIdentifiable.h>
#include <imtbase/IComplexCollectionFilter.h>
#include <imtbase/imtbase.h>
#include <imtclientgql/IGqlClient.h>
#include <imtclientgql/CApiClientComp.h>
#include <imtclientgql/CGqlClientSyncAdapterComp.h>
#include <imtclientgql/CGqlObjectCollectionComp.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <imtgql/CGqlResponse.h>
#include <imtservergql/CAuthenticationManagerComp.h>
#include <imtservergql/CAuthorizationSnapshot.h>
#include <imtservergql/IServerTokenReceiver.h>
#include <imtservergql/imtservergql.h>
#include <imtserverapp/CGqlObjectCollectionDelegateComp.h>


namespace
{


typedef imtgql::IGqlContextCreator Creator;

const QByteArray s_userId = "6a1e5d1c-7f0b-4b5e-9a3e-2b4f1c0d9e01";
const QByteArray s_userLogin = "renewal-user";
const QByteArray s_otherUserId = "4f3e2d1c-0b9a-4876-9543-210fedcba987";
const QByteArray s_tenantId = "0c9d8e7f-6a5b-4c3d-8e2f-1a0b9c8d7e6f";
const QByteArray s_sessionId = "3f2e1d0c-9b8a-4f6e-8d5c-4b3a2f1e0d9c";
const QByteArray s_productId = "Product";
const QByteArray s_roleId = "1b2c3d4e-5f6a-4b7c-8d9e-0f1a2b3c4d5e";
const QByteArray s_otherTenantId = "7d6c5b4a-3f2e-4d1c-9b0a-8f7e6d5c4b3a";

QByteArray GetAuthorizationFingerprint(const imtgql::IGqlContext& context)
{
	const imtservergql::AuthorizationData data = imtservergql::GetAuthorizationData(context);
	QByteArrayList parts;
	parts << QByteArray::number(data.principalKind)
		  << data.userId
		  << data.tenantId
		  << data.productId
		  << data.serverProductId
		  << QByteArray(!data.hasUser ? "-" : (data.isAdmin ? "admin" : "user"))
		  << QByteArray(data.isTenantOwner ? "owner" : "")
		  << (!data.hasUser ? QByteArray() : "groups:" + data.groups.join(','))
		  << data.permissions.join(',')
		  << data.organizationPermissions.join(',')
		  << data.tenantPermissions.join(',')
		  << (data.hasBoundary ? "bounded:" + data.boundary.join(',') : QByteArray())
		  << (data.hasGrantingAuthority ? QByteArray(data.isGrantingAdmin ? "granting-admin:" : "granting:") + data.grantingPermissions.join(',') : QByteArray());
	return parts.join('\n');
}


// Diagnostic formatting for the existing field-level assertions, not a production authorization key.
enum FingerprintPart
{
	FP_ADMIN = 5,
	FP_GROUPS = 7,
	FP_PERMISSIONS = 8,
	FP_ORGANIZATION_PERMISSIONS = 9,
	FP_BOUNDARY = 11,
	FP_GRANTING = 12
};


QByteArray Part(const QByteArray& fingerprint, int part)
{
	return fingerprint.split('\n').value(part);
}


class CContextComp: public icomp::CComponentBase, public imtgql::CGqlContext
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CContextComp)
		I_REGISTER_INTERFACE(imtgql::IGqlContext);
	I_END_COMPONENT
};


template<class Component>
class TComponentFactory: public icomp::CSimComponentContextBase::ComponentsFactory
{
public:
	std::function<void(Component&)> configure;

	KeyList GetFactoryKeys() const override { return {QByteArray()}; }
	istd::TUniqueInterfacePtr<icomp::IComponent> CreateInstance(const QByteArray&) const override
	{
		auto* component = new icomp::TSimComponentWrap<Component>;
		component->InitComponent();
		if (configure){
			configure(*component);
		}
		return istd::TUniqueInterfacePtr<icomp::IComponent>(component);
	}
};


class CContextFactory: public icomp::CSimComponentContextBase::ComponentsFactory
{
public:
	KeyList GetFactoryKeys() const override { return {QByteArray()}; }
	istd::TUniqueInterfacePtr<icomp::IComponent> CreateInstance(const QByteArray&) const override
	{
		auto* component = new icomp::TSimComponentWrap<CContextComp>;
		component->InitComponent();
		return istd::TUniqueInterfacePtr<icomp::IComponent>(component);
	}
};


// A copy keeps the type, as the copy of a user component does; the type decides whether the user is the administrator
class CTestUser: public imtauth::CIdentifiableUserInfo
{
public:
	bool isAdmin = false;

	bool IsAdmin() const override { return isAdmin; }
	istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override
	{
		auto* clonePtr = new CTestUser;
		if (!clonePtr->CopyFrom(*this, mode)){
			delete clonePtr;
			return istd::IChangeableUniquePtr();
		}
		clonePtr->isAdmin = isAdmin;
		return istd::IChangeableUniquePtr(clonePtr);
	}
};


// The collection factory asks for IChangeable; keep the standalone simulation's normal user clone/lifetime.
class CCollectionUserComp: public imtauth::CUserInfoComp
{
public:
	typedef imtauth::CUserInfoComp BaseClass;
	I_BEGIN_COMPONENT(CCollectionUserComp)
		I_REGISTER_INTERFACE(istd::IChangeable);
	I_END_COMPONENT
};


// As strict as the SQL collections: a failed read is reported; a selection lists every document, as a collection ignoring the filter
class CDocumentCollectionComp:
			public icomp::CComponentBase,
			public imtbase::CObjectCollection,
			virtual public imtbase::IObjectCollectionReadStatus
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CDocumentCollectionComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
	I_END_COMPONENT

	QMap<QByteArray, std::shared_ptr<istd::IChangeable>> documents;
	QSet<QByteArray> unreadableIds;
	QSet<QByteArray> deniedIds;
	bool isReadable = true;
	mutable QMap<QByteArray, int> reads;
	mutable int selections = 0;
	std::function<void()> duringRead;

	template<class Document>
	Document& Get(const QByteArray& id) { return dynamic_cast<Document&>(*documents.value(id)); }

	int TotalReads() const
	{
		int retVal = 0;
		for (int count : reads){
			retVal += count;
		}
		return retVal;
	}

	bool GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet*) const override
	{
		++reads[objectId];
		if (duringRead){
			duringRead();
		}
		const auto it = documents.constFind(objectId);
		if (!isReadable || unreadableIds.contains(objectId) || it == documents.cend()){
			return false;
		}
		dataPtr.FromUnique(it.value()->CloneMe());
		return dataPtr.IsValid();
	}

	bool GetObjectDataWithStatus(const Id& objectId, DataPtr& dataPtr, bool& isAccessDenied, const iprm::IParamsSet* paramsPtr) const override
	{
		isAccessDenied = deniedIds.contains(objectId);
		if (isAccessDenied){
			++reads[objectId];
			return false;
		}
		return GetObjectData(objectId, dataPtr, paramsPtr);
	}

	QByteArray GetObjectTypeId(const Id& objectId) const override
	{
		const auto it = documents.constFind(objectId);
		if (it != documents.cend() && dynamic_cast<const imtauth::IUserInfo*>(it.value().get()) != nullptr){
			return "User";
		}
		return imtbase::CObjectCollection::GetObjectTypeId(objectId);
	}

	Ids GetElementIds(int, int, const iprm::IParamsSet*, ilog::IMessageConsumer* logPtr) const override
	{
		++selections;
		if (!isReadable){
			if (logPtr != nullptr){
				logPtr->AddMessage(ilog::IMessageConsumer::MessagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, "Read failed", "Test")));
			}
			return Ids();
		}
		return documents.keys();
	}
};


// The flat Tenants table accepts Id, not the DocumentId column of JSON document collections.
class CFlatTenantCollectionComp: public CDocumentCollectionComp
{
public:
	typedef CDocumentCollectionComp BaseClass;
	I_BEGIN_COMPONENT(CFlatTenantCollectionComp)
	I_END_COMPONENT

	mutable QByteArray selectedField;
	mutable QByteArray selectedId;

	Ids GetElementIds(int offset, int count, const iprm::IParamsSet* paramsPtr, ilog::IMessageConsumer* logPtr) const override
	{
		const Ids ids = BaseClass::GetElementIds(offset, count, paramsPtr, logPtr);
		iprm::TParamsPtr<imtbase::IComplexCollectionFilter> filterPtr(paramsPtr, "ComplexFilter", false);
		if (filterPtr.IsValid()){
			const auto& expression = filterPtr->GetFilterExpression();
			if (expression.fieldFilters.size() == 1 && expression.filterExpressions.isEmpty()){
				const auto& field = expression.fieldFilters.first();
				selectedField = field.fieldId;
				selectedId = field.filterValue.toString().toUtf8();
				if (field.fieldId == "Id" && field.filterOperation == imtbase::IComplexCollectionFilter::FO_EQUAL){
					return ids.contains(selectedId) ? Ids{selectedId} : Ids();
				}
			}
		}
		if (logPtr != nullptr){
			logPtr->AddMessage(ilog::IMessageConsumer::MessagePtr(new ilog::CMessage(
						istd::IInformationProvider::IC_ERROR, 0, "Unknown column or invalid filter for the flat Tenants table", "Test")));
		}
		return Ids();
	}
};


// The roles and groups as a product server resolves them for a request: read through its collections when asked for
class CRoleSource: virtual public imtauth::IRoleInfoProvider
{
public:
	const imtbase::IObjectCollection* collectionPtr = nullptr;

	const imtbase::ICollectionInfo& GetRoleList() const override { return m_list; }
	imtauth::IRoleUniquePtr GetRole(const QByteArray& roleId, const iprm::IParamsSet*) const override
	{
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (collectionPtr == nullptr || !collectionPtr->GetObjectData(roleId, dataPtr)){
			return imtauth::IRoleUniquePtr();
		}
		imtauth::IRoleUniquePtr retVal;
		retVal.MoveCastedPtr(dataPtr->CloneMe());
		imtauth::CRole* rolePtr = dynamic_cast<imtauth::CRole*>(retVal.GetPtr());
		if (rolePtr != nullptr){
			rolePtr->SetParentRoleProvider(this);
		}
		return retVal;
	}

private:
	imtbase::CCollectionInfo m_list;
};


class CGroupSource: virtual public imtauth::IUserGroupInfoProvider
{
public:
	const imtbase::IObjectCollection* collectionPtr = nullptr;
	const imtauth::IRoleInfoProvider* roleSourcePtr = nullptr;

	const imtbase::ICollectionInfo& GetUserGroupList() const override { return m_list; }
	imtauth::IUserGroupInfoSharedPtr GetUserGroup(const QByteArray& groupId, const iprm::IParamsSet*) const override
	{
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (collectionPtr == nullptr || !collectionPtr->GetObjectData(groupId, dataPtr)){
			return imtauth::IUserGroupInfoSharedPtr();
		}
		imtauth::CUserBaseInfo* groupPtr = dynamic_cast<imtauth::CUserBaseInfo*>(dataPtr.GetPtr());
		if (groupPtr != nullptr){
			groupPtr->SetRoleProvider(roleSourcePtr);
			groupPtr->SetUserGroupProvider(this);
		}
		imtauth::IUserGroupInfoSharedPtr retVal;
		retVal.SetCastedPtr(dataPtr);
		return retVal;
	}

private:
	imtbase::CCollectionInfo m_list;
};


class CSessionControllerComp: public icomp::CComponentBase, virtual public imtauth::IJwtSessionController
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CSessionControllerComp)
		I_REGISTER_INTERFACE(imtauth::IJwtSessionController);
	I_END_COMPONENT

	JwtState jwtState = JS_OK;
	bool isSessionAlive = true;
	bool isSessionChecked = true;
	std::function<void()> duringValidation;
	// The session record, as the token claims name it unless changed
	QByteArray sessionUserId = s_userId;
	QByteArray sessionTenantId;
	QByteArray sessionProductId = s_productId;
	bool isSessionReadable = true;
	mutable int validationCalls = 0;
	mutable int sessionChecks = 0;
	mutable int sessionReads = 0;

	bool ValidateSession(const QByteArray& sessionId, bool* isCheckedPtr) const override
	{
		++sessionChecks;
		if (isCheckedPtr != nullptr){
			*isCheckedPtr = isSessionChecked;
		}
		return isSessionChecked && isSessionAlive && sessionId == s_sessionId;
	}
	JwtState ValidateJwt(const QByteArray&) const override
	{
		++validationCalls;
		if (duringValidation){
			duringValidation();
		}
		return jwtState;
	}
	bool RefreshToken(const QByteArray&, UserSession&) const override { return false; }
	bool CreateNewSession(const QByteArray&, const QByteArray&, UserSession&) const override { return false; }
	imtauth::ISessionSharedPtr GetSession(const QByteArray& sessionId) const override
	{
		++sessionReads;
		if (!isSessionReadable || !isSessionAlive || sessionId != s_sessionId){
			return imtauth::ISessionSharedPtr();
		}
		auto* sessionPtr = new imtauth::CSessionInfo;
		sessionPtr->SetUserId(sessionUserId);
		sessionPtr->SetTenantId(sessionTenantId);
		sessionPtr->SetProductId(sessionProductId);
		sessionPtr->SetExpirationDate(QDateTime::currentDateTimeUtc().addSecs(3600));
		imtauth::ISessionSharedPtr retVal;
		retVal.SetPtr(sessionPtr);
		return retVal;
	}
	bool RemoveSession(const QByteArray&) const override { return false; }
	QByteArray GetUserFromJwt(const QByteArray&) const override { return QByteArray(); }
	QByteArray GetTenantFromJwt(const QByteArray&) const override { return QByteArray(); }
	QByteArray GetSessionFromJwt(const QByteArray&) const override { return QByteArray(); }
};


class CMembershipManagerComp: public icomp::CComponentBase, virtual public imtauth::ITenantMembershipManager
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CMembershipManagerComp)
		I_REGISTER_INTERFACE(imtauth::ITenantMembershipManager);
	I_END_COMPONENT

	QByteArrayList permissions{"/Organization/Read"};
	bool isPresent = true;
	bool isReadable = true;
	mutable int reads = 0;
	std::function<void()> duringRead;

	MembershipIds GetMembershipsByTenant(const QByteArray&) const override { return MembershipIds(); }
	MembershipIds GetMembershipsByUser(const QByteArray&, bool* isReadPtr) const override
	{
		if (isReadPtr != nullptr){
			*isReadPtr = isReadable;
		}
		return isReadable && isPresent ? MembershipIds{"membership"} : MembershipIds();
	}
	imtauth::ITenantMembershipUniquePtr GetMembership(const QByteArray&) const override { return FindMembership(s_userId, s_tenantId); }
	imtauth::ITenantMembershipUniquePtr FindMembership(const QByteArray& userId, const QByteArray& tenantId, bool* isReadPtr = nullptr) const override
	{
		++reads;
		if (duringRead){
			duringRead();
		}
		if (isReadPtr != nullptr){
			*isReadPtr = isReadable;
		}
		if (!isReadable || !isPresent){
			return imtauth::ITenantMembershipUniquePtr();
		}
		auto* membershipPtr = new imtauth::CTenantMembership;
		membershipPtr->SetUserId(userId);
		membershipPtr->SetTenantId(tenantId);
		membershipPtr->SetActive(true);
		membershipPtr->SetOrganizationPermissions(permissions);
		return imtauth::ITenantMembershipUniquePtr(membershipPtr);
	}
	QByteArray AddMembership(const QByteArray&, const QByteArray&) override { return QByteArray(); }
	bool RemoveMembership(const QByteArray&) override { return false; }
	bool UpdateMembershipOrganizationPermissions(const QByteArray&, const QByteArrayList&) override { return false; }
	bool IsMember(const QByteArray&, const QByteArray&) const override { return true; }
};


// The organization as the authorization server answers it: present, absent or not read
class CTenantManagerComp: public icomp::CComponentBase, virtual public imtauth::ITenantManager
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CTenantManagerComp)
		I_REGISTER_INTERFACE(imtauth::ITenantManager);
	I_END_COMPONENT

	QByteArrayList permissions{"/Read"};
	bool isPresent = true;
	bool isReadable = true;
	QByteArray ownerId = "owner";
	QByteArray creatorId;
	mutable int reads = 0;

	imtauth::ITenantInfoUniquePtr GetTenant(const QByteArray& tenantId, bool* isReadPtr) const override
	{
		++reads;
		if (isReadPtr != nullptr){
			*isReadPtr = isReadable;
		}
		if (!isReadable || !isPresent){
			return imtauth::ITenantInfoUniquePtr();
		}
		imtauth::ITenantInfoUniquePtr tenantPtr(new imtauth::CIdentifiableTenantInfo);
		tenantPtr->SetTenantId(tenantId);
		tenantPtr->SetOwnerId(ownerId);
		tenantPtr->SetCreatorId(creatorId);
		tenantPtr->SetTenantPermissions(permissions);
		return tenantPtr;
	}
	QByteArrayList GetTenantIds() const override { return QByteArrayList(); }
	QByteArrayList GetOwnedOrCreatedTenantIds(const QByteArray&, bool*) const override { return QByteArrayList(); }
	QByteArrayList GetChildTenantIds(const QByteArray&, bool*) const override { return QByteArrayList(); }
	QByteArrayList FindTenantIdsByName(const QString&) const override { return QByteArrayList(); }
	QByteArrayList GetTenantPermissions(const QByteArray&) const override { return permissions; }
	QByteArray CreateTenant(const QString&, const QString&, const QByteArray&, const QByteArray&, const QByteArray&) override { return QByteArray(); }
	bool RemoveTenant(const QByteArray&) override { return false; }
	bool UpdateTenant(const QByteArray&, const QString&, const QString&, const QByteArray&, bool) override { return false; }
	bool SetTenantActive(const QByteArray&, bool) override { return false; }
	bool SetTenantPermissions(const QByteArray&, const QByteArrayList&) override { return false; }
	bool SetTenantHierarchy(const QByteArray&, const QByteArray&) override { return false; }
	QByteArray GetSystemTenantId() const override { return QByteArray(); }
	bool EnsureSystemTenant() override { return false; }
};


class CTokenManagerStub: public imtauth::CPersonalAccessTokenManagerComp
{
public:
	QByteArrayList permissions{"/Read"};
	QByteArray productId;
	QByteArray clientProductId;
	bool isValid = true;
	bool isCheckFailed = false;

	bool ValidateToken(const QByteArray& raw, QByteArray& user, QByteArray& id, QByteArrayList& scopes,
				QByteArray* product, QByteArray* client, bool* failed) const override
	{
		user = clientProductId.isEmpty() ? s_userId : QByteArray();
		id = raw;
		scopes = permissions;
		if (product != nullptr){
			*product = productId;
		}
		if (client != nullptr){
			*client = clientProductId;
		}
		if (failed != nullptr){
			*failed = isCheckFailed;
		}
		return isValid && !isCheckFailed;
	}

	bool UpdateLastUsedAt(const QByteArray&) override { return true; }
};


// Keeps what a token is issued with
class CIssuingTokenManagerStub: public imtauth::CPersonalAccessTokenManagerComp
{
public:
	int created = 0;
	QByteArray createdProductId;

	TokenCreationResult CreateToken(const QByteArray&, const QByteArray& productId, const QString&, const QString&, const QByteArrayList&, const QDateTime&, const QByteArray&) override
	{
		++created;
		createdProductId = productId;
		return TokenCreationResult{"token-id", "imt_pat_issued", true};
	}
};


class CReceiverComp: public icomp::CComponentBase, virtual public imtservergql::IServerTokenReceiver
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CReceiverComp)
		I_REGISTER_INTERFACE(imtservergql::IServerTokenReceiver);
	I_END_COMPONENT

	imtauth::IPersonalAccessTokenManager* managerPtr = nullptr;

	QByteArray GetServerProductId() const override { return "Receiver"; }
	imtauth::IPersonalAccessTokenManager* GetTokenManager() const override { return managerPtr; }
	bool IsEnabled() const override { return true; }
};


// The sessions of the authorization server: a read fails or answers, and a selection lists other sessions too
class CSessionCollectionComp: public icomp::CComponentBase, public imtbase::CObjectCollection
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CSessionCollectionComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
	I_END_COMPONENT

	bool isPresent = true;
	bool isReadable = true;

	bool GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet*) const override
	{
		if (!isPresent || !isReadable || objectId != s_sessionId){
			return false;
		}
		auto* sessionPtr = new imtauth::CSessionInfo;
		sessionPtr->SetUserId(s_userId);
		sessionPtr->SetExpirationDate(QDateTime::currentDateTimeUtc().addSecs(3600));
		dataPtr.FromUnique(istd::IChangeableUniquePtr(sessionPtr));
		return true;
	}

	Ids GetElementIds(int, int, const iprm::IParamsSet*, ilog::IMessageConsumer* logPtr) const override
	{
		if (!isReadable){
			if (logPtr != nullptr){
				logPtr->AddMessage(ilog::IMessageConsumer::MessagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, "Read failed", "Test")));
			}
			return Ids();
		}
		Ids ids{"7d6c5b4a-3f2e-4d1c-9b8a-0f1e2d3c4b5a"};
		if (isPresent){
			ids << s_sessionId;
		}
		return ids;
	}
};


// The session controller of the authorization server, with the access token expired: only the session decides
class CExpiredJwtSessionControllerComp: public imtauth::CJwtSessionControllerComp
{
protected:
	JwtState ValidateJwt(const QByteArray&) const override { return JS_EXPIRED; }
};


// The network between a product server and the authorization server: a request reaches the GraphQL handler there
class CLoopbackGqlClientComp: public icomp::CComponentBase, virtual public imtclientgql::IGqlClient
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CLoopbackGqlClientComp)
		I_REGISTER_INTERFACE(imtclientgql::IGqlClient);
	I_END_COMPONENT

	const imtgql::IGqlRequestHandler* handlerPtr = nullptr;

	GqlResult SendRequest(GqlRequestPtr requestPtr, imtbase::IUrlParam*) const override
	{
		const imtgql::CGqlRequest* gqlRequestPtr = dynamic_cast<const imtgql::CGqlRequest*>(requestPtr.GetPtr());
		if (handlerPtr == nullptr || gqlRequestPtr == nullptr){
			return {{}, EC_INVALID_REQUEST, QStringLiteral("Invalid loopback request")};
		}

		// Shaped as the GraphQL servlet answers
		QString errorMessage;
		const QJsonObject result = handlerPtr->CreateResponse(*gqlRequestPtr, errorMessage);
		QJsonObject root;
		if (result.isEmpty()){
			root.insert("errors", QJsonArray{QJsonObject{{"message", errorMessage}}});
		}
		else{
			const QJsonValue data = result.contains("data") ? result.value("data") : QJsonValue(result);
			root.insert("data", QJsonObject{{QString::fromUtf8(gqlRequestPtr->GetCommandId()), data}});
		}

		auto* responsePtr = new imtgql::CGqlResponse(requestPtr);
		responsePtr->SetResponseData(QJsonDocument(root).toJson(QJsonDocument::Compact));
		GqlResponsePtr retVal;
		retVal.SetPtr(responsePtr);
		return {retVal, EC_NONE, {}};
	}

	bool SendRequestNoWait(GqlRequestPtr, imtbase::IUrlParam*) const override { return false; }
};


class CTestAuthenticationManager: public imtservergql::CAuthenticationManagerComp
{
public:
	void AccessChanged()
	{
		OnUpdate(nullptr, istd::IChangeable::ChangeSet(istd::IChangeable::CF_ANY));
	}
};


class CCompletedAsyncClientComp: public icomp::CComponentBase, virtual public imtclientgql::IAsyncGqlClient
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CCompletedAsyncClientComp)
		I_REGISTER_INTERFACE(imtclientgql::IAsyncGqlClient);
	I_END_COMPONENT

	ErrorCategory errorCategory = EC_NETWORK;
	mutable int requests = 0;

	QFuture<GqlResult> SendRequest(GqlRequestPtr, imtbase::IUrlParam*) const override
	{
		++requests;
		GqlResult result;
		result.errorCategory = errorCategory;
		result.errorMessage = QStringLiteral("Transport failed");
		return QtFuture::makeReadyValueFuture(result);
	}
};


class CLocalHttpProtocolComp: public icomp::CComponentBase, virtual public imtclientgql::IClientProtocolEngine
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CLocalHttpProtocolComp)
		I_REGISTER_INTERFACE(imtclientgql::IClientProtocolEngine);
	I_END_COMPONENT

	QUrl url;

	QNetworkRequest* CreateNetworkRequest(const imtgql::IGqlRequest&, imtbase::IUrlParam*) const override
	{
		auto* requestPtr = new QNetworkRequest(url);
		requestPtr->setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
		return requestPtr;
	}
};


// Which organization sees a role or a group
class CBindingManagerComp: public icomp::CComponentBase, virtual public imtauth::ITenantEntityBindingManager
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CBindingManagerComp)
		I_REGISTER_INTERFACE(imtauth::ITenantEntityBindingManager);
	I_END_COMPONENT

	QSet<QByteArray> bindings;
	bool isReadable = true;

	void Bind(const QByteArray& tenantId, const QByteArray& entityType, const QByteArray& entityId)
	{
		bindings.insert(tenantId + '/' + entityType + '/' + entityId);
	}

	bool AddBinding(const QByteArray&, const QByteArray&, const QByteArray&) override { return false; }
	bool RemoveBinding(const QByteArray&, const QByteArray&, const QByteArray&) override { return false; }
	bool RemoveAllBindingsForEntity(const QByteArray&, const QByteArray&) override { return false; }
	bool RemoveAllBindingsForTenant(const QByteArray&, const QByteArray&) override { return false; }
	bool HasBinding(const QByteArray& tenantId, const QByteArray& entityType, const QByteArray& entityId, bool* isReadPtr) const override
	{
		if (isReadPtr != nullptr){
			*isReadPtr = isReadable;
		}
		return isReadable && bindings.contains(tenantId + '/' + entityType + '/' + entityId);
	}
	QByteArrayList GetEntityIds(const QByteArray&, const QByteArray&) const override { return QByteArrayList(); }
	bool HasAnyTenantBinding(const QByteArray& entityType, const QByteArray& entityId, bool* isReadPtr) const override
	{
		if (isReadPtr != nullptr){
			*isReadPtr = isReadable;
		}
		for (const QByteArray& binding : bindings){
			if (isReadable && binding.endsWith('/' + entityType + '/' + entityId)){
				return true;
			}
		}
		return false;
	}
};


// The roles another organization delegates to the user
class CDelegatedAccessComp: public icomp::CComponentBase, virtual public imtauth::IDelegatedAccess
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CDelegatedAccessComp)
		I_REGISTER_INTERFACE(imtauth::IDelegatedAccess);
	I_END_COMPONENT

	QByteArrayList roleIds;
	bool isReadable = true;

	imtauth::DelegatedTenantList GetAccessibleTenants(const QByteArray&, const QByteArray&) const override { return imtauth::DelegatedTenantList(); }
	bool HasDelegatedRole(const QByteArray&, const QByteArray&, const QByteArray&, const QByteArray&) const override { return false; }
	QByteArrayList GetDelegatedRoles(const QByteArray&, const QByteArray&, bool* isReadPtr) const override
	{
		if (isReadPtr != nullptr){
			*isReadPtr = isReadable;
		}
		return isReadable ? roleIds : QByteArrayList();
	}
	bool IsDelegatedAccess(const QByteArray&, const QByteArray&, const QByteArray&) const override { return false; }
	QByteArrayList GetDelegatedUserRoles(const QByteArray&) const override { return QByteArrayList(); }
};


// The version of the authorization server: recent formats
class CVersionInfoComp: public icomp::CComponentBase, virtual public iser::IVersionInfo
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CVersionInfoComp)
		I_REGISTER_INTERFACE(iser::IVersionInfo);
	I_END_COMPONENT

	VersionIds GetVersionIds() const override { return VersionIds{imtcore::VI_IMTCORE}; }
	bool GetVersionNumber(int versionId, quint32& result) const override
	{
		result = 100000;
		return versionId == imtcore::VI_IMTCORE;
	}
	QString GetVersionIdDescription(int) const override { return QStringLiteral("ImtCore"); }
	QString GetEncodedVersionName(int, quint32 versionNumber) const override { return QString::number(versionNumber); }
};


// The input of the authorization server: a request is authenticated by its headers as its GraphQL servlet does, then handled
class CAuthorizationServerInputComp: public icomp::CComponentBase, virtual public imtclientgql::IGqlClient
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CAuthorizationServerInputComp)
		I_REGISTER_INTERFACE(imtclientgql::IGqlClient);
	I_END_COMPONENT

	const imtgql::IGqlContextCreator* authenticationPtr = nullptr;
	const imtgql::IGqlRequestHandler* handlerPtr = nullptr;
	// The credential of the product server
	QByteArray serverToken;
	QByteArray authenticationToken;
	mutable int requests = 0;
	mutable int userTokensSent = 0;
	mutable Creator::ContextCreationStatus inputStatus = Creator::CCS_OK;
	std::function<void(QJsonObject&)> changePayload;
	bool malformedReply = false;
	bool isUnavailable = false;
	mutable int handledRequests = 0;
	mutable QByteArrayList commands;
	std::function<void(const QByteArray&)> beforeRequest;

	GqlResult SendRequest(GqlRequestPtr requestPtr, imtbase::IUrlParam*) const override
	{
		++requests;
		commands << requestPtr->GetCommandId();
		if (beforeRequest){
			beforeRequest(requestPtr->GetCommandId());
		}
		if (isUnavailable){
			return {{}, EC_NETWORK, QStringLiteral("Authorization server unavailable")};
		}
		const imtgql::CGqlRequest* gqlRequestPtr = dynamic_cast<const imtgql::CGqlRequest*>(requestPtr.GetPtr());
		if (gqlRequestPtr == nullptr){
			return {{}, EC_INVALID_REQUEST, QStringLiteral("Invalid authorization request")};
		}

		// The headers the client engine sends
		imtgql::IGqlContext::Headers headers;
		const imtgql::IGqlContext* requestContextPtr = gqlRequestPtr->GetRequestContext();
		if (requestContextPtr != nullptr && !requestContextPtr->GetToken().isEmpty()){
			headers.insert(imtbase::s_authenticationTokenHeaderId, requestContextPtr->GetToken());
			++userTokensSent;
		}
		if (requestContextPtr != nullptr && !requestContextPtr->GetProductId().isEmpty()){
			headers.insert(imtbase::s_productIdHeaderId, requestContextPtr->GetProductId());
		}
		if (!serverToken.isEmpty()){
			headers.insert(imtbase::s_serverTokenHeaderId, serverToken);
		}
		if (!authenticationToken.isEmpty()){
			headers.insert(imtbase::s_authenticationTokenHeaderId, authenticationToken);
		}

		// Another server: the context of the asking thread is kept
		istd::IChangeableUniquePtr askingContextPtr;
		if (imtgql::CGqlRequestContextManager::GetContext() != nullptr){
			askingContextPtr = imtgql::CGqlRequestContextManager::GetContext()->CloneMe();
		}
		Creator::ContextCreationError error;
		imtgql::IGqlContextUniquePtr contextPtr = authenticationPtr->CreateGqlContext(headers, error);
		imtgql::CGqlRequestContextManager::SetContext(dynamic_cast<const imtgql::IGqlContext*>(askingContextPtr.GetPtr()));
		inputStatus = error.status;
		if (!contextPtr.IsValid()){
			const bool isAccessDenied = error.status == Creator::CCS_UNAUTHORIZED || error.status == Creator::CCS_FORBIDDEN;
			return {{}, isAccessDenied ? EC_ACCESS_DENIED : EC_INTERNAL, error.message};
		}

		imtgql::CGqlRequest request;
		qsizetype errorPosition = -1;
		if (!request.ParseQuery(gqlRequestPtr->GetQuery(), errorPosition)){
			return {{}, EC_INVALID_REQUEST, QStringLiteral("Invalid authorization query")};
		}
		request.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(contextPtr)));

		QString errorMessage;
		++handledRequests;
		const QJsonObject result = handlerPtr->CreateResponse(request, errorMessage);
		QJsonObject root;
		if (result.isEmpty()){
			root.insert("errors", QJsonArray{QJsonObject{{"message", errorMessage}}});
		}
		else{
			QJsonObject data = result.contains("data") ? result.value("data").toObject() : result;
			if (changePayload){
				changePayload(data);
			}
			root.insert("data", QJsonObject{{QString::fromUtf8(gqlRequestPtr->GetCommandId()), data}});
		}

		auto* responsePtr = new imtgql::CGqlResponse(requestPtr);
		responsePtr->SetResponseData(malformedReply ? QByteArray("{") : QJsonDocument(root).toJson(QJsonDocument::Compact));
		GqlResponsePtr retVal;
		retVal.SetPtr(responsePtr);
		return {retVal, EC_NONE, {}};
	}

	bool SendRequestNoWait(GqlRequestPtr, imtbase::IUrlParam*) const override { return false; }
};


// The user collection of the authorization server, handing out a user with its permissions
class COwnerUserControllerComp: public imtauthgql::CUserCollectionControllerComp
{
public:
	typedef imtauthgql::CUserCollectionControllerComp BaseClass;
	I_BEGIN_COMPONENT(COwnerUserControllerComp);
	I_END_COMPONENT;

	using BaseClass::CreateAdaptedObjectData;
};


// The users of a product server as its remote collection reads them: computed by the owner for the organization of the request
class CComputingUserSourceComp: public icomp::CComponentBase, public imtbase::CObjectCollection
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CComputingUserSourceComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
	I_END_COMPONENT

	const COwnerUserControllerComp* ownerPtr = nullptr;
	const CDocumentCollectionComp* storePtr = nullptr;
	mutable int reads = 0;
	// The owner refuses the read, as it does for a closed session
	bool isFailing = false;
	std::function<void()> duringRead;

	bool GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet*) const override
	{
		++reads;
		if (duringRead){
			duringRead();
		}
		if (isFailing){
			return false;
		}
		DataPtr storedPtr;
		if (!storePtr->GetObjectData(objectId, storedPtr, nullptr)){
			return false;
		}

		const imtgql::IGqlContext* currentPtr = imtgql::CGqlRequestContextManager::GetContext();
		imtgql::IGqlContextUniquePtr contextPtr(new imtgql::CGqlContext);
		if (currentPtr != nullptr){
			contextPtr->SetTenantId(currentPtr->GetTenantId());
			contextPtr->SetProductId(currentPtr->GetProductId());
		}
		imtgql::CGqlRequest request;
		request.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(contextPtr)));

		QString errorMessage;
		istd::IChangeableUniquePtr userPtr = ownerPtr->CreateAdaptedObjectData(objectId, *storedPtr, request, errorMessage);
		if (!userPtr.IsValid()){
			return false;
		}
		dataPtr.FromUnique(std::move(userPtr));
		return true;
	}

	Ids GetElementIds(int offset, int count, const iprm::IParamsSet* paramsPtr, ilog::IMessageConsumer* logPtr) const override
	{
		return storePtr->GetElementIds(offset, count, paramsPtr, logPtr);
	}
};


struct CRenewalFixture
{
	enum Option
	{
		O_RECEIVER = 1,
		// The users and the roles are read through caches, as on a product server
		O_CACHE = 2,
		// The sessions are checked at the authorization server, over the network
		O_REMOTE_SESSIONS = 4,
		// The roles have no collection: a check must not take them for absent
		O_NO_ROLE_COLLECTION = 8,
		// A product server whose users the authorization server computes; a session is authorized there, through its input
		O_COMPUTED_BY_SOURCE = 16,
		// Product authentication does not own membership/invitation services; the authorization owner still does.
		O_NO_PRODUCT_MEMBERSHIP = 64,
		// Read users through the real remote collection, delegate and authorization owner input.
		O_REMOTE_USERS = 128,
		O_MULTI_USER_TYPES = 256,
		// The requests of a session use an answer of the authorization server for some seconds
		O_SESSION_CACHE = 512,
		// The product server knows its product
		O_PRODUCT_INFO = 1024
	};

	CContextFactory factory;
	icomp::TSimSharedComponentPtr<CDocumentCollectionComp> users;
	icomp::TSimSharedComponentPtr<CDocumentCollectionComp> roles;
	icomp::TSimSharedComponentPtr<CDocumentCollectionComp> groups;
	icomp::TSimSharedComponentPtr<imtbase::CCachedObjectCollectionComp> cachedUsers;
	icomp::TSimSharedComponentPtr<imtbase::CCachedObjectCollectionComp> cachedRoles;
	CRoleSource roleSource;
	CGroupSource groupSource;
	icomp::TSimSharedComponentPtr<CSessionControllerComp> sessions;
	icomp::TSimSharedComponentPtr<CSessionControllerComp> productSessions;
	icomp::TSimSharedComponentPtr<CSessionCollectionComp> sessionStore;
	icomp::TSimSharedComponentPtr<CExpiredJwtSessionControllerComp> serverSessions;
	icomp::TSimSharedComponentPtr<imtauthgql::CGqlJwtSessionControllerComp> sessionHandler;
	icomp::TSimSharedComponentPtr<CLoopbackGqlClientComp> network;
	icomp::TSimSharedComponentPtr<imtauthgql::CRemoteJwtSessionControllerComp> remoteSessions;
	icomp::TSimSharedComponentPtr<CMembershipManagerComp> memberships;
	icomp::TSimSharedComponentPtr<CTenantManagerComp> tenants;
	icomp::TSimSharedComponentPtr<CTokenManagerStub> personalTokens;
	icomp::TSimSharedComponentPtr<CTokenManagerStub> serverTokens;
	icomp::TSimSharedComponentPtr<CReceiverComp> receiver;
	icomp::TSimSharedComponentPtr<CTestAuthenticationManager> authentication;
	icomp::TSimSharedComponentPtr<CBindingManagerComp> bindings;
	icomp::TSimSharedComponentPtr<CDelegatedAccessComp> delegations;
	icomp::TSimSharedComponentPtr<COwnerUserControllerComp> owner;
	icomp::TSimSharedComponentPtr<CComputingUserSourceComp> computedUsers;
	icomp::TSimSharedComponentPtr<CDocumentCollectionComp> serverRoles;
	icomp::TSimSharedComponentPtr<CDocumentCollectionComp> serverGroups;
	icomp::TSimSharedComponentPtr<CTokenManagerStub> ownerServerTokens;
	icomp::TSimSharedComponentPtr<CReceiverComp> ownerReceiver;
	icomp::TSimSharedComponentPtr<CTestAuthenticationManager> ownerAuthentication;
	icomp::TSimSharedComponentPtr<imtauthgql::CGqlJwtSessionControllerComp> sessionHandler2;
	icomp::TSimSharedComponentPtr<CAuthorizationServerInputComp> ownerInput;
	icomp::TSimSharedComponentPtr<CAuthorizationServerInputComp> userInput;
	icomp::TSimSharedComponentPtr<imtserverapp::CGqlObjectCollectionDelegateComp> userDelegate;
	icomp::TSimSharedComponentPtr<imtclientgql::CGqlObjectCollectionComp> remoteUsers;
	icomp::TSimSharedComponentPtr<imtauthgql::CRemoteJwtSessionControllerComp> remoteSessionProvider;
	// The organizations as the product server would read them itself
	icomp::TSimSharedComponentPtr<CTenantManagerComp> serverTenants;
	icomp::TSimSharedComponentPtr<CVersionInfoComp> versionInfo;
	icomp::TSimSharedComponentPtr<imtlic::CProductInfoComp> productInfo;
	TComponentFactory<imtauth::CUserInfoComp> userFactory;
	TComponentFactory<CCollectionUserComp> collectionUserFactory;
	Creator::ContextCreationStatus lastStatus = Creator::CCS_OK;

	explicit CRenewalFixture(int options = 0)
	{
		auto userPtr = std::make_shared<CTestUser>();
		userPtr->SetObjectUuid(s_userId);
		userPtr->SetId(s_userLogin);
		userPtr->SetEnabled(true);
		userPtr->SetRoles(s_productId, {s_roleId});
		userPtr->SetRoleProvider(&roleSource);
		userPtr->SetUserGroupProvider(&groupSource);
		users->documents.insert(s_userId, userPtr);
		AddRole(s_roleId, {"/Read"});
		users->InitComponent();
		roles->InitComponent();
		groups->InitComponent();

		const imtbase::IObjectCollection* userCollectionPtr = &*users;
		const imtbase::IObjectCollection* roleCollectionPtr = &*roles;
		if (options & O_CACHE){
			for (auto* cachePtr : {&cachedUsers, &cachedRoles}){
				(*cachePtr)->SetIntAttr("ObjectCacheLimit", 100);
				(*cachePtr)->SetIntAttr("MetaInfoCacheLimit", 100);
			}
			cachedUsers->SetRef("ObjectCollection", users);
			cachedUsers->InitComponent();
			cachedRoles->SetRef("ObjectCollection", roles);
			cachedRoles->InitComponent();
			userCollectionPtr = &*cachedUsers;
			roleCollectionPtr = &*cachedRoles;
		}
		roleSource.collectionPtr = roleCollectionPtr;
		groupSource.collectionPtr = &*groups;
		groupSource.roleSourcePtr = &roleSource;

		sessions->InitComponent();
		memberships->InitComponent();
		tenants->InitComponent();
		personalTokens->InitComponent();
		serverTokens->productId = "Receiver";
		serverTokens->clientProductId = "Client";
		serverTokens->InitComponent();
		receiver->managerPtr = &*serverTokens;
		receiver->InitComponent();

		if (options & O_COMPUTED_BY_SOURCE){
			bindings->InitComponent();
			delegations->InitComponent();
			owner->SetRef("ObjectCollection", users);
			owner->SetRef("RoleCollection", roles);
			owner->SetRef("UserGroupCollection", groups);
			owner->SetRef("BindingManager", bindings);
			owner->SetRef("MembershipManager", memberships);
			owner->SetRef("DelegatedAccess", delegations);
			if (options & O_REMOTE_USERS){
				versionInfo->InitComponent();
				owner->SetRef("VersionInfo", versionInfo);
				owner->SetIdAttr("CollectionId", "Users");
			}
			owner->InitComponent();
			computedUsers->ownerPtr = &*owner;
			computedUsers->storePtr = &*users;
			computedUsers->InitComponent();
			cachedUsers->SetIntAttr("ObjectCacheLimit", 100);
			cachedUsers->SetIntAttr("MetaInfoCacheLimit", 100);
			if (options & O_REMOTE_USERS){
				userDelegate->SetIdAttr("CollectionId", "Users");
				userDelegate->InitComponent();
				userInput->authenticationPtr = &*ownerAuthentication;
				userInput->handlerPtr = &*owner;
				userInput->serverToken = "imt_srv_product";
				userInput->InitComponent();
				remoteUsers->SetRef("ApiClient", userInput);
				remoteUsers->SetRef("GqlDatabaseDelegate", userDelegate);
				remoteUsers->InsertMultiAttr("TypeIds", QByteArray("User"));
				remoteUsers->InsertMultiAttr("TypeNames", QStringLiteral("User"));
				remoteUsers->InsertMultiFactory("ObjectFactories", &collectionUserFactory);
				if (options & O_MULTI_USER_TYPES){
					remoteUsers->InsertMultiAttr("TypeIds", QByteArray("OtherUser"));
					remoteUsers->InsertMultiAttr("TypeNames", QStringLiteral("Other user"));
					remoteUsers->InsertMultiFactory("ObjectFactories", &collectionUserFactory);
				}
				remoteUsers->InitComponent();
				cachedUsers->SetRef("ObjectCollection", remoteUsers);
			}
			else{
				cachedUsers->SetRef("ObjectCollection", computedUsers);
			}
			cachedUsers->InitComponent();
			serverRoles->InitComponent();
			serverGroups->InitComponent();

			// Its own roles and groups are wired, so reading them would show
			authentication->SetRef("UserCollection", cachedUsers);
			authentication->SetRef("RoleCollection", serverRoles);
			authentication->SetRef("UserGroupCollection", serverGroups);

			ownerServerTokens->productId = "Receiver";
			ownerServerTokens->clientProductId = s_productId;
			ownerServerTokens->InitComponent();
			ownerReceiver->managerPtr = &*ownerServerTokens;
			ownerReceiver->InitComponent();
			ownerAuthentication->SetRef("SlaveJwtSessionController", sessions);
			ownerAuthentication->SetRef("ServerTokenReceiver", ownerReceiver);
			if (options & O_REMOTE_USERS){
				ownerAuthentication->SetRef("PersonalAccessTokenManager", personalTokens);
				ownerAuthentication->SetRef("UserCollection", users);
				ownerAuthentication->SetRef("RoleCollection", roles);
				ownerAuthentication->SetRef("UserGroupCollection", groups);
				ownerAuthentication->SetRef("MembershipManager", memberships);
			}
			ownerAuthentication->SetFactory("GqlContextFactory", &factory);
			ownerAuthentication->InitComponent();
			sessionHandler2->SetRef("JwtSessionController", sessions);
			sessionHandler2->SetRef("AuthorizationSessionController", sessions);
			sessionHandler2->SetRef("UserCollection", users);
			sessionHandler2->SetRef("RoleCollection", roles);
			sessionHandler2->SetRef("UserGroupCollection", groups);
			sessionHandler2->SetRef("TenantManager", tenants);
			sessionHandler2->SetRef("BindingManager", bindings);
			sessionHandler2->SetRef("DelegatedAccess", delegations);
			sessionHandler2->SetRef("MembershipManager", memberships);
			sessionHandler2->SetRef("ServerTokenReceiver", ownerReceiver);
			if (!(options & O_REMOTE_USERS)){
				versionInfo->InitComponent();
			}
			sessionHandler2->SetRef("VersionInfo", versionInfo);
			sessionHandler2->InitComponent();
			ownerInput->authenticationPtr = &*ownerAuthentication;
			ownerInput->handlerPtr = &*sessionHandler2;
			ownerInput->serverToken = "imt_srv_product";
			ownerInput->InitComponent();
			remoteSessionProvider->SetRef("ApiClient", ownerInput);
			userFactory.configure = [this](imtauth::CUserInfoComp& user){
				user.SetRoleProvider(&roleSource);
				user.SetUserGroupProvider(&groupSource);
			};
			remoteSessionProvider->SetFactory("UserFactory", &userFactory);
			remoteSessionProvider->InitComponent();
			// Read by the product server itself, it would fail
			serverTenants->isReadable = false;
			serverTenants->InitComponent();
			authentication->SetRef("SessionAuthorizationProvider", remoteSessionProvider);
			if (!(options & O_SESSION_CACHE)){
				authentication->SetIntAttr("TenantDataCacheLifetime", 0);
			}
		}
		else if (options & O_CACHE){
			authentication->SetRef("UserCollection", cachedUsers);
			authentication->SetRef("RoleCollection", cachedRoles);
		}
		else{
			authentication->SetRef("UserCollection", users);
			if (!(options & O_NO_ROLE_COLLECTION)){
				authentication->SetRef("RoleCollection", roles);
			}
		}
		if (!(options & O_COMPUTED_BY_SOURCE)){
			authentication->SetRef("UserGroupCollection", groups);
		}
		if (options & O_COMPUTED_BY_SOURCE){
			// Any legacy preliminary token/session lookup would fail instead of hiding a second remote route.
			productSessions->jwtState = imtauth::IJwtSessionController::JS_NONE;
			productSessions->isSessionChecked = false;
			productSessions->isSessionReadable = false;
			productSessions->InitComponent();
			authentication->SetRef("SlaveJwtSessionController", productSessions);
		}
		else if (options & O_REMOTE_SESSIONS){
			sessionStore->InitComponent();
			serverSessions->SetRef("SessionCollection", sessionStore);
			serverSessions->InitComponent();
			sessionHandler->SetRef("JwtSessionController", serverSessions);
			sessionHandler->InitComponent();
			network->handlerPtr = &*sessionHandler;
			network->InitComponent();
			remoteSessions->SetRef("ApiClient", network);
			remoteSessions->InitComponent();
			authentication->SetRef("SlaveJwtSessionController", remoteSessions);
		}
		else{
			authentication->SetRef("SlaveJwtSessionController", sessions);
		}
		if (!(options & O_NO_PRODUCT_MEMBERSHIP)){
			authentication->SetRef("MembershipManager", memberships);
		}
		authentication->SetRef("TenantManager", (options & O_COMPUTED_BY_SOURCE) ? serverTenants : tenants);
		authentication->SetRef("PersonalAccessTokenManager", personalTokens);
		if (options & O_RECEIVER){
			authentication->SetRef("ServerTokenReceiver", receiver);
		}
		if (options & O_PRODUCT_INFO){
			productInfo->InitComponent();
			productInfo->SetProductId(s_productId);
			authentication->SetRef("FeatureContainer", productInfo);
		}
		authentication->SetFactory("GqlContextFactory", &factory);
		authentication->InitComponent();
	}

	CTestUser& User()
	{
		return users->Get<CTestUser>(s_userId);
	}

	imtauth::CRole& Role(const QByteArray& roleId)
	{
		return roles->Get<imtauth::CRole>(roleId);
	}

	void AddRole(const QByteArray& roleId, const QByteArrayList& permissions, const QByteArrayList& includedRoles = QByteArrayList())
	{
		auto rolePtr = std::make_shared<imtauth::CIdentifiableRoleInfo>();
		rolePtr->SetObjectUuid(roleId);
		rolePtr->SetRoleId(roleId);
		rolePtr->SetProductId(s_productId);
		rolePtr->SetLocalPermissions(permissions);
		for (const QByteArray& includedRoleId : includedRoles){
			rolePtr->IncludeRole(includedRoleId);
		}
		roles->documents.insert(roleId, rolePtr);
	}

	void AddGroup(const QByteArray& groupId, const QByteArray& productId, const QByteArrayList& roleIds, const QByteArrayList& parentGroups = QByteArrayList())
	{
		auto groupPtr = std::make_shared<imtauth::CIdentifiableUserGroupInfo>();
		groupPtr->SetObjectUuid(groupId);
		groupPtr->SetId(groupId);
		if (!roleIds.isEmpty()){
			groupPtr->SetRoles(productId, roleIds);
		}
		for (const QByteArray& parentGroupId : parentGroups){
			groupPtr->AddParentGroup(parentGroupId);
		}
		groups->documents.insert(groupId, groupPtr);
	}

	static QByteArray Jwt(const QByteArray& tenantId = QByteArray(), const QByteArray& productId = QByteArray(), qint64 expiresInSeconds = 3600)
	{
		QJsonObject payload;
		payload.insert("userId", QString::fromUtf8(s_userId));
		payload.insert("tenantId", QString::fromUtf8(tenantId));
		if (!productId.isEmpty()){
			payload.insert("productId", QString::fromUtf8(productId));
		}
		payload.insert("sessionId", QString::fromUtf8(s_sessionId));
		payload.insert("exp", double(QDateTime::currentSecsSinceEpoch() + expiresInSeconds));
		return "e30." + QJsonDocument(payload).toJson(QJsonDocument::Compact).toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals) + ".signature";
	}

	static imtgql::IGqlContext::Headers Headers(const QByteArray& token, const QByteArray& serverToken = QByteArray())
	{
		// A server names the product of its token or none
		imtgql::IGqlContext::Headers headers;
		if (!token.isEmpty()){
			headers.insert(imtbase::s_authenticationTokenHeaderId, token);
			headers.insert(imtbase::s_productIdHeaderId, s_productId);
		}
		if (!serverToken.isEmpty()){
			headers.insert(imtbase::s_serverTokenHeaderId, serverToken);
		}
		return headers;
	}

	Creator::AuthorizationState Renew(const imtgql::IGqlContext::Headers& headers, QByteArray* authorizationPtr = nullptr, bool isEstablished = true)
	{
		Creator::AuthorizationState state = Creator::AS_UNAVAILABLE;
		Creator::ContextCreationError error;
		const imtgql::IGqlContextUniquePtr contextPtr = authentication->RenewGqlContext(headers, isEstablished, state, error);
		lastStatus = error.status;
		if (state == Creator::AS_CONFIRMED){
			if (!contextPtr.IsValid()){
				return Creator::AS_UNAVAILABLE;
			}
			if (authorizationPtr != nullptr){
				*authorizationPtr = GetAuthorizationFingerprint(*contextPtr);
			}
		}
		else if (contextPtr.IsValid() || error.message.isEmpty()){
			// Only a confirmed check gives a context, every other one says why: no state matches this one
			return static_cast<Creator::AuthorizationState>(-1);
		}
		return state;
	}

	imtgql::IGqlContextUniquePtr RenewContext(const imtgql::IGqlContext::Headers& headers)
	{
		Creator::AuthorizationState state = Creator::AS_UNAVAILABLE;
		Creator::ContextCreationError error;
		imtgql::IGqlContextUniquePtr contextPtr = authentication->RenewGqlContext(headers, true, state, error);
		return state == Creator::AS_CONFIRMED ? std::move(contextPtr) : imtgql::IGqlContextUniquePtr();
	}

	// The permissions the owner hands out with the user for the organization; false when it hands out no user
	bool Compute(const QByteArray& tenantId, QByteArrayList& permissions, QString& errorMessage)
	{
		imtbase::IObjectCollection::DataPtr storedPtr;
		if (!users->GetObjectData(s_userId, storedPtr, nullptr)){
			return false;
		}
		imtgql::IGqlContextUniquePtr contextPtr(new imtgql::CGqlContext);
		contextPtr->SetTenantId(tenantId);
		contextPtr->SetProductId(s_productId);
		imtgql::CGqlRequest request;
		request.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(contextPtr)));

		const istd::IChangeableUniquePtr userPtr = owner->CreateAdaptedObjectData(s_userId, *storedPtr, request, errorMessage);
		const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(userPtr.GetPtr());
		if (userInfoPtr == nullptr){
			return false;
		}
		permissions = userInfoPtr->GetLocalPermissions(s_productId);
		std::sort(permissions.begin(), permissions.end());
		return true;
	}

	// The permissions of the user in the context of a request or of a subscription in the organization
	bool ServerPermissions(const QByteArray& tenantId, bool isSubscription, QByteArrayList& permissions)
	{
		const auto headers = Headers(Jwt(tenantId, s_productId));
		Creator::ContextCreationError error;
		imtgql::IGqlContextUniquePtr contextPtr;
		if (isSubscription){
			Creator::AuthorizationState state = Creator::AS_UNAVAILABLE;
			contextPtr = authentication->RenewGqlContext(headers, true, state, error);
			if (state != Creator::AS_CONFIRMED){
				return false;
			}
		}
		else{
			contextPtr = authentication->CreateGqlContext(headers, error);
			imtgql::CGqlRequestContextManager::Clear();
		}
		if (!contextPtr.IsValid() || contextPtr->GetUserInfo() == nullptr){
			return false;
		}
		// A member is given the access to its organization on top of what the user holds
		permissions.clear();
		for (const QByteArray& permission : contextPtr->GetUserInfo()->GetPermissions(s_productId)){
			if (!imtauth::GetOrganizationAccessPermissions().contains(permission)){
				permissions << permission;
			}
		}
		std::sort(permissions.begin(), permissions.end());
		return true;
	}

	Creator::ContextCreationStatus Create(const imtgql::IGqlContext::Headers& headers, QByteArray* authorizationPtr = nullptr)
	{
		Creator::ContextCreationError error;
		const imtgql::IGqlContextUniquePtr contextPtr = authentication->CreateGqlContext(headers, error);
		imtgql::CGqlRequestContextManager::Clear();
		if (contextPtr.IsValid() && authorizationPtr != nullptr){
			*authorizationPtr = GetAuthorizationFingerprint(*contextPtr);
		}
		return contextPtr.IsValid() ? Creator::CCS_OK : error.status;
	}
};


} // namespace


void CAuthorizationRenewalTest::RoleChangeIsDetected()
{
	CRenewalFixture fixture;
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt());

	QByteArray before;
	QCOMPARE(fixture.Renew(headers, &before), Creator::AS_CONFIRMED);
	QVERIFY(Part(before, FP_PERMISSIONS).contains("/Read"));
	QVERIFY(!Part(before, FP_PERMISSIONS).contains("/Write"));
	const istd::IChangeableUniquePtr userBeforePtr = fixture.User().CloneMe();
	QVERIFY(userBeforePtr.IsValid());

	// The role changes, the document of the user does not
	fixture.Role(s_roleId).SetLocalPermissions({"/Read", "/Write"});
	QVERIFY(fixture.User().IsEqual(*userBeforePtr));
	QByteArray after;
	QCOMPARE(fixture.Renew(headers, &after), Creator::AS_CONFIRMED);
	QVERIFY(Part(after, FP_PERMISSIONS).contains("/Write"));
	QVERIFY(before != after);

	QByteArray again;
	QCOMPARE(fixture.Renew(headers, &again), Creator::AS_CONFIRMED);
	QCOMPARE(again, after);
}


void CAuthorizationRenewalTest::GroupChangeIsDetected()
{
	CRenewalFixture fixture;
	fixture.AddGroup("group", s_productId, QByteArrayList());
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt());

	QByteArray before;
	QCOMPARE(fixture.Renew(headers, &before), Creator::AS_CONFIRMED);

	// A group that gives no permission: the objects shared with it open anyway
	fixture.User().AddToGroup("group");
	QByteArray after;
	QCOMPARE(fixture.Renew(headers, &after), Creator::AS_CONFIRMED);
	QCOMPARE(Part(after, FP_PERMISSIONS), Part(before, FP_PERMISSIONS));
	QVERIFY(Part(after, FP_GROUPS).contains("group"));
	QVERIFY(before != after);
}


void CAuthorizationRenewalTest::ParentGroupRoleChangeIsDetected()
{
	CRenewalFixture fixture;
	const QByteArray parentRoleId = "2c3d4e5f-6a7b-4c8d-9e0f-1a2b3c4d5e6f";
	fixture.AddRole(parentRoleId, {"/Reports"});
	fixture.AddGroup("parent", s_productId, {parentRoleId});
	fixture.AddGroup("group", s_productId, QByteArrayList(), {"parent"});
	fixture.User().AddToGroup("group");
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt());

	QByteArray before;
	QCOMPARE(fixture.Renew(headers, &before), Creator::AS_CONFIRMED);
	QVERIFY(Part(before, FP_PERMISSIONS).contains("/Reports"));

	// The role of the parent of the group changes; the user and the groups stay
	fixture.Role(parentRoleId).SetLocalPermissions({"/Reports", "/Export"});
	QByteArray after;
	QCOMPARE(fixture.Renew(headers, &after), Creator::AS_CONFIRMED);
	QVERIFY(Part(after, FP_PERMISSIONS).contains("/Export"));
	QCOMPARE(Part(after, FP_GROUPS), Part(before, FP_GROUPS));
}


void CAuthorizationRenewalTest::AdministratorIsKept()
{
	CRenewalFixture fixture;
	fixture.User().isAdmin = true;
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt());

	QByteArray before;
	QCOMPARE(fixture.Renew(headers, &before), Creator::AS_CONFIRMED);
	QCOMPARE(Part(before, FP_ADMIN), QByteArray("admin"));

	fixture.User().isAdmin = false;
	QByteArray after;
	QCOMPARE(fixture.Renew(headers, &after), Creator::AS_CONFIRMED);
	QCOMPARE(Part(after, FP_ADMIN), QByteArray("user"));
}


void CAuthorizationRenewalTest::ProductOfGroupIsKept()
{
	CRenewalFixture fixture;
	const QByteArray groupRoleId = "3d4e5f6a-7b8c-4d9e-0f1a-2b3c4d5e6f7a";
	fixture.AddRole(groupRoleId, {"/Reports/Read"});
	// The user holds the product only through the group, and the request names another one
	fixture.AddGroup("group", "Reports", {groupRoleId});
	fixture.User().AddToGroup("group");
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt());

	const imtgql::IGqlContextUniquePtr contextPtr = fixture.RenewContext(headers);
	QVERIFY(contextPtr.IsValid());
	const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
	QVERIFY(userInfoPtr->GetPermissions("Reports").contains("/Reports/Read"));
	QVERIFY(userInfoPtr->GetPermissions(s_productId).contains("/Read"));
	QVERIFY(!userInfoPtr->GetPermissions(s_productId).contains("/Reports/Read"));

	// Without a product too
	const imtauth::IUserBaseInfo::FeatureIds all = userInfoPtr->GetPermissions();
	QVERIFY(all.contains("/Reports/Read"));
	QVERIFY(all.contains("/Read"));

	// As a request resolves it through the providers
	QByteArray renewed;
	QCOMPARE(fixture.Renew(headers, &renewed), Creator::AS_CONFIRMED);
	QByteArray requested;
	QCOMPARE(fixture.Create(headers, &requested), Creator::CCS_OK);
	QCOMPARE(renewed, requested);
}


void CAuthorizationRenewalTest::MissedInvalidationIsRead()
{
	CRenewalFixture fixture(CRenewalFixture::O_CACHE);
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt());

	QByteArray before;
	QCOMPARE(fixture.Create(headers, &before), Creator::CCS_OK);

	// Changed at the source, and nobody told the caches
	fixture.Role(s_roleId).SetLocalPermissions({"/Read", "/Write"});
	QByteArray requested;
	QCOMPARE(fixture.Create(headers, &requested), Creator::CCS_OK);
	QCOMPARE(requested, before);
	QByteArray renewed;
	QCOMPARE(fixture.Renew(headers, &renewed), Creator::AS_CONFIRMED);
	QVERIFY(Part(renewed, FP_PERMISSIONS).contains("/Write"));

	fixture.User().SetEnabled(false);
	QCOMPARE(fixture.Create(headers), Creator::CCS_OK);
	QCOMPARE(fixture.Renew(headers), Creator::AS_DENIED);
}


void CAuthorizationRenewalTest::RoleAndGroupReadFailure_data()
{
	QTest::addColumn<int>("failure");
	QTest::addColumn<int>("expected");
	QTest::newRow("role-not-readable") << 0 << int(Creator::AS_UNAVAILABLE);
	QTest::newRow("role-removed") << 1 << int(Creator::AS_CONFIRMED);
	QTest::newRow("group-not-readable") << 2 << int(Creator::AS_UNAVAILABLE);
	QTest::newRow("group-removed") << 3 << int(Creator::AS_CONFIRMED);
	QTest::newRow("roles-not-configured") << 4 << int(Creator::AS_UNAVAILABLE);
}


void CAuthorizationRenewalTest::RoleAndGroupReadFailure()
{
	QFETCH(int, failure);
	QFETCH(int, expected);
	CRenewalFixture fixture(failure == 4 ? CRenewalFixture::O_NO_ROLE_COLLECTION : 0);
	const QByteArray groupRoleId = "3d4e5f6a-7b8c-4d9e-0f1a-2b3c4d5e6f7a";
	fixture.AddRole(groupRoleId, {"/Group/Read"});
	fixture.AddGroup("group", s_productId, {groupRoleId});
	fixture.User().AddToGroup("group");
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt());

	QByteArray before;
	if (failure != 4){
		QCOMPARE(fixture.Renew(headers, &before), Creator::AS_CONFIRMED);
		QVERIFY(Part(before, FP_PERMISSIONS).contains("/Group/Read"));
	}

	switch (failure){
	case 0:
		fixture.roles->unreadableIds.insert(groupRoleId);
		break;
	case 1:
		fixture.roles->documents.remove(groupRoleId);
		break;
	case 2:
		fixture.groups->unreadableIds.insert("group");
		break;
	case 3:
		fixture.groups->documents.remove("group");
		break;
	default:
		// The request path still resolves them through the providers of the user
		QCOMPARE(fixture.Create(headers), Creator::CCS_OK);
	}

	// A failed read is not an empty role; a removed one grants nothing
	QByteArray after;
	QCOMPARE(int(fixture.Renew(headers, &after)), expected);
	if (expected == Creator::AS_CONFIRMED){
		QVERIFY(!Part(after, FP_PERMISSIONS).contains("/Group/Read"));
		QVERIFY(Part(after, FP_PERMISSIONS).contains("/Read"));
	}
	else{
		QCOMPARE(fixture.lastStatus, Creator::CCS_INTERNAL_ERROR);
	}
}


void CAuthorizationRenewalTest::CyclesAreReadOnce()
{
	CRenewalFixture fixture;
	const QByteArray firstRoleId = "4e5f6a7b-8c9d-4e0f-1a2b-3c4d5e6f7a8b";
	const QByteArray secondRoleId = "5f6a7b8c-9d0e-4f1a-2b3c-4d5e6f7a8b9c";
	fixture.AddRole(firstRoleId, {"/First"}, {secondRoleId});
	fixture.AddRole(secondRoleId, {"/Second"}, {firstRoleId});
	fixture.AddGroup("ancestor", s_productId, {firstRoleId}, {"left"});
	fixture.AddGroup("left", s_productId, QByteArrayList(), {"ancestor"});
	fixture.AddGroup("right", s_productId, {secondRoleId}, {"ancestor"});
	fixture.User().AddToGroup("left");
	fixture.User().AddToGroup("right");
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt());

	fixture.roles->reads.clear();
	fixture.groups->reads.clear();
	QByteArray renewed;
	QCOMPARE(fixture.Renew(headers, &renewed), Creator::AS_CONFIRMED);
	QVERIFY(Part(renewed, FP_PERMISSIONS).contains("/First"));
	QVERIFY(Part(renewed, FP_PERMISSIONS).contains("/Second"));

	// Each one read once, whatever the cycles and the common ancestors
	for (const QByteArray& groupId : {QByteArray("ancestor"), QByteArray("left"), QByteArray("right")}){
		QCOMPARE(fixture.groups->reads.value(groupId), 1);
	}
	for (const QByteArray& roleId : {s_roleId, firstRoleId, secondRoleId}){
		QCOMPARE(fixture.roles->reads.value(roleId), 1);
	}
}


void CAuthorizationRenewalTest::ContextOutlivesSnapshot()
{
	CRenewalFixture fixture;
	const QByteArray groupRoleId = "3d4e5f6a-7b8c-4d9e-0f1a-2b3c4d5e6f7a";
	fixture.AddRole(groupRoleId, {"/Group/Read"});
	fixture.AddGroup("group", s_productId, {groupRoleId});
	fixture.User().AddToGroup("group");
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt());

	// The snapshot is gone once the check returns: the context holds what it resolved
	const imtgql::IGqlContextUniquePtr contextPtr = fixture.RenewContext(headers);
	QVERIFY(contextPtr.IsValid());
	const QByteArray fingerprint = GetAuthorizationFingerprint(*contextPtr);
	fixture.users->reads.clear();
	fixture.roles->reads.clear();
	fixture.groups->reads.clear();

	const istd::IChangeableUniquePtr copyPtr = contextPtr->CloneMe();
	const imtgql::IGqlContext* copiedContextPtr = dynamic_cast<const imtgql::IGqlContext*>(copyPtr.GetPtr());
	QVERIFY(copiedContextPtr != nullptr);
	QCOMPARE(GetAuthorizationFingerprint(*contextPtr), fingerprint);
	QCOMPARE(GetAuthorizationFingerprint(*copiedContextPtr), fingerprint);
	QVERIFY(contextPtr->GetUserInfo()->GetPermissions(s_productId).contains("/Group/Read"));
	QVERIFY(contextPtr->GetUserInfo()->GetRoleProvider() == nullptr);
	QVERIFY(contextPtr->GetUserInfo()->GetUserGroupProvider() == nullptr);
	QCOMPARE(fixture.users->TotalReads() + fixture.roles->TotalReads() + fixture.groups->TotalReads(), 0);
}


void CAuthorizationRenewalTest::OrganizationPermissionsAreCompared()
{
	CRenewalFixture fixture;
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt(s_tenantId));

	QByteArray before;
	QCOMPARE(fixture.Renew(headers, &before), Creator::AS_CONFIRMED);
	QVERIFY(before.contains("/Organization/Read"));

	fixture.memberships->permissions = {"/Organization/Read", "/Organization/Manage"};
	QByteArray after;
	QCOMPARE(fixture.Renew(headers, &after), Creator::AS_CONFIRMED);
	QVERIFY(before != after);
}


void CAuthorizationRenewalTest::TokenBoundariesAreCompared_data()
{
	QTest::addColumn<int>("kind");
	QTest::newRow("personal-token") << 0;
	QTest::newRow("server-on-its-own") << 1;
	QTest::newRow("user-through-server") << 2;
}


void CAuthorizationRenewalTest::TokenBoundariesAreCompared()
{
	QFETCH(int, kind);
	const bool server = kind != 0;
	CRenewalFixture fixture(server ? CRenewalFixture::O_RECEIVER : 0);
	fixture.Role(s_roleId).SetLocalPermissions({"/Read", "/Write"});
	imtgql::IGqlContext::Headers headers;
	if (kind == 0){
		headers = CRenewalFixture::Headers("imt_pat_user");
	}
	else{
		// The client product names itself; its user comes with a session opened through it
		headers.insert(imtbase::s_productIdHeaderId, "Client");
		if (kind == 1){
			headers.insert(imtbase::s_authenticationTokenHeaderId, "imt_srv_client");
		}
		else{
			headers.insert(imtbase::s_authenticationTokenHeaderId, CRenewalFixture::Jwt(QByteArray(), "Client"));
			headers.insert(imtbase::s_serverTokenHeaderId, "imt_srv_client");
		}
	}
	CTokenManagerStub& tokens = server ? *fixture.serverTokens : *fixture.personalTokens;
	tokens.permissions = {"/Read", "/Write"};

	QByteArray before;
	QCOMPARE(fixture.Renew(headers, &before), Creator::AS_CONFIRMED);
	QVERIFY(before.contains("bounded:/Read,/Write"));

	// Narrowed at its store: the renewal does not take the cached token
	QCOMPARE(fixture.Create(headers), Creator::CCS_OK);
	tokens.permissions = {"/Read"};
	QByteArray after;
	QCOMPARE(fixture.Renew(headers, &after), Creator::AS_CONFIRMED);
	QCOMPARE(Part(after, FP_BOUNDARY), QByteArray("bounded:/Read"));
	QVERIFY(before != after);

	tokens.isValid = false;
	QCOMPARE(fixture.Renew(headers), Creator::AS_DENIED);
	tokens.isValid = true;
	tokens.isCheckFailed = true;
	QCOMPARE(fixture.Renew(headers), Creator::AS_UNAVAILABLE);
}


void CAuthorizationRenewalTest::ExpiredAccessToken_data()
{
	QTest::addColumn<bool>("established");
	QTest::addColumn<bool>("alive");
	QTest::addColumn<bool>("checked");
	QTest::addColumn<int>("expected");
	QTest::newRow("session-alive") << true << true << true << int(Creator::AS_CONFIRMED);
	QTest::newRow("session-closed") << true << false << true << int(Creator::AS_DENIED);
	QTest::newRow("session-not-checked") << true << true << false << int(Creator::AS_UNAVAILABLE);
	QTest::newRow("new-subscription") << false << true << true << int(Creator::AS_DENIED);
}


void CAuthorizationRenewalTest::ExpiredAccessToken()
{
	QFETCH(bool, established);
	QFETCH(bool, alive);
	QFETCH(bool, checked);
	QFETCH(int, expected);
	CRenewalFixture fixture;
	fixture.sessions->jwtState = imtauth::IJwtSessionController::JS_EXPIRED;
	fixture.sessions->isSessionAlive = alive;
	fixture.sessions->isSessionChecked = checked;
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt());

	QCOMPARE(int(fixture.Renew(headers, nullptr, established)), expected);
	if (!established){
		// The client refreshes its token, as for a request
		QCOMPARE(fixture.lastStatus, Creator::CCS_UNAUTHORIZED);
	}

	// A new request with the same token is refused whatever the session: the client has to refresh it
	QCOMPARE(fixture.Create(headers), Creator::CCS_UNAUTHORIZED);
}


void CAuthorizationRenewalTest::SessionCheckCrossesNetwork_data()
{
	QTest::addColumn<int>("session");
	QTest::addColumn<int>("expected");
	QTest::newRow("alive") << 0 << int(Creator::AS_CONFIRMED);
	QTest::newRow("closed") << 1 << int(Creator::AS_DENIED);
	QTest::newRow("store-not-readable") << 2 << int(Creator::AS_UNAVAILABLE);
}


void CAuthorizationRenewalTest::SessionCheckCrossesNetwork()
{
	QFETCH(int, session);
	QFETCH(int, expected);
	CRenewalFixture fixture(CRenewalFixture::O_REMOTE_SESSIONS);
	fixture.sessionStore->isPresent = session != 1;
	fixture.sessionStore->isReadable = session != 2;
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt());

	// The access token expired: the session at the authorization server decides, through its handler and the network
	QCOMPARE(int(fixture.Renew(headers)), expected);
}


void CAuthorizationRenewalTest::UnavailableIsNotDenied_data()
{
	QTest::addColumn<int>("failure");
	QTest::addColumn<int>("expected");
	QTest::newRow("session-check-failed") << 0 << int(Creator::AS_UNAVAILABLE);
	QTest::newRow("token-invalid") << 1 << int(Creator::AS_DENIED);
	QTest::newRow("user-not-readable") << 2 << int(Creator::AS_UNAVAILABLE);
	QTest::newRow("user-removed") << 3 << int(Creator::AS_DENIED);
	QTest::newRow("user-disabled") << 4 << int(Creator::AS_DENIED);
}


void CAuthorizationRenewalTest::UnavailableIsNotDenied()
{
	QFETCH(int, failure);
	QFETCH(int, expected);
	CRenewalFixture fixture;
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt());
	QCOMPARE(fixture.Renew(headers), Creator::AS_CONFIRMED);

	switch (failure){
	case 0:
		fixture.sessions->jwtState = imtauth::IJwtSessionController::JS_NONE;
		break;
	case 1:
		fixture.sessions->jwtState = imtauth::IJwtSessionController::JS_INVALID;
		break;
	case 2:
		fixture.users->isReadable = false;
		break;
	case 3:
		fixture.users->documents.remove(s_userId);
		break;
	default:
		fixture.User().SetEnabled(false);
	}

	QCOMPARE(int(fixture.Renew(headers)), expected);
}


void CAuthorizationRenewalTest::OrganizationReadFailure_data()
{
	QTest::addColumn<int>("failure");
	QTest::addColumn<bool>("isInOrganization");
	QTest::addColumn<int>("expected");
	QTest::newRow("tenant-not-readable") << 0 << true << int(Creator::AS_UNAVAILABLE);
	QTest::newRow("tenant-removed") << 1 << true << int(Creator::AS_CONFIRMED);
	QTest::newRow("membership-not-readable") << 2 << true << int(Creator::AS_UNAVAILABLE);
	QTest::newRow("membership-removed") << 3 << true << int(Creator::AS_CONFIRMED);
	QTest::newRow("memberships-not-readable") << 2 << false << int(Creator::AS_UNAVAILABLE);
	QTest::newRow("memberships-removed") << 3 << false << int(Creator::AS_CONFIRMED);
}


void CAuthorizationRenewalTest::OrganizationReadFailure()
{
	QFETCH(int, failure);
	QFETCH(bool, isInOrganization);
	QFETCH(int, expected);
	CRenewalFixture fixture;
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt(isInOrganization ? s_tenantId : QByteArray()));

	QByteArray before;
	QCOMPARE(fixture.Renew(headers, &before), Creator::AS_CONFIRMED);

	switch (failure){
	case 0:
		fixture.tenants->isReadable = false;
		break;
	case 1:
		fixture.tenants->isPresent = false;
		break;
	case 2:
		fixture.memberships->isReadable = false;
		break;
	default:
		fixture.memberships->isPresent = false;
	}

	// A failed read says nothing; what the store answered is a change of the access
	QByteArray after;
	QCOMPARE(int(fixture.Renew(headers, &after)), expected);
	if (expected == Creator::AS_CONFIRMED){
		QVERIFY(before != after);
	}
}


void CAuthorizationRenewalTest::UnreadOrganizationIsNotCached()
{
	CRenewalFixture fixture;
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt(s_tenantId));

	QByteArray complete;
	QCOMPARE(fixture.Create(headers, &complete), Creator::CCS_OK);
	fixture.authentication->AccessChanged();

	// A request goes on without the organization data it could not read: it holds no more than with all of it, and keeps nothing
	fixture.memberships->isReadable = false;
	QByteArray incomplete;
	QCOMPARE(fixture.Create(headers, &incomplete), Creator::CCS_OK);
	QVERIFY(Part(incomplete, FP_ORGANIZATION_PERMISSIONS).isEmpty());
	QVERIFY(!Part(complete, FP_ORGANIZATION_PERMISSIONS).isEmpty());
	for (const QByteArray& permission : Part(incomplete, FP_PERMISSIONS).split(',')){
		QVERIFY2(Part(complete, FP_PERMISSIONS).split(',').contains(permission), permission.constData());
	}
	const int readsAfterFailure = fixture.memberships->reads;

	fixture.memberships->isReadable = true;
	QCOMPARE(fixture.Create(headers), Creator::CCS_OK);
	QCOMPARE(fixture.memberships->reads, readsAfterFailure + 1);
	QCOMPARE(fixture.Create(headers), Creator::CCS_OK);
	QCOMPARE(fixture.memberships->reads, readsAfterFailure + 1);
}


void CAuthorizationRenewalTest::ChangeDuringCheckIsStale()
{
	CRenewalFixture fixture;
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt());

	// Even a refusal read while the access changed is not used
	fixture.sessions->jwtState = imtauth::IJwtSessionController::JS_INVALID;
	fixture.sessions->duringValidation = [&](){ fixture.authentication->AccessChanged(); };
	QCOMPARE(fixture.Renew(headers), Creator::AS_STALE);

	fixture.sessions->jwtState = imtauth::IJwtSessionController::JS_OK;
	QCOMPARE(fixture.Renew(headers), Creator::AS_STALE);

	fixture.sessions->duringValidation = {};
	QCOMPARE(fixture.Renew(headers), Creator::AS_CONFIRMED);
}


void CAuthorizationRenewalTest::StaleReadDoesNotFillCache()
{
	CRenewalFixture fixture;
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt(s_tenantId));

	fixture.memberships->duringRead = [&](){ fixture.authentication->AccessChanged(); };
	QCOMPARE(fixture.Create(headers), Creator::CCS_OK);
	fixture.memberships->duringRead = {};
	const int readsAfterStale = fixture.memberships->reads;

	// The organization data read before the change is not kept: the next request reads it again, and keeps that
	QCOMPARE(fixture.Create(headers), Creator::CCS_OK);
	QCOMPARE(fixture.memberships->reads, readsAfterStale + 1);
	QCOMPARE(fixture.Create(headers), Creator::CCS_OK);
	QCOMPARE(fixture.memberships->reads, readsAfterStale + 1);

	// A change drops what was kept
	fixture.authentication->AccessChanged();
	QCOMPARE(fixture.Create(headers), Creator::CCS_OK);
	QCOMPARE(fixture.memberships->reads, readsAfterStale + 2);
}


void CAuthorizationRenewalTest::GrantingAuthorityOfRequest_data()
{
	QTest::addColumn<int>("kind");
	QTest::addColumn<bool>("isSet");
	QTest::addColumn<bool>("isAdmin");
	QTest::addColumn<QByteArrayList>("granting");
	QTest::addColumn<QByteArrayList>("held");
	QTest::newRow("user-through-server") << 0 << true << false << QByteArrayList{"/Read", "/Write"} << QByteArrayList{"/Read"};
	QTest::newRow("superuser-through-server") << 1 << true << true << QByteArrayList() << QByteArrayList{"/Read"};
	QTest::newRow("personal-token-through-server") << 2 << true << false << QByteArrayList{"/Read"} << QByteArrayList{"/Read"};
	QTest::newRow("server-on-its-own") << 3 << false << false << QByteArrayList() << QByteArrayList{"/Read"};
	QTest::newRow("user-at-this-server") << 4 << false << false << QByteArrayList() << QByteArrayList{"/Read", "/Write"};
}


void CAuthorizationRenewalTest::GrantingAuthorityOfRequest()
{
	QFETCH(int, kind);
	QFETCH(bool, isSet);
	QFETCH(bool, isAdmin);
	QFETCH(QByteArrayList, granting);
	QFETCH(QByteArrayList, held);
	CRenewalFixture fixture(kind == 4 ? 0 : CRenewalFixture::O_RECEIVER);
	// No organization access: it adds permissions of its own
	fixture.memberships->isPresent = false;
	fixture.Role(s_roleId).SetLocalPermissions({"/Read", "/Write"});
	fixture.User().SetRoles("Client", {s_roleId});
	fixture.User().isAdmin = kind == 1;
	fixture.serverTokens->permissions = {"/Read"};
	fixture.personalTokens->permissions = {"/Read"};
	fixture.personalTokens->productId = "Client";

	imtgql::IGqlContext::Headers headers;
	if (kind == 4){
		headers = CRenewalFixture::Headers(CRenewalFixture::Jwt());
	}
	else{
		headers.insert(imtbase::s_productIdHeaderId, "Client");
		headers.insert(imtbase::s_authenticationTokenHeaderId, kind == 3 ? QByteArray("imt_srv_client") : (kind == 2 ? QByteArray("imt_pat_user") : CRenewalFixture::Jwt(QByteArray(), "Client")));
		if (kind != 3){
			headers.insert(imtbase::s_serverTokenHeaderId, "imt_srv_client");
		}
	}

	// A request, a new subscription and an established one are built alike
	for (const int built : {0, 1, 2}){
		Creator::ContextCreationError error;
		Creator::AuthorizationState state = Creator::AS_UNAVAILABLE;
		const imtgql::IGqlContextUniquePtr contextPtr = built != 0
					? fixture.authentication->RenewGqlContext(headers, built == 2, state, error)
					: fixture.authentication->CreateGqlContext(headers, error);
		imtgql::CGqlRequestContextManager::Clear();
		QVERIFY2(contextPtr.IsValid(), qPrintable(error.message));

		bool isGrantingAdmin = false;
		QByteArrayList grantingPermissions;
		QCOMPARE(contextPtr->GetGrantingAuthority(isGrantingAdmin, grantingPermissions), isSet);
		if (isSet){
			QCOMPARE(isGrantingAdmin, isAdmin);
			if (!isAdmin){
				std::sort(grantingPermissions.begin(), grantingPermissions.end());
				QCOMPARE(grantingPermissions, granting);
			}
		}

		// What the request may do stays bounded by the tokens; a superuser through a server is none here
		QByteArrayList heldPermissions = imtservergql::GetUserPermissions(*contextPtr);
		std::sort(heldPermissions.begin(), heldPermissions.end());
		QCOMPARE(heldPermissions, held);
		QVERIFY(contextPtr->GetUserInfo() != nullptr);
		QCOMPARE(contextPtr->GetUserInfo()->IsAdmin(), false);
	}
}


void CAuthorizationRenewalTest::GrantedPermissionsAreChecked_data()
{
	QTest::addColumn<int>("authority");
	QTest::addColumn<QByteArrayList>("permissions");
	QTest::addColumn<bool>("isTenant");
	QTest::addColumn<bool>("expected");
	QTest::newRow("within-authority") << 0 << QByteArrayList{"/Write"} << false << true;
	QTest::newRow("beyond-authority") << 0 << QByteArrayList{"/Delete"} << false << false;
	QTest::newRow("administrator") << 1 << QByteArrayList{"/Delete"} << false << true;
	QTest::newRow("empty-authority-no-fallback") << 2 << QByteArrayList{"/Read"} << false << false;
	QTest::newRow("no-authority-held") << 3 << QByteArrayList{"/Read"} << false << true;
	QTest::newRow("no-authority-not-held") << 3 << QByteArrayList{"/Write"} << false << false;
	QTest::newRow("organization") << 0 << QByteArrayList{"/Delete"} << true << true;
}


void CAuthorizationRenewalTest::GrantedPermissionsAreChecked()
{
	QFETCH(int, authority);
	QFETCH(QByteArrayList, permissions);
	QFETCH(bool, isTenant);
	QFETCH(bool, expected);

	// The user may do '/Read'; what it may give is set apart, or not
	imtgql::CGqlContext context;
	context.SetProductId(s_productId);
	imtauth::CIdentifiableUserInfo user;
	user.SetLocalPermissions(s_productId, {"/Read"});
	context.SetUserInfo(&user);
	if (isTenant){
		context.SetTenantId(s_tenantId);
	}
	if (authority == 0){
		context.SetGrantingAuthority(false, {"/Read", "/Write"});
	}
	else if (authority == 1){
		context.SetGrantingAuthority(true, QByteArrayList());
	}
	else if (authority == 2){
		context.SetGrantingAuthority(false, QByteArrayList());
	}

	QString errorMessage;
	QCOMPARE(imtauthgql::CheckGrantedPermissions(&context, QByteArrayList(), permissions, errorMessage), expected);
	QCOMPARE(errorMessage.isEmpty(), expected);

	// What a role held already is not given again
	QVERIFY(imtauthgql::CheckGrantedPermissions(&context, permissions, permissions, errorMessage));
}


void CAuthorizationRenewalTest::GrantingAuthorityIsCopied_data()
{
	QTest::addColumn<bool>("isAdmin");
	QTest::addColumn<QByteArrayList>("permissions");
	QTest::newRow("permissions") << false << QByteArrayList{"/Read", "/Write"};
	QTest::newRow("administrator") << true << QByteArrayList();
	QTest::newRow("explicitly-empty") << false << QByteArrayList();
}


void CAuthorizationRenewalTest::GrantingAuthorityIsCopied()
{
	QFETCH(bool, isAdmin);
	QFETCH(QByteArrayList, permissions);
	imtgql::CGqlContext context;
	context.SetGrantingAuthority(isAdmin, permissions);

	const istd::IChangeableUniquePtr clonePtr = context.CloneMe();
	imtgql::CGqlContext copy;
	QVERIFY(copy.CopyFrom(context));
	for (const imtgql::IGqlContext* copiedContextPtr : {dynamic_cast<const imtgql::IGqlContext*>(clonePtr.GetPtr()), static_cast<const imtgql::IGqlContext*>(&copy)}){
		QVERIFY(copiedContextPtr != nullptr);
		bool copiedIsAdmin = !isAdmin;
		QByteArrayList copiedPermissions{"/Other"};
		QVERIFY(copiedContextPtr->GetGrantingAuthority(copiedIsAdmin, copiedPermissions));
		QCOMPARE(copiedIsAdmin, isAdmin);
		QCOMPARE(copiedPermissions, permissions);
	}

	QVERIFY(copy.ResetData());
	bool resetIsAdmin = true;
	QByteArrayList resetPermissions{"/Other"};
	QVERIFY(!copy.GetGrantingAuthority(resetIsAdmin, resetPermissions));
	QCOMPARE(resetIsAdmin, false);
	QVERIFY(resetPermissions.isEmpty());
}


void CAuthorizationRenewalTest::GrantingAuthorityChangeIsDetected_data()
{
	QTest::addColumn<bool>("isAdministrator");
	QTest::newRow("permission-beyond-token-removed") << false;
	QTest::newRow("administrator-removed") << true;
}


void CAuthorizationRenewalTest::GrantingAuthorityChangeIsDetected()
{
	QFETCH(bool, isAdministrator);
	CRenewalFixture fixture(CRenewalFixture::O_RECEIVER);
	fixture.Role(s_roleId).SetLocalPermissions({"/Read", "/Write"});
	fixture.User().SetRoles("Client", {s_roleId});
	fixture.User().isAdmin = isAdministrator;
	fixture.serverTokens->permissions = {"/Read"};
	imtgql::IGqlContext::Headers headers;
	headers.insert(imtbase::s_productIdHeaderId, "Client");
	headers.insert(imtbase::s_authenticationTokenHeaderId, CRenewalFixture::Jwt(QByteArray(), "Client"));
	headers.insert(imtbase::s_serverTokenHeaderId, "imt_srv_client");

	QByteArray before;
	QCOMPARE(fixture.Renew(headers, &before), Creator::AS_CONFIRMED);

	// Only what the user may give in its product changes: what the request may do stays within the token
	if (isAdministrator){
		fixture.User().isAdmin = false;
	}
	else{
		fixture.Role(s_roleId).SetLocalPermissions({"/Read"});
	}
	QByteArray after;
	QCOMPARE(fixture.Renew(headers, &after), Creator::AS_CONFIRMED);
	QCOMPARE(Part(after, FP_PERMISSIONS), Part(before, FP_PERMISSIONS));
	QCOMPARE(Part(after, FP_ADMIN), Part(before, FP_ADMIN));
	QVERIFY(Part(before, FP_GRANTING) != Part(after, FP_GRANTING));
	QVERIFY(before != after);
}


void CAuthorizationRenewalTest::PersonalTokenThroughServer_data()
{
	QTest::addColumn<int>("product");
	QTest::addColumn<QByteArrayList>("scopes");
	QTest::addColumn<bool>("isThroughServer");
	QTest::addColumn<bool>("expected");
	QTest::addColumn<QByteArray>("stored");
	QTest::newRow("no-product") << 0 << QByteArrayList{"/Write"} << true << true << QByteArray("ProLife");
	QTest::newRow("empty-product") << 1 << QByteArrayList{"/Write"} << true << true << QByteArray("ProLife");
	QTest::newRow("same-product") << 2 << QByteArrayList{"/Write"} << true << true << QByteArray("ProLife");
	QTest::newRow("other-product") << 3 << QByteArrayList{"/Write"} << true << false << QByteArray();
	QTest::newRow("scope-beyond-authority") << 2 << QByteArrayList{"/Delete"} << true << false << QByteArray();
	QTest::newRow("at-this-server-no-product") << 0 << QByteArrayList{"/Write"} << false << true << QByteArray();
}


void CAuthorizationRenewalTest::PersonalTokenThroughServer()
{
	QFETCH(int, product);
	QFETCH(QByteArrayList, scopes);
	QFETCH(bool, isThroughServer);
	QFETCH(bool, expected);
	QFETCH(QByteArray, stored);

	icomp::TSimSharedComponentPtr<CIssuingTokenManagerStub> tokens;
	tokens->InitComponent();
	icomp::TSimSharedComponentPtr<imtauthgql::CPersonalAccessTokenControllerComp> controller;
	controller->SetRef("PersonalAccessTokenManager", tokens);
	controller->InitComponent();

	sdl::V1_0::imtauth::CreateTokenRequestArguments arguments;
	arguments.input.Emplace();
	arguments.input->userId = s_userId;
	if (product == 1){
		arguments.input->productId = QByteArray();
	}
	else if (product == 2){
		arguments.input->productId = QByteArray("ProLife");
	}
	else if (product == 3){
		arguments.input->productId = QByteArray("Lisa");
	}
	arguments.input->name = QStringLiteral("token");
	imtsdl::TElementList<QByteArray> scopeList;
	scopeList.FromList(scopes);
	arguments.input->scopes = scopeList;
	arguments.input->expiresAt = QStringLiteral("2099-12-31T23:59:59Z");
	imtgql::CGqlRequest gqlRequest;
	QVERIFY(sdl::V1_0::imtauth::CCreateTokenGqlRequest::SetupGqlRequest(gqlRequest, arguments));

	// The user issues a token for itself: through the server of ProLife what it may give is set apart
	imtgql::IGqlContextUniquePtr contextPtr(new imtgql::CGqlContext);
	contextPtr->SetUserId(s_userId);
	contextPtr->SetProductId("ProLife");
	imtauth::CIdentifiableUserInfo user;
	user.SetObjectUuid(s_userId);
	user.SetId(s_userLogin);
	user.SetLocalPermissions("ProLife", {"/Write"});
	contextPtr->SetUserInfo(&user);
	if (isThroughServer){
		contextPtr->SetGrantingAuthority(false, {"/Read", "/Write"});
	}
	gqlRequest.SetGqlContext(std::move(contextPtr));

	QString errorMessage;
	const QJsonObject result = controller->CreateResponse(gqlRequest, errorMessage);
	const QJsonObject payload = result.value("data").toObject();
	QCOMPARE(payload.value("success").toBool(), expected);
	QCOMPARE(tokens->created, expected ? 1 : 0);
	if (expected){
		QCOMPARE(tokens->createdProductId, stored);
	}
}


void CAuthorizationRenewalTest::PersonalTokenWorksInItsProductOnly()
{
	// A token bound to ProLife, as one issued through its server
	CRenewalFixture fixture;
	fixture.personalTokens->productId = "ProLife";
	fixture.User().SetRoles("ProLife", {s_roleId});

	imtgql::IGqlContext::Headers headers;
	headers.insert(imtbase::s_authenticationTokenHeaderId, "imt_pat_user");
	headers.insert(imtbase::s_productIdHeaderId, "ProLife");
	QCOMPARE(fixture.Create(headers), Creator::CCS_OK);

	headers.insert(imtbase::s_productIdHeaderId, "Lisa");
	QCOMPARE(fixture.Create(headers), Creator::CCS_FORBIDDEN);
	QCOMPARE(fixture.Renew(headers), Creator::AS_DENIED);
}


void CAuthorizationRenewalTest::ComputedUserFollowsOrganization_data()
{
	QTest::addColumn<QByteArrayList>("tenantIds");

	QTest::newRow("A, B, A") << QByteArrayList{s_tenantId, s_otherTenantId, s_tenantId};
	QTest::newRow("B, A, B") << QByteArrayList{s_otherTenantId, s_tenantId, s_otherTenantId};
}


void CAuthorizationRenewalTest::ComputedUserFollowsOrganization()
{
	QFETCH(QByteArrayList, tenantIds);

	// One user, a role in each organization; each order starts with cold caches
	CRenewalFixture fixture(CRenewalFixture::O_COMPUTED_BY_SOURCE);
	const QByteArray roleOfA = "2a3b4c5d-6e7f-4a8b-9c0d-1e2f3a4b5c6d";
	const QByteArray roleOfB = "3b4c5d6e-7f8a-4b9c-8d1e-2f3a4b5c6d7e";
	fixture.AddRole(roleOfA, {"/A"});
	fixture.AddRole(roleOfB, {"/B"});
	fixture.serverRoles->documents = fixture.roles->documents;
	fixture.bindings->Bind(s_tenantId, "Roles", roleOfA);
	fixture.bindings->Bind(s_otherTenantId, "Roles", roleOfB);
	fixture.User().SetRoles(s_productId, {roleOfA, roleOfB});
	fixture.tenants->permissions = {"/A", "/B"};

	for (const QByteArray& tenantId : tenantIds){
		const QByteArrayList expected = {tenantId == s_tenantId ? "/A" : "/B"};
		QByteArrayList computed;
		QString errorMessage;
		QVERIFY2(fixture.Compute(tenantId, computed, errorMessage), qPrintable(errorMessage));
		QCOMPARE(computed, expected);

		// A request and a subscription ask the authorization server afresh
		fixture.sessions->sessionTenantId = tenantId;
		for (const bool isSubscription : {false, true}){
			const int requestsBefore = fixture.ownerInput->requests;
			QByteArrayList held;
			QVERIFY(fixture.ServerPermissions(tenantId, isSubscription, held));
			QCOMPARE(held, expected);
			QVERIFY(fixture.ownerInput->requests > requestsBefore);
		}
	}

	// The server takes the permissions as they come: its roles and groups are not read
	QCOMPARE(fixture.serverRoles->TotalReads(), 0);
	QCOMPARE(fixture.serverGroups->TotalReads(), 0);
}


void CAuthorizationRenewalTest::InvisibleAssignmentsGrantNothing()
{
	// The group is visible in the organization; its parent group, and the role its role includes, are not
	CRenewalFixture fixture(CRenewalFixture::O_COMPUTED_BY_SOURCE);
	const QByteArray groupId = "4c5d6e7f-8a9b-4c0d-9e1f-3a4b5c6d7e8f";
	const QByteArray parentGroupId = "5d6e7f8a-9b0c-4d1e-8f2a-4b5c6d7e8f9a";
	const QByteArray groupRoleId = "6e7f8a9b-0c1d-4e2f-9a3b-5c6d7e8f9a0b";
	const QByteArray includedRoleId = "7f8a9b0c-1d2e-4f3a-8b4c-6d7e8f9a0b1c";
	const QByteArray parentRoleId = "8a9b0c1d-2e3f-4a4b-9c5d-7e8f9a0b1c2d";
	fixture.AddRole(includedRoleId, {"/Secret"});
	fixture.AddRole(groupRoleId, {}, {includedRoleId});
	fixture.AddRole(parentRoleId, {"/Hidden"});
	fixture.AddGroup(parentGroupId, s_productId, {parentRoleId});
	fixture.AddGroup(groupId, s_productId, {groupRoleId}, {parentGroupId});
	fixture.serverRoles->documents = fixture.roles->documents;
	fixture.serverGroups->documents = fixture.groups->documents;
	fixture.bindings->Bind(s_tenantId, "UserGroups", groupId);
	fixture.bindings->Bind(s_tenantId, "Roles", groupRoleId);
	fixture.bindings->Bind(s_tenantId, "Roles", parentRoleId);
	fixture.bindings->Bind(s_otherTenantId, "Roles", includedRoleId);
	fixture.bindings->Bind(s_otherTenantId, "UserGroups", parentGroupId);
	fixture.User().SetRoles(s_productId, {});
	fixture.User().AddToGroup(groupId);
	// The organization allows them: only the assignments decide
	fixture.tenants->permissions = {"/Secret", "/Hidden"};
	fixture.sessions->sessionTenantId = s_tenantId;

	QByteArrayList computed;
	QString errorMessage;
	QVERIFY2(fixture.Compute(s_tenantId, computed, errorMessage), qPrintable(errorMessage));
	QVERIFY(computed.isEmpty());

	for (const bool isSubscription : {false, true}){
		QByteArrayList held{"unset"};
		QVERIFY(fixture.ServerPermissions(s_tenantId, isSubscription, held));
		QCOMPARE(held, computed);
	}
}


void CAuthorizationRenewalTest::SessionCheckAfterFailedRead_data()
{
	QTest::addColumn<bool>("isSessionChecked");
	QTest::addColumn<bool>("isSessionAlive");
	QTest::addColumn<int>("status");

	// A cached collection tells no failure: an unread session is not a refusal, a session alive leaves the user unavailable
	QTest::newRow("session not read") << false << true << int(Creator::CCS_INTERNAL_ERROR);
	QTest::newRow("session closed") << true << false << int(Creator::CCS_UNAUTHORIZED);
	QTest::newRow("session alive") << true << true << int(Creator::CCS_FORBIDDEN);
}


void CAuthorizationRenewalTest::SessionCheckAfterFailedRead()
{
	QFETCH(bool, isSessionChecked);
	QFETCH(bool, isSessionAlive);
	QFETCH(int, status);

	// The user is not read; the session is looked at only then, once the token was accepted
	CRenewalFixture fixture(CRenewalFixture::O_CACHE);
	const std::function<void()> closeSession = [&fixture, isSessionChecked, isSessionAlive](){
		fixture.sessions->jwtState = imtauth::IJwtSessionController::JS_EXPIRED;
		fixture.sessions->isSessionChecked = isSessionChecked;
		fixture.sessions->isSessionAlive = isSessionAlive;
	};
	fixture.users->unreadableIds.insert(s_userId);
	fixture.users->duringRead = closeSession;

	// A session store that failed does not make a closed session
	QCOMPARE(int(fixture.Create(CRenewalFixture::Headers(CRenewalFixture::Jwt(s_tenantId)))), status);
}


void CAuthorizationRenewalTest::SessionAuthorizationConfirmsExpiredToken()
{
	CRenewalFixture fixture(CRenewalFixture::O_COMPUTED_BY_SOURCE);
	fixture.bindings->Bind(s_tenantId, "Roles", s_roleId);
	fixture.tenants->permissions = {"/Read", "/Write"};
	fixture.sessions->sessionTenantId = s_tenantId;
	const QByteArray expiredToken = CRenewalFixture::Jwt(s_tenantId, s_productId, -60);
	const auto headers = CRenewalFixture::Headers(expiredToken);

	// The access token expired, its session lives: an established subscription is confirmed
	fixture.sessions->jwtState = imtauth::IJwtSessionController::JS_EXPIRED;
	QByteArray before;
	QCOMPARE(fixture.Renew(headers, &before), Creator::AS_CONFIRMED);
	QVERIFY(Part(before, FP_PERMISSIONS).contains("/Read"));

	// The user and its organization came in one answer: nothing was asked with the token of the user, nor read here
	QVERIFY(fixture.ownerInput->requests > 0);
	QCOMPARE(fixture.ownerInput->handledRequests, fixture.ownerInput->requests);
	QCOMPARE(fixture.ownerInput->userTokensSent, 0);
	QCOMPARE(fixture.computedUsers->reads, 0);
	QCOMPARE(fixture.productSessions->validationCalls, 0);
	QCOMPARE(fixture.productSessions->sessionChecks, 0);
	QCOMPARE(fixture.productSessions->sessionReads, 0);
	QCOMPARE(fixture.serverTenants->reads, 0);
	QVERIFY(fixture.tenants->reads > 0);

	// A change of the role is seen
	fixture.Role(s_roleId).SetLocalPermissions({"/Read", "/Write"});
	QByteArray after;
	QCOMPARE(fixture.Renew(headers, &after), Creator::AS_CONFIRMED);
	QVERIFY(Part(after, FP_PERMISSIONS).contains("/Write"));
	QVERIFY(after != before);

	fixture.sessions->isSessionChecked = false;
	QCOMPARE(fixture.Renew(headers), Creator::AS_UNAVAILABLE);
	QCOMPARE(fixture.lastStatus, Creator::CCS_INTERNAL_ERROR);
	fixture.sessions->isSessionChecked = true;

	// Without the established subscription the operation refuses it too, asking for a new authentication
	const imtauth::ISessionAuthorizationProvider::SessionAuthorization refused = fixture.remoteSessionProvider->GetSessionAuthorization(expiredToken, s_productId, false);
	QCOMPARE(refused.state, imtauth::ISessionAuthorizationProvider::SAS_REFUSED);
	QCOMPARE(refused.refusalReason, imtauth::ISessionAuthorizationProvider::RR_REAUTHENTICATE);

	// The expired token opens no request and no new subscription, before or after
	QCOMPARE(fixture.Create(headers), Creator::CCS_UNAUTHORIZED);
	QCOMPARE(fixture.Renew(headers, nullptr, false), Creator::AS_DENIED);
	QCOMPARE(fixture.lastStatus, Creator::CCS_UNAUTHORIZED);

	// The session is closed: refused
	fixture.sessions->isSessionAlive = false;
	QCOMPARE(fixture.Renew(headers), Creator::AS_DENIED);
	QCOMPARE(fixture.lastStatus, Creator::CCS_UNAUTHORIZED);
	QCOMPARE(fixture.computedUsers->reads, 0);
	QCOMPARE(fixture.serverTenants->reads, 0);
	QCOMPARE(fixture.serverRoles->TotalReads(), 0);
	QCOMPARE(fixture.serverGroups->TotalReads(), 0);
}


void CAuthorizationRenewalTest::SessionAuthorizationRefusals_data()
{
	QTest::addColumn<QByteArray>("refusal");

	QTest::newRow("no server token") << QByteArray("no server token");
	QTest::newRow("revoked server token") << QByteArray("revoked server token");
	QTest::newRow("revoked server token with cached input") << QByteArray("revoked server token with cached input");
	QTest::newRow("token of another product") << QByteArray("another product");
	QTest::newRow("personal access token") << QByteArray("personal access token");
	QTest::newRow("another user in the session") << QByteArray("another user");
	QTest::newRow("another organization in the session") << QByteArray("another organization");
	QTest::newRow("another product in the session") << QByteArray("another session product");
	QTest::newRow("session opened at owner") << QByteArray("owner session");
	QTest::newRow("server token issued for another receiver") << QByteArray("another receiver");
	QTest::newRow("server token without client product") << QByteArray("missing client product");
	QTest::newRow("server token in authentication header") << QByteArray("server token in authentication header");
}


void CAuthorizationRenewalTest::SessionAuthorizationRefusals()
{
	QFETCH(QByteArray, refusal);

	CRenewalFixture fixture(CRenewalFixture::O_COMPUTED_BY_SOURCE);
	QByteArray accessToken = CRenewalFixture::Jwt(QByteArray(), s_productId);
	auto ask = [&fixture, &accessToken](){
		return fixture.remoteSessionProvider->GetSessionAuthorization(accessToken, s_productId, true);
	};
	{
		const imtauth::ISessionAuthorizationProvider::SessionAuthorization computed = ask();
		QVERIFY2(computed.state == imtauth::ISessionAuthorizationProvider::SAS_COMPUTED, qPrintable(computed.message + QStringLiteral(" / input %1").arg(int(fixture.ownerInput->inputStatus))));
	}

	if (refusal == "no server token"){
		fixture.ownerInput->serverToken.clear();
	}
	else if (refusal == "revoked server token" || refusal == "revoked server token with cached input"){
		fixture.ownerServerTokens->isValid = false;
	}
	else if (refusal == "another product"){
		fixture.ownerServerTokens->clientProductId = "Other";
	}
	else if (refusal == "personal access token"){
		accessToken = "imt_pat_personal";
	}
	else if (refusal == "another user"){
		fixture.sessions->sessionUserId = "4f3e2d1c-0b9a-4876-9543-210fedcba987";
	}
	else if (refusal == "another organization"){
		fixture.sessions->sessionTenantId = s_otherTenantId;
	}
	else if (refusal == "another session product"){
		fixture.sessions->sessionProductId = "Other";
	}
	else if (refusal == "owner session"){
		accessToken = CRenewalFixture::Jwt();
		fixture.sessions->sessionProductId.clear();
	}
	else if (refusal == "server token in authentication header"){
		fixture.ownerInput->authenticationToken = fixture.ownerInput->serverToken;
	}
	else if (refusal == "missing client product"){
		fixture.ownerServerTokens->clientProductId.clear();
	}
	else{
		fixture.ownerServerTokens->productId = "OtherReceiver";
	}

	if (refusal != "revoked server token with cached input"){
		fixture.ownerAuthentication->AccessChanged();
	}

	const int readsBefore = fixture.users->TotalReads();
	const int handledBefore = fixture.ownerInput->handledRequests;
	const imtauth::ISessionAuthorizationProvider::SessionAuthorization authorization = ask();
	const bool isRefusedAtInput = refusal == "no server token" || refusal == "revoked server token"
				|| refusal == "another product" || refusal == "another receiver" || refusal == "missing client product";
	if (isRefusedAtInput){
		QCOMPARE(fixture.ownerInput->inputStatus, Creator::CCS_FORBIDDEN);
		QCOMPARE(fixture.ownerInput->handledRequests, handledBefore);
		// The separate HTTP denial result is propagated; no handler payload is fabricated.
		QCOMPARE(authorization.state, imtauth::ISessionAuthorizationProvider::SAS_REFUSED);
		QCOMPARE(authorization.refusalReason, imtauth::ISessionAuthorizationProvider::RR_FORBIDDEN);
		const auto headers = CRenewalFixture::Headers(accessToken);
		QCOMPARE(fixture.Create(headers), Creator::CCS_FORBIDDEN);
		QCOMPARE(fixture.Renew(headers, nullptr, false), Creator::AS_DENIED);
		QCOMPARE(fixture.Renew(headers), Creator::AS_DENIED);
	}
	else{
		QCOMPARE(fixture.ownerInput->inputStatus, Creator::CCS_OK);
		QCOMPARE(fixture.ownerInput->handledRequests, handledBefore + 1);
		QCOMPARE(authorization.state, imtauth::ISessionAuthorizationProvider::SAS_REFUSED);
		QCOMPARE(authorization.refusalReason, imtauth::ISessionAuthorizationProvider::RR_FORBIDDEN);
	}
	QVERIFY(!authorization.userPtr.IsValid());
	QCOMPARE(fixture.users->TotalReads(), readsBefore);
}


void CAuthorizationRenewalTest::SessionAuthorizationOutcomes_data()
{
	QTest::addColumn<QByteArray>("change");
	QTest::addColumn<int>("requestStatus");
	QTest::addColumn<int>("subscriptionState");

	QTest::newRow("disabled user") << QByteArray("disabled user") << int(Creator::CCS_FORBIDDEN) << int(Creator::AS_DENIED);
	QTest::newRow("absent user") << QByteArray("absent user") << int(Creator::CCS_FORBIDDEN) << int(Creator::AS_DENIED);
	QTest::newRow("absent organization") << QByteArray("absent organization") << int(Creator::CCS_FORBIDDEN) << int(Creator::AS_DENIED);
	QTest::newRow("organization not read") << QByteArray("organization not read") << int(Creator::CCS_INTERNAL_ERROR) << int(Creator::AS_UNAVAILABLE);
	QTest::newRow("session not read") << QByteArray("session not read") << int(Creator::CCS_INTERNAL_ERROR) << int(Creator::AS_UNAVAILABLE);
	QTest::newRow("session check unavailable") << QByteArray("session check unavailable") << int(Creator::CCS_INTERNAL_ERROR) << int(Creator::AS_UNAVAILABLE);
	QTest::newRow("revoked session with unexpired JWT") << QByteArray("revoked session") << int(Creator::CCS_UNAUTHORIZED) << int(Creator::AS_DENIED);
	QTest::newRow("user not read") << QByteArray("user not read") << int(Creator::CCS_INTERNAL_ERROR) << int(Creator::AS_UNAVAILABLE);
	QTest::newRow("role not read") << QByteArray("role not read") << int(Creator::CCS_INTERNAL_ERROR) << int(Creator::AS_UNAVAILABLE);
	QTest::newRow("group not read") << QByteArray("group not read") << int(Creator::CCS_INTERNAL_ERROR) << int(Creator::AS_UNAVAILABLE);
	QTest::newRow("binding not read") << QByteArray("binding not read") << int(Creator::CCS_INTERNAL_ERROR) << int(Creator::AS_UNAVAILABLE);
	QTest::newRow("membership not read") << QByteArray("membership not read") << int(Creator::CCS_INTERNAL_ERROR) << int(Creator::AS_UNAVAILABLE);
	QTest::newRow("delegation not read") << QByteArray("delegation not read") << int(Creator::CCS_INTERNAL_ERROR) << int(Creator::AS_UNAVAILABLE);
	QTest::newRow("network unavailable") << QByteArray("network unavailable") << int(Creator::CCS_INTERNAL_ERROR) << int(Creator::AS_UNAVAILABLE);
}


void CAuthorizationRenewalTest::SessionAuthorizationOutcomes()
{
	QFETCH(QByteArray, change);
	QFETCH(int, requestStatus);
	QFETCH(int, subscriptionState);

	CRenewalFixture fixture(CRenewalFixture::O_COMPUTED_BY_SOURCE);
	const QByteArray tenantId = change == "delegation not read" ? s_otherTenantId : s_tenantId;
	const QByteArray groupId = "4c5d6e7f-8a9b-4c0d-9e1f-3a4b5c6d7e8f";
	fixture.bindings->Bind(tenantId, "Roles", s_roleId);
	if (change == "group not read"){
		fixture.AddGroup(groupId, s_productId, {s_roleId});
		fixture.bindings->Bind(tenantId, "UserGroups", groupId);
		fixture.User().SetRoles(s_productId, {});
		fixture.User().AddToGroup(groupId);
	}
	fixture.sessions->sessionTenantId = tenantId;
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt(tenantId, s_productId));
	QCOMPARE(fixture.Create(headers), Creator::CCS_OK);
	QCOMPARE(fixture.Renew(headers), Creator::AS_CONFIRMED);

	if (change == "disabled user"){
		fixture.User().SetEnabled(false);
	}
	else if (change == "absent user"){
		fixture.users->documents.remove(s_userId);
	}
	else if (change == "absent organization"){
		fixture.tenants->isPresent = false;
	}
	else if (change == "organization not read"){
		fixture.tenants->isReadable = false;
	}
	else if (change == "session not read"){
		fixture.sessions->isSessionReadable = false;
	}
	else if (change == "session check unavailable"){
		fixture.sessions->isSessionChecked = false;
	}
	else if (change == "revoked session"){
		fixture.sessions->isSessionAlive = false;
	}
	else if (change == "user not read"){
		fixture.users->unreadableIds.insert(s_userId);
	}
	else if (change == "role not read"){
		fixture.roles->unreadableIds.insert(s_roleId);
	}
	else if (change == "group not read"){
		fixture.groups->unreadableIds.insert(groupId);
	}
	else if (change == "binding not read"){
		fixture.bindings->isReadable = false;
	}
	else if (change == "membership not read"){
		fixture.memberships->isReadable = false;
	}
	else if (change == "network unavailable"){
		fixture.ownerInput->isUnavailable = true;
	}
	else{
		fixture.delegations->isReadable = false;
	}

	// The answer of the authorization server itself is typed: not only the checks of the product server decide
	const imtauth::ISessionAuthorizationProvider::SessionAuthorization answer = fixture.remoteSessionProvider->GetSessionAuthorization(
				CRenewalFixture::Jwt(tenantId, s_productId), s_productId, true);
	if (requestStatus == Creator::CCS_INTERNAL_ERROR){
		QCOMPARE(answer.state, imtauth::ISessionAuthorizationProvider::SAS_UNCHECKED);
	}
	else{
		QCOMPARE(answer.state, imtauth::ISessionAuthorizationProvider::SAS_REFUSED);
		QCOMPARE(answer.refusalReason, requestStatus == Creator::CCS_UNAUTHORIZED
					? imtauth::ISessionAuthorizationProvider::RR_REAUTHENTICATE
					: imtauth::ISessionAuthorizationProvider::RR_FORBIDDEN);
	}

	// A request tells a refusal from what could not be checked; a subscription is denied or kept
	QCOMPARE(int(fixture.Create(headers)), requestStatus);
	QCOMPARE(int(fixture.Renew(headers, nullptr, false)), subscriptionState);
	QCOMPARE(int(fixture.lastStatus), requestStatus);
	QCOMPARE(int(fixture.Renew(headers)), subscriptionState);
	QCOMPARE(int(fixture.lastStatus), requestStatus);
	QCOMPARE(fixture.computedUsers->reads, 0);
	QCOMPARE(fixture.serverTenants->reads, 0);
	QCOMPARE(fixture.productSessions->validationCalls, 0);
}


void CAuthorizationRenewalTest::SessionAuthorizationKeepsOrganizationRules_data()
{
	QTest::addColumn<QByteArray>("role");

	QTest::newRow("owner") << QByteArray("owner");
	QTest::newRow("creator") << QByteArray("creator");
	QTest::newRow("member") << QByteArray("member");
	QTest::newRow("no organization") << QByteArray("no organization");
}


void CAuthorizationRenewalTest::SessionAuthorizationKeepsOrganizationRules()
{
	QFETCH(QByteArray, role);

	// The organization is read only by the authorization server; the product server takes its answer
	CRenewalFixture fixture(CRenewalFixture::O_COMPUTED_BY_SOURCE | CRenewalFixture::O_NO_PRODUCT_MEMBERSHIP);
	const QByteArray tenantId = (role == "no organization") ? QByteArray() : s_tenantId;
	fixture.bindings->Bind(s_tenantId, "Roles", s_roleId);
	fixture.sessions->sessionTenantId = tenantId;
	if (role == "owner"){
		fixture.tenants->ownerId = s_userId;
	}
	else if (role == "creator"){
		fixture.tenants->creatorId = s_userId;
	}

	QByteArray fingerprint;
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt(tenantId, s_productId));
	QCOMPARE(fixture.Create(headers, &fingerprint), Creator::CCS_OK);
	QByteArray renewed;
	QCOMPARE(fixture.Renew(headers, &renewed), Creator::AS_CONFIRMED);
	QCOMPARE(renewed, fingerprint);
	QCOMPARE(fixture.ownerInput->requests, 2);
	QCOMPARE(fixture.ownerInput->handledRequests, 2);
	QCOMPARE(fixture.computedUsers->reads, 0);
	QCOMPARE(fixture.serverTenants->reads, 0);
	QCOMPARE(fixture.productSessions->validationCalls, 0);
	QCOMPARE(fixture.productSessions->sessionChecks, 0);
	QCOMPARE(fixture.productSessions->sessionReads, 0);
	if (tenantId.isEmpty()){
		QCOMPARE(fixture.tenants->reads, 0);
	}
	const imtgql::IGqlContextUniquePtr contextPtr = fixture.RenewContext(headers);
	QVERIFY(contextPtr.IsValid());
	// The owner and the creator administer their organization
	QCOMPARE(contextPtr->GetOrganizationPermissions().isEmpty(), role == "member" || role == "no organization");
	if (role == "member" || role == "no organization"){
		const QByteArrayList permissions = contextPtr->GetUserInfo()->GetPermissions(s_productId);
		QVERIFY(!permissions.contains("/Organization/Read"));
		// Tenant navigation remains available to a member, without granting membership administration permissions.
		for (const QByteArray& permission : imtauth::GetOrganizationAccessPermissions()){
			QCOMPARE(permissions.contains(permission), role == "member");
		}
	}
}


void CAuthorizationRenewalTest::SessionAuthorizationRejectsIncompleteReply_data()
{
	QTest::addColumn<QByteArray>("change");

	for (const QByteArray& field : {QByteArray("state"), QByteArray("refusalReason"), QByteArray("userId"), QByteArray("tenantId"),
				QByteArray("userData"), QByteArray("tenantOwnerId"), QByteArray("tenantCreatorId"), QByteArray("tenantPermissions")}){
		const QByteArray rowName = "missing " + field;
		QTest::newRow(rowName.constData()) << rowName;
	}
	QTest::newRow("malformed JSON") << QByteArray("malformed JSON");
	QTest::newRow("unknown state") << QByteArray("unknown state");
	QTest::newRow("contradictory refusal") << QByteArray("contradictory refusal");
	QTest::newRow("refused without reason") << QByteArray("refused without reason");
	QTest::newRow("refused with NONE") << QByteArray("refused with NONE");
	QTest::newRow("invalid serialized user") << QByteArray("invalid serialized user");
	QTest::newRow("payload user mismatch") << QByteArray("payload user mismatch");
	QTest::newRow("payload tenant mismatch") << QByteArray("payload tenant mismatch");
	QTest::newRow("serialized user mismatch") << QByteArray("serialized user mismatch");
	QTest::newRow("serialized user UUID empty") << QByteArray("serialized user UUID empty");
	QTest::newRow("payload and serialized user mismatch") << QByteArray("payload and serialized user mismatch");
	QTest::newRow("disabled serialized user") << QByteArray("disabled serialized user");
	QTest::newRow("no computed product") << QByteArray("no computed product");
	QTest::newRow("roles without computed permissions") << QByteArray("roles without computed permissions");
}


void CAuthorizationRenewalTest::SessionAuthorizationRejectsIncompleteReply()
{
	QFETCH(QByteArray, change);

	CRenewalFixture fixture(CRenewalFixture::O_COMPUTED_BY_SOURCE);
	fixture.sessions->sessionTenantId = s_tenantId;
	fixture.bindings->Bind(s_tenantId, "Roles", s_roleId);
	const QByteArray token = CRenewalFixture::Jwt(s_tenantId, s_productId);
	const auto headers = CRenewalFixture::Headers(token);
	QCOMPARE(fixture.Create(headers), Creator::CCS_OK);
	QCOMPARE(fixture.Renew(headers), Creator::AS_CONFIRMED);

	if (change == "roles without computed permissions"){
		fixture.Role(s_roleId).SetLocalPermissions({});
		const auto emptyAuthorization = fixture.remoteSessionProvider->GetSessionAuthorization(token, s_productId, true);
		QCOMPARE(emptyAuthorization.state, imtauth::ISessionAuthorizationProvider::SAS_COMPUTED);
		const auto* userPtr = dynamic_cast<const imtauth::CUserBaseInfo*>(emptyAuthorization.userPtr.GetPtr());
		QVERIFY(userPtr != nullptr);
		QVERIFY(userPtr->HasLocalPermissions(s_productId));
		QVERIFY(userPtr->GetLocalPermissions(s_productId).isEmpty());
		fixture.Role(s_roleId).SetLocalPermissions({"/Read"});
	}

	bool changed = false;
	fixture.ownerInput->malformedReply = change == "malformed JSON";
	fixture.ownerInput->changePayload = [&fixture, &change, &changed](QJsonObject& payload){
		QVERIFY(payload.contains("state"));
		QVERIFY(payload.contains("userData"));
		if (change.startsWith("missing ")){
			const QString field = QString::fromUtf8(change.mid(8));
			QVERIFY(payload.contains(field));
			payload.remove(field);
		}
		else if (change == "unknown state"){
			payload.insert("state", "UNKNOWN");
		}
		else if (change == "contradictory refusal"){
			payload.insert("refusalReason", "FORBIDDEN");
		}
		else if (change == "refused without reason" || change == "refused with NONE"){
			payload.insert("state", "REFUSED");
			if (change == "refused without reason"){
				payload.remove("refusalReason");
			}
			else{
				payload.insert("refusalReason", "NONE");
			}
		}
		else if (change == "invalid serialized user"){
			payload.insert("userData", "{");
		}
		else if (change == "payload user mismatch"){
			payload.insert("userId", QString::fromUtf8(s_otherUserId));
		}
		else if (change == "payload tenant mismatch"){
			payload.insert("tenantId", QString::fromUtf8(s_otherTenantId));
		}
		else if (change != "malformed JSON"){
			imtauth::CIdentifiableUserInfo user;
			iser::CJsonMemReadArchive reader(payload.value("userData").toString().toUtf8());
			QVERIFY(user.Serialize(reader));
			QCOMPARE(user.GetId(), s_userLogin);
			QCOMPARE(user.GetObjectUuid(), s_userId);
			if (change == "disabled serialized user"){
				user.SetEnabled(false);
			}
			else if (change == "no computed product" || change == "roles without computed permissions"){
				QVERIFY(user.GetProducts().contains(s_productId));
				user.RemoveProduct(s_productId);
				QVERIFY(!user.GetProducts().contains(s_productId));
				if (change == "roles without computed permissions"){
					user.SetRoles(s_productId, {s_roleId});
					QVERIFY(user.GetProducts().contains(s_productId));
					QVERIFY(!user.HasLocalPermissions(s_productId));
				}
			}
			else{
				const QByteArray objectUuid = change == "serialized user UUID empty" ? QByteArray() : s_otherUserId;
				if (objectUuid.isEmpty()){
					QVERIFY(user.imtbase::CIdentifiable::ResetData());
				}
				else{
					QVERIFY(user.SetObjectUuid(objectUuid));
				}
				QCOMPARE(user.GetObjectUuid(), objectUuid);
				QCOMPARE(user.GetId(), s_userLogin);
				if (change == "payload and serialized user mismatch"){
					payload.insert("userId", QString::fromUtf8(s_otherUserId));
				}
			}
			iser::CJsonMemWriteArchive writer(&*fixture.versionInfo);
			QVERIFY(user.Serialize(writer));
			payload.insert("userData", QString::fromUtf8(writer.GetData()));
		}
		changed = true;
	};

	const auto authorization = fixture.remoteSessionProvider->GetSessionAuthorization(token, s_productId, true);
	QVERIFY(changed);
	QCOMPARE(authorization.state, imtauth::ISessionAuthorizationProvider::SAS_UNCHECKED);
	QVERIFY(!authorization.userPtr.IsValid());
	QCOMPARE(fixture.Create(headers), Creator::CCS_INTERNAL_ERROR);
	QCOMPARE(fixture.Renew(headers, nullptr, false), Creator::AS_UNAVAILABLE);
	QCOMPARE(fixture.lastStatus, Creator::CCS_INTERNAL_ERROR);
	QCOMPARE(fixture.Renew(headers), Creator::AS_UNAVAILABLE);
	QCOMPARE(fixture.lastStatus, Creator::CCS_INTERNAL_ERROR);
	QCOMPARE(fixture.ownerInput->inputStatus, Creator::CCS_OK);
	QCOMPARE(fixture.computedUsers->reads, 0);
	QCOMPARE(fixture.serverTenants->reads, 0);
}


void CAuthorizationRenewalTest::SessionAuthorizationContextIsDetached()
{
	imtgql::IGqlContextUniquePtr contextPtr;
	{
		CRenewalFixture fixture(CRenewalFixture::O_COMPUTED_BY_SOURCE);
		const QByteArray token = CRenewalFixture::Jwt(QByteArray(), s_productId);
		const auto authorization = fixture.remoteSessionProvider->GetSessionAuthorization(token, s_productId, true);
		QCOMPARE(authorization.state, imtauth::ISessionAuthorizationProvider::SAS_COMPUTED);
		const imtauth::IUserInfo* remoteUserPtr = dynamic_cast<const imtauth::IUserInfo*>(authorization.userPtr.GetPtr());
		QVERIFY(remoteUserPtr != nullptr);
		QVERIFY(s_userLogin != s_userId);
		QCOMPARE(authorization.userId, s_userId);
		QCOMPARE(remoteUserPtr->GetId(), s_userLogin);
		QCOMPARE(imtbase::GetObjectUuid(remoteUserPtr), s_userId);
		QVERIFY(remoteUserPtr->GetRoleProvider() == nullptr);
		QVERIFY(remoteUserPtr->GetUserGroupProvider() == nullptr);

		const auto headers = CRenewalFixture::Headers(token);
		contextPtr = fixture.RenewContext(headers);
		QVERIFY(contextPtr.IsValid());
		const imtauth::IUserInfo* userPtr = contextPtr->GetUserInfo();
		QVERIFY(userPtr != nullptr);
		QCOMPARE(contextPtr->GetUserId(), s_userId);
		QCOMPARE(userPtr->GetId(), s_userLogin);
		QCOMPARE(imtbase::GetObjectUuid(userPtr), s_userId);
		QVERIFY(userPtr->GetRoleProvider() == nullptr);
		QVERIFY(userPtr->GetUserGroupProvider() == nullptr);
		QCOMPARE(userPtr->GetPermissions(s_productId), QByteArrayList{"/Read"});

		fixture.Role(s_roleId).SetLocalPermissions({"/Read", "/Write"});
		QCOMPARE(userPtr->GetPermissions(s_productId), QByteArrayList{"/Read"});
		const imtgql::IGqlContextUniquePtr freshPtr = fixture.RenewContext(headers);
		QVERIFY(freshPtr.IsValid());
		QVERIFY(freshPtr->GetUserInfo()->GetPermissions(s_productId).contains("/Write"));
	}
	QCOMPARE(contextPtr->GetUserInfo()->GetPermissions(s_productId), QByteArrayList{"/Read"});
	QCOMPARE(contextPtr->GetUserInfo()->GetId(), s_userLogin);
	QCOMPARE(imtbase::GetObjectUuid(contextPtr->GetUserInfo()), s_userId);
}


void CAuthorizationRenewalTest::SessionAuthorizationIsKeptForRequests()
{
	CRenewalFixture fixture(CRenewalFixture::O_COMPUTED_BY_SOURCE | CRenewalFixture::O_SESSION_CACHE);
	fixture.bindings->Bind(s_tenantId, "Roles", s_roleId);
	fixture.sessions->sessionTenantId = s_tenantId;
	const QByteArray token = CRenewalFixture::Jwt(s_tenantId, s_productId);
	const auto headers = CRenewalFixture::Headers(token);

	// The requests of a session share one answer of the authorization server
	QByteArray first;
	QCOMPARE(fixture.Create(headers, &first), Creator::CCS_OK);
	int asked = fixture.ownerInput->requests;
	QVERIFY(asked > 0);
	QByteArray second;
	QCOMPARE(fixture.Create(headers, &second), Creator::CCS_OK);
	QCOMPARE(fixture.ownerInput->requests, asked);
	QCOMPARE(second, first);

	// A subscription is checked there every time, and its answer is not kept for the requests
	fixture.User().SetEnabled(false);
	QCOMPARE(fixture.Renew(headers), Creator::AS_DENIED);
	QVERIFY(fixture.ownerInput->requests > asked);
	asked = fixture.ownerInput->requests;
	QCOMPARE(fixture.Create(headers), Creator::CCS_OK);
	QCOMPARE(fixture.ownerInput->requests, asked);

	// A change this server is told about ends the answer; a refusal is not kept
	fixture.authentication->AccessChanged();
	QCOMPARE(fixture.Create(headers), Creator::CCS_FORBIDDEN);
	QVERIFY(fixture.ownerInput->requests > asked);
	fixture.User().SetEnabled(true);
	QCOMPARE(fixture.Create(headers), Creator::CCS_OK);
	asked = fixture.ownerInput->requests;
	QCOMPARE(fixture.Create(headers), Creator::CCS_OK);
	QCOMPARE(fixture.ownerInput->requests, asked);

	// An invalidated token is asked again
	fixture.authentication->InvalidateToken(token);
	QCOMPARE(fixture.Create(headers), Creator::CCS_OK);
	QVERIFY(fixture.ownerInput->requests > asked);

	// An answer does not outlive its access token
	const auto shortHeaders = CRenewalFixture::Headers(CRenewalFixture::Jwt(s_tenantId, s_productId, 1));
	QCOMPARE(fixture.Create(shortHeaders), Creator::CCS_OK);
	QTest::qWait(2100);
	asked = fixture.ownerInput->requests;
	QVERIFY(fixture.Create(shortHeaders) != Creator::CCS_OK);
	QVERIFY(fixture.ownerInput->requests > asked);
}


void CAuthorizationRenewalTest::SessionOfAnotherProductIsRefused()
{
	// A product server that receives no servers takes only the sessions opened through it
	CRenewalFixture fixture(CRenewalFixture::O_COMPUTED_BY_SOURCE | CRenewalFixture::O_PRODUCT_INFO);
	fixture.bindings->Bind(s_tenantId, "Roles", s_roleId);
	fixture.sessions->sessionTenantId = s_tenantId;

	QCOMPARE(fixture.Create(CRenewalFixture::Headers(CRenewalFixture::Jwt(s_tenantId, s_productId))), Creator::CCS_OK);
	const int asked = fixture.ownerInput->requests;

	for (const QByteArray& sessionProductId : {QByteArray("OtherProduct"), QByteArray()}){
		const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt(s_tenantId, sessionProductId));
		QCOMPARE(fixture.Create(headers), Creator::CCS_FORBIDDEN);
		QCOMPARE(fixture.Renew(headers), Creator::AS_DENIED);
	}

	// Refused here: the authorization server is not asked about it
	QCOMPARE(fixture.ownerInput->requests, asked);
}


void CAuthorizationRenewalTest::SessionAuthorizationSyncTransportFailure_data()
{
	QTest::addColumn<int>("category");
	QTest::newRow("access denied") << int(imtclientgql::IAsyncGqlClient::EC_ACCESS_DENIED);
	QTest::newRow("network failure") << int(imtclientgql::IAsyncGqlClient::EC_NETWORK);
	QTest::newRow("timeout") << int(imtclientgql::IAsyncGqlClient::EC_TIMEOUT);
	QTest::newRow("internal failure") << int(imtclientgql::IAsyncGqlClient::EC_INTERNAL);
}


void CAuthorizationRenewalTest::SessionAuthorizationSyncTransportFailure()
{
	QFETCH(int, category);

	icomp::TSimSharedComponentPtr<CCompletedAsyncClientComp> asyncClient;
	asyncClient->errorCategory = static_cast<imtclientgql::IAsyncGqlClient::ErrorCategory>(category);
	asyncClient->InitComponent();
	icomp::TSimSharedComponentPtr<imtclientgql::CGqlClientSyncAdapterComp> client;
	client->SetRef("AsyncClient", asyncClient);
	client->InitComponent();
	imtclientgql::IGqlClient::GqlRequestPtr requestPtr;
	requestPtr.SetPtr(new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "GetSessionAuthorization"));
	const bool expectedDenied = category == imtclientgql::IAsyncGqlClient::EC_ACCESS_DENIED;
	const auto result = client->SendRequest(requestPtr);
	QVERIFY(!result);
	QCOMPARE(result.errorCategory == imtclientgql::IGqlClient::EC_ACCESS_DENIED, expectedDenied);
	QVERIFY(!client->SendRequest(requestPtr));

	TComponentFactory<imtauth::CUserInfoComp> userFactory;
	icomp::TSimSharedComponentPtr<imtauthgql::CRemoteJwtSessionControllerComp> provider;
	provider->SetRef("ApiClient", client);
	provider->SetFactory("UserFactory", &userFactory);
	provider->InitComponent();
	const auto authorization = provider->GetSessionAuthorization(CRenewalFixture::Jwt(QByteArray(), s_productId), s_productId, true);
	QCOMPARE(authorization.state, expectedDenied ? imtauth::ISessionAuthorizationProvider::SAS_REFUSED : imtauth::ISessionAuthorizationProvider::SAS_UNCHECKED);
	if (expectedDenied){
		QCOMPARE(authorization.refusalReason, imtauth::ISessionAuthorizationProvider::RR_FORBIDDEN);
	}
	QVERIFY(!authorization.userPtr.IsValid());
	QCOMPARE(asyncClient->requests, 3);
}


void CAuthorizationRenewalTest::SessionAuthorizationHttpTransportFailure_data()
{
	QTest::addColumn<int>("httpStatus");
	QTest::newRow("HTTP 401") << 401;
	QTest::newRow("HTTP 403") << 403;
	QTest::newRow("HTTP 503") << 503;
	QTest::newRow("HTTP 200 incomplete payload") << 200;
}


void CAuthorizationRenewalTest::SessionAuthorizationHttpTransportFailure()
{
	QFETCH(int, httpStatus);

	// Like the imtrest uploader tests: the real synchronous HTTP client serves this loopback in its event loop.
	QTcpServer server;
	QVERIFY(server.listen(QHostAddress::LocalHost, 0));
	int requests = 0;
	QObject::connect(&server, &QTcpServer::newConnection, &server, [&server, httpStatus, &requests](){
		while (server.hasPendingConnections()){
			QTcpSocket* socketPtr = server.nextPendingConnection();
			auto buffer = std::make_shared<QByteArray>();
			QObject::connect(socketPtr, &QTcpSocket::readyRead, socketPtr, [socketPtr, buffer, httpStatus, &requests](){
				*buffer += socketPtr->readAll();
				const qsizetype headEnd = buffer->indexOf("\r\n\r\n");
				if (headEnd < 0){
					return;
				}
				int bodyLength = 0;
				for (const QByteArray& line : buffer->left(headEnd).split('\n')){
					if (line.toLower().startsWith("content-length:")){
						bodyLength = line.mid(line.indexOf(':') + 1).trimmed().toInt();
					}
				}
				if (buffer->size() < headEnd + 4 + bodyLength){
					return;
				}
				++requests;
				buffer->clear();
				socketPtr->write("HTTP/1.1 " + QByteArray::number(httpStatus) + " Test\r\nContent-Type: application/json\r\nContent-Length: 2\r\nConnection: close\r\n\r\n{}");
				socketPtr->disconnectFromHost();
			});
		}
	});

	icomp::TSimSharedComponentPtr<CLocalHttpProtocolComp> protocol;
	protocol->url = QUrl(QStringLiteral("http://127.0.0.1:%1/graphql").arg(server.serverPort()));
	protocol->InitComponent();
	icomp::TSimSharedComponentPtr<imtclientgql::CApiClientComp> client;
	client->SetRef("ProtocolEngine", protocol);
	client->InitComponent();
	imtclientgql::IGqlClient::GqlRequestPtr requestPtr;
	requestPtr.SetPtr(new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "GetSessionAuthorization"));
	const bool expectedDenied = httpStatus == 401 || httpStatus == 403;
	const auto result = client->SendRequest(requestPtr);
	QCOMPARE(bool(result), httpStatus == 200);
	QCOMPARE(result.errorCategory == imtclientgql::IGqlClient::EC_ACCESS_DENIED, expectedDenied);
	QCOMPARE(bool(client->SendRequest(requestPtr)), httpStatus == 200);

	TComponentFactory<imtauth::CUserInfoComp> userFactory;
	icomp::TSimSharedComponentPtr<imtauthgql::CRemoteJwtSessionControllerComp> provider;
	provider->SetRef("ApiClient", client);
	provider->SetFactory("UserFactory", &userFactory);
	provider->InitComponent();
	const auto authorization = provider->GetSessionAuthorization(CRenewalFixture::Jwt(QByteArray(), s_productId), s_productId, true);
	QCOMPARE(authorization.state, expectedDenied ? imtauth::ISessionAuthorizationProvider::SAS_REFUSED : imtauth::ISessionAuthorizationProvider::SAS_UNCHECKED);
	if (expectedDenied){
		QCOMPARE(authorization.refusalReason, imtauth::ISessionAuthorizationProvider::RR_FORBIDDEN);
	}
	QVERIFY(!authorization.userPtr.IsValid());
	QCOMPARE(requests, 3);

	// The collection preserves the same real HTTP result for both of its read requests.
	TComponentFactory<CCollectionUserComp> collectionUserFactory;
	icomp::TSimSharedComponentPtr<imtserverapp::CGqlObjectCollectionDelegateComp> delegate;
	delegate->SetIdAttr("CollectionId", "Users");
	delegate->InitComponent();
	for (const int typeCount : {1, 2}){
		icomp::TSimSharedComponentPtr<imtclientgql::CGqlObjectCollectionComp> collection;
		collection->SetRef("ApiClient", client);
		collection->SetRef("GqlDatabaseDelegate", delegate);
		for (int index = 0; index < typeCount; ++index){
			collection->InsertMultiAttr("TypeIds", QByteArray::number(index));
			collection->InsertMultiAttr("TypeNames", QString::number(index));
			collection->InsertMultiFactory("ObjectFactories", &collectionUserFactory);
		}
		collection->InitComponent();
		imtbase::IObjectCollection::DataPtr userPtr;
		bool isAccessDenied = !expectedDenied;
		QVERIFY(!collection->GetObjectDataWithStatus(s_userId, userPtr, isAccessDenied));
		QCOMPARE(isAccessDenied, expectedDenied);
		QVERIFY(!userPtr.IsValid());
	}
	QCOMPARE(requests, 5);
}


void CAuthorizationRenewalTest::PersonalTokenKeepsItsRoute()
{
	// A personal access token keeps its own checks: the session authorization is not asked
	CRenewalFixture fixture(CRenewalFixture::O_COMPUTED_BY_SOURCE);
	fixture.personalTokens->permissions = {"/Read"};
	fixture.Role(s_roleId).SetLocalPermissions({"/Read", "/Write"});
	const auto headers = CRenewalFixture::Headers("imt_pat_personal");

	QByteArray authorization;
	QCOMPARE(fixture.Create(headers, &authorization), Creator::CCS_OK);
	QVERIFY(Part(authorization, FP_PERMISSIONS).contains("/Read"));
	QVERIFY(!Part(authorization, FP_PERMISSIONS).contains("/Write"));
	for (const bool isEstablished : {false, true}){
		QByteArray renewed;
		QCOMPARE(fixture.Renew(headers, &renewed, isEstablished), Creator::AS_CONFIRMED);
		QCOMPARE(Part(renewed, FP_PERMISSIONS), Part(authorization, FP_PERMISSIONS));
	}
	QCOMPARE(fixture.ownerInput->requests, 0);

	// A revoked token is refused as before
	fixture.personalTokens->isValid = false;
	QCOMPARE(fixture.Renew(headers, nullptr, false), Creator::AS_DENIED);
	QCOMPARE(fixture.Renew(headers), Creator::AS_DENIED);
	QCOMPARE(fixture.Create(headers), Creator::CCS_FORBIDDEN);
	QCOMPARE(fixture.ownerInput->requests, 0);
}


void CAuthorizationRenewalTest::PersonalTokenRemoteUserRead_data()
{
	QTest::addColumn<bool>("multipleTypes");
	QTest::addColumn<QByteArray>("failure");
	for (const bool multipleTypes : {false, true}){
		for (const QByteArray& failure : {QByteArray("disabled"), QByteArray("unavailable"), QByteArray("malformed")}){
			const QByteArray name = (multipleTypes ? "type-id-" : "data-") + failure;
			QTest::newRow(name.constData()) << multipleTypes << failure;
		}
	}
	QTest::newRow("data-refused-after-type-id") << true << QByteArray("disabled during data");
}


void CAuthorizationRenewalTest::PersonalTokenRemoteUserRead()
{
	QFETCH(bool, multipleTypes);
	QFETCH(QByteArray, failure);
	CRenewalFixture fixture(CRenewalFixture::O_COMPUTED_BY_SOURCE
				| CRenewalFixture::O_REMOTE_USERS | (multipleTypes ? CRenewalFixture::O_MULTI_USER_TYPES : 0));
	fixture.personalTokens->productId = s_productId;
	const auto headers = CRenewalFixture::Headers("imt_pat_personal");
	QByteArray authorization;
	QCOMPARE(fixture.Create(headers, &authorization), Creator::CCS_OK);
	QVERIFY(Part(authorization, FP_PERMISSIONS).contains("/Read"));
	QVERIFY(fixture.userInput->userTokensSent > 0);
	QCOMPARE(fixture.ownerInput->requests, 0);

	// Prime the actual cache with a live user. The fresh check must bypass it after the refusal.
	imtgql::CGqlContext requestContext;
	requestContext.SetToken("imt_pat_personal");
	requestContext.SetProductId(s_productId);
	imtgql::CGqlRequestContextManager::SetContext(&requestContext);
	imtbase::IObjectCollection::DataPtr cachedUserPtr;
	const bool isCached = fixture.cachedUsers->GetObjectData(s_userId, cachedUserPtr);
	imtgql::CGqlRequestContextManager::Clear();
	QVERIFY(isCached);

	const bool isDenied = failure.startsWith("disabled");
	if (failure == "disabled"){
		fixture.User().SetEnabled(false);
	}
	else if (failure == "disabled during data"){
		fixture.userInput->beforeRequest = [&fixture](const QByteArray& command){
			fixture.User().SetEnabled(command != "GetObjectData");
		};
	}
	else if (failure == "unavailable"){
		fixture.userInput->isUnavailable = true;
	}
	else{
		fixture.userInput->malformedReply = true;
	}
	const int requestsBefore = fixture.userInput->requests;
	fixture.userInput->commands.clear();
	QCOMPARE(fixture.Create(headers), isDenied ? Creator::CCS_FORBIDDEN : Creator::CCS_INTERNAL_ERROR);
	for (const bool isEstablished : {false, true}){
		QCOMPARE(fixture.Renew(headers, nullptr, isEstablished), isDenied ? Creator::AS_DENIED : Creator::AS_UNAVAILABLE);
		QCOMPARE(fixture.lastStatus, isDenied ? Creator::CCS_FORBIDDEN : Creator::CCS_INTERNAL_ERROR);
	}
	QVERIFY(fixture.userInput->requests > requestsBefore);
	if (isDenied){
		// In particular no SelectIds fallback may reinterpret a confirmed refusal.
		const QByteArrayList expectedCommands = failure == "disabled during data"
					? QByteArrayList{"GetObjectTypeId", "GetObjectData"}
					: QByteArrayList{multipleTypes ? "GetObjectTypeId" : "GetObjectData"};
		QCOMPARE(fixture.userInput->commands, expectedCommands + expectedCommands + expectedCommands);
	}
	QCOMPARE(fixture.ownerInput->requests, 0);
	QCOMPARE(fixture.serverRoles->TotalReads(), 0);
	QCOMPARE(fixture.serverGroups->TotalReads(), 0);

	fixture.userInput->beforeRequest = {};
	fixture.userInput->isUnavailable = false;
	fixture.userInput->malformedReply = false;
	fixture.User().SetEnabled(true);
	QCOMPARE(fixture.Create(headers), Creator::CCS_OK);
	QCOMPARE(fixture.Renew(headers), Creator::AS_CONFIRMED);
	QCOMPARE(fixture.ownerInput->requests, 0);
}


void CAuthorizationRenewalTest::RemoteCollectionReadStatus_data()
{
	QTest::addColumn<bool>("multipleTypes");
	QTest::newRow("single factory") << false;
	QTest::newRow("multiple factories") << true;
}


void CAuthorizationRenewalTest::RemoteCollectionReadStatus()
{
	QFETCH(bool, multipleTypes);
	CRenewalFixture fixture(CRenewalFixture::O_COMPUTED_BY_SOURCE
				| CRenewalFixture::O_REMOTE_USERS | (multipleTypes ? CRenewalFixture::O_MULTI_USER_TYPES : 0));
	fixture.personalTokens->productId = s_productId;
	fixture.userInput->authenticationToken = "imt_pat_personal";
	const auto* statusPtr = dynamic_cast<const imtbase::IObjectCollectionReadStatus*>(&*fixture.remoteUsers);
	QVERIFY(statusPtr != nullptr);
	QCOMPARE(fixture.remoteUsers->GetObjectTypesInfo()->GetOptionsCount(), multipleTypes ? 2 : 1);
	imtbase::IObjectCollection::DataPtr dataPtr;
	bool isAccessDenied = true;
	QVERIFY(statusPtr->GetObjectDataWithStatus(s_userId, dataPtr, isAccessDenied));
	QVERIFY(!isAccessDenied);
	const auto* userPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
	QVERIFY(userPtr != nullptr);
	QCOMPARE(userPtr->GetId(), s_userLogin);
	QCOMPARE(imtbase::GetObjectUuid(userPtr), s_userId);
	QVERIFY(userPtr->GetPermissions(s_productId).contains("/Read"));
	QCOMPARE(fixture.userInput->requests, multipleTypes ? 2 : 1);

	const istd::IChangeable* previousPtr = dataPtr.GetPtr();
	fixture.User().SetEnabled(false);
	QVERIFY(!statusPtr->GetObjectDataWithStatus(s_userId, dataPtr, isAccessDenied));
	QVERIFY(isAccessDenied);
	QCOMPARE(dataPtr.GetPtr(), previousPtr);
	QVERIFY(!fixture.remoteUsers->GetObjectData(s_userId, dataPtr));
	QCOMPARE(dataPtr.GetPtr(), previousPtr);

	fixture.userInput->isUnavailable = true;
	QVERIFY(!statusPtr->GetObjectDataWithStatus(s_userId, dataPtr, isAccessDenied));
	QVERIFY(!isAccessDenied);
	QCOMPARE(dataPtr.GetPtr(), previousPtr);
	fixture.userInput->isUnavailable = false;
	fixture.User().SetEnabled(true);
	QVERIFY(statusPtr->GetObjectDataWithStatus(s_userId, dataPtr, isAccessDenied));
	QVERIFY(!isAccessDenied);
	QVERIFY(dataPtr.IsValid());
	QCOMPARE(fixture.ownerInput->requests, 0);
}


void CAuthorizationRenewalTest::SnapshotAccessRefusal_data()
{
	QTest::addColumn<QByteArray>("kind");
	QTest::newRow("user") << QByteArray("user");
	QTest::newRow("role") << QByteArray("role");
	QTest::newRow("group") << QByteArray("group");
}


void CAuthorizationRenewalTest::SnapshotAccessRefusal()
{
	QFETCH(QByteArray, kind);
	CRenewalFixture fixture(CRenewalFixture::O_COMPUTED_BY_SOURCE);
	const QByteArray groupId = "refused-group";
	if (kind == "user"){
		fixture.users->deniedIds.insert(s_userId);
	}
	else if (kind == "role"){
		fixture.roles->deniedIds.insert(s_roleId);
	}
	else{
		fixture.AddGroup(groupId, s_productId, {s_roleId});
		fixture.User().AddToGroup(groupId);
		fixture.groups->deniedIds.insert(groupId);
	}

	imtservergql::CAuthorizationSnapshot snapshot(&*fixture.users, &*fixture.roles, &*fixture.groups);
	istd::IChangeableUniquePtr userPtr;
	QString errorMessage;
	const auto status = snapshot.ReadUser(s_userId, userPtr, errorMessage);
	if (kind == "user"){
		QCOMPARE(status, imtservergql::CAuthorizationSnapshot::RS_DENIED);
		QVERIFY(!userPtr.IsValid());
		QVERIFY(!errorMessage.isEmpty());
	}
	else{
		QCOMPARE(status, imtservergql::CAuthorizationSnapshot::RS_READ);
		auto* userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(userPtr.GetPtr());
		QVERIFY(userInfoPtr != nullptr);
		QVERIFY(!snapshot.KeepPermissions(*userInfoPtr, s_productId, errorMessage));
		QVERIFY(!errorMessage.isEmpty());
		// A refused graph node is read once, remains incomplete, and gives no partial authorization.
		const int readsBefore = fixture.roles->TotalReads() + fixture.groups->TotalReads();
		QVERIFY(!snapshot.KeepPermissions(*userInfoPtr, s_productId, errorMessage));
		QCOMPARE(fixture.roles->TotalReads() + fixture.groups->TotalReads(), readsBefore);
		imtservergql::CAuthorizationSnapshot::DetachUser(*userPtr);
	}
	QCOMPARE(fixture.users->selections + fixture.roles->selections + fixture.groups->selections, 0);

	const auto authorization = fixture.sessionHandler2->GetSessionAuthorization(CRenewalFixture::Jwt(QByteArray(), s_productId), s_productId, true);
	QCOMPARE(authorization.state, kind == "user" ? imtauth::ISessionAuthorizationProvider::SAS_REFUSED : imtauth::ISessionAuthorizationProvider::SAS_UNCHECKED);
	if (kind == "user"){
		QCOMPARE(authorization.refusalReason, imtauth::ISessionAuthorizationProvider::RR_FORBIDDEN);
	}
	QVERIFY(!authorization.userPtr.IsValid());
	QVERIFY(!authorization.message.isEmpty());
}


void CAuthorizationRenewalTest::TenantLookupReportsCompleteness_data()
{
	QTest::addColumn<QByteArray>("condition");
	QTest::addColumn<bool>("expectedRead");
	QTest::addColumn<bool>("expectedTenant");
	QTest::newRow("absent tenant") << QByteArray("absent") << true << false;
	QTest::newRow("selection failure") << QByteArray("selection failure") << false << false;
	QTest::newRow("listed but unreadable tenant") << QByteArray("unreadable") << false << false;
	QTest::newRow("existing tenant") << QByteArray("existing") << true << true;
}


void CAuthorizationRenewalTest::TenantLookupReportsCompleteness()
{
	QFETCH(QByteArray, condition);
	QFETCH(bool, expectedRead);
	QFETCH(bool, expectedTenant);

	icomp::TSimSharedComponentPtr<CFlatTenantCollectionComp> collection;
	auto tenantPtr = std::make_shared<imtauth::CIdentifiableTenantInfo>();
	const QByteArray storedId = condition == "absent" ? s_otherTenantId : s_tenantId;
	tenantPtr->SetObjectUuid(storedId);
	tenantPtr->SetTenantId(storedId);
	tenantPtr->SetOwnerId(s_userId);
	collection->documents.insert(storedId, tenantPtr);
	collection->isReadable = condition != "selection failure";
	if (condition == "unreadable"){
		collection->unreadableIds.insert(s_tenantId);
	}
	collection->InitComponent();
	TComponentFactory<imtauth::CTenantInfoComp> tenantFactory;
	icomp::TSimSharedComponentPtr<imtauth::CTenantManagerComp> manager;
	manager->SetRef("TenantCollection", collection);
	manager->SetFactory("TenantFactory", &tenantFactory);
	manager->InitComponent();

	bool isRead = !expectedRead;
	const imtauth::ITenantInfoUniquePtr resultPtr = manager->GetTenant(s_tenantId, &isRead);
	QCOMPARE(isRead, expectedRead);
	QCOMPARE(resultPtr.IsValid(), expectedTenant);
	QCOMPARE(collection->reads.value(s_tenantId), 1);
	QCOMPARE(collection->selections, expectedTenant ? 0 : 1);
	if (expectedTenant){
		QCOMPARE(resultPtr->GetTenantId(), s_tenantId);
		QCOMPARE(resultPtr->GetOwnerId(), s_userId);
	}
	else{
		QCOMPARE(collection->selectedField, QByteArray("Id"));
		QCOMPARE(collection->selectedId, s_tenantId);
	}
}


void CAuthorizationRenewalTest::AccessWithinTheRegistration_data()
{
	QTest::addColumn<QByteArray>("change");
	QTest::addColumn<bool>("isWithin");
	QTest::addColumn<bool>("isSameRecipient");

	QTest::newRow("unchanged") << QByteArray("unchanged") << true << true;
	QTest::newRow("a permission more") << QByteArray("permission more") << true << true;
	QTest::newRow("a permission less") << QByteArray("permission less") << false << true;
	QTest::newRow("a group more") << QByteArray("group more") << true << true;
	QTest::newRow("a group less") << QByteArray("group less") << false << true;
	QTest::newRow("an organization permission less") << QByteArray("organization permission less") << false << true;
	QTest::newRow("a permission of the organization less") << QByteArray("tenant permission less") << false << true;
	QTest::newRow("a bound appears") << QByteArray("bound appears") << false << true;
	QTest::newRow("the bound goes") << QByteArray("bound goes") << false << true;
	QTest::newRow("an empty bound is not none") << QByteArray("bound empty") << false << true;
	QTest::newRow("the bound grows") << QByteArray("bound grows") << true << true;
	QTest::newRow("the authority to grant goes") << QByteArray("granting goes") << false << true;
	QTest::newRow("granting as administrator") << QByteArray("granting admin") << false << true;
	QTest::newRow("administrator") << QByteArray("admin") << false << true;
	QTest::newRow("owner") << QByteArray("owner") << false << true;
	QTest::newRow("another organization") << QByteArray("tenant") << false << false;
	QTest::newRow("another product") << QByteArray("product") << false << false;
	QTest::newRow("another server") << QByteArray("server product") << false << false;
	QTest::newRow("another user") << QByteArray("user") << false << false;
	QTest::newRow("another principal") << QByteArray("principal") << false << false;
}


void CAuthorizationRenewalTest::AccessWithinTheRegistration()
{
	QFETCH(QByteArray, change);
	QFETCH(bool, isWithin);
	QFETCH(bool, isSameRecipient);

	imtservergql::AuthorizationData registered;
	registered.principalKind = imtgql::IGqlContext::PK_USER;
	registered.userId = s_userId;
	registered.tenantId = s_tenantId;
	registered.productId = s_productId;
	registered.hasUser = true;
	registered.groups = {"g1", "g2"};
	registered.permissions = {"/A", "/B"};
	registered.organizationPermissions = {"/Org"};
	registered.tenantPermissions = {"/A", "/B"};
	registered.hasBoundary = true;
	registered.boundary = {"/A"};
	registered.hasGrantingAuthority = true;
	registered.grantingPermissions = {"/A"};

	imtservergql::AuthorizationData current = registered;
	if (change == "permission more"){ current.permissions = {"/A", "/B", "/C"}; }
	else if (change == "permission less"){ current.permissions = {"/A"}; }
	else if (change == "group more"){ current.groups = {"g1", "g2", "g3"}; }
	else if (change == "group less"){ current.groups = {"g1"}; }
	else if (change == "organization permission less"){ current.organizationPermissions = {}; }
	else if (change == "tenant permission less"){ current.tenantPermissions = {"/A"}; }
	else if (change == "bound appears"){ registered.hasBoundary = false; registered.boundary = {}; }
	else if (change == "bound goes"){ current.hasBoundary = false; current.boundary = {}; }
	else if (change == "bound empty"){ registered.hasBoundary = false; registered.boundary = {}; current.boundary = {}; }
	else if (change == "bound grows"){ current.boundary = {"/A", "/B"}; }
	else if (change == "granting goes"){ current.hasGrantingAuthority = false; current.grantingPermissions = {}; }
	else if (change == "granting admin"){ current.isGrantingAdmin = true; }
	else if (change == "admin"){ current.isAdmin = true; }
	else if (change == "owner"){ current.isTenantOwner = true; }
	else if (change == "tenant"){ current.tenantId = s_otherTenantId; }
	else if (change == "product"){ current.productId = "Other"; }
	else if (change == "server product"){ current.serverProductId = "Server"; }
	else if (change == "user"){ current.userId = "4f3e2d1c-0b9a-4876-9543-210fedcba987"; }
	else if (change == "principal"){ current.principalKind = imtgql::IGqlContext::PK_SERVICE; }

	QCOMPARE(imtservergql::IsAccessWithin(registered, current), isWithin);
	QCOMPARE(imtservergql::IsSameRecipient(registered, current), isSameRecipient);
}


I_ADD_TEST(CAuthorizationRenewalTest);
