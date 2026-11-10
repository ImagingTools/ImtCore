// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CAuthorizationRenewalTest.h"


#include <functional>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <icomp/TSimComponentWrap.h>
#include <ilog/CMessage.h>
#include <itest/CStandardTestExecutor.h>
#include <imtauth/CPersonalAccessTokenManagerComp.h>
#include <imtauth/CRole.h>
#include <imtauth/CTenantInfo.h>
#include <imtauth/CTenantMembership.h>
#include <imtauth/CUserInfo.h>
#include <imtauth/IJwtSessionController.h>
#include <imtauth/IRoleInfoProvider.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtbase/CCollectionInfo.h>
#include <imtbase/CObjectCollection.h>
#include <imtbase/imtbase.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <imtservergql/CAuthenticationManagerComp.h>
#include <imtservergql/IServerTokenReceiver.h>
#include <imtservergql/imtservergql.h>


namespace
{


typedef imtgql::IGqlContextCreator Creator;

const QByteArray s_userId = "6a1e5d1c-7f0b-4b5e-9a3e-2b4f1c0d9e01";
const QByteArray s_tenantId = "0c9d8e7f-6a5b-4c3d-8e2f-1a0b9c8d7e6f";
const QByteArray s_sessionId = "3f2e1d0c-9b8a-4f6e-8d5c-4b3a2f1e0d9c";
const QByteArray s_productId = "Product";


class CContextComp: public icomp::CComponentBase, public imtgql::CGqlContext
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CContextComp)
		I_REGISTER_INTERFACE(imtgql::IGqlContext);
	I_END_COMPONENT
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


// Roles are read at the time the permissions are asked for, as the cached role collection of a product server does
class CRoleProvider: virtual public imtauth::IRoleInfoProvider
{
public:
	QByteArrayList permissions{"/Read"};

	const imtbase::ICollectionInfo& GetRoleList() const override { return m_roles; }
	imtauth::IRoleUniquePtr GetRole(const QByteArray& roleId, const iprm::IParamsSet*) const override
	{
		auto* rolePtr = new imtauth::CRole;
		rolePtr->SetRoleId(roleId);
		rolePtr->SetProductId(s_productId);
		rolePtr->SetLocalPermissions(permissions);
		return imtauth::IRoleUniquePtr(rolePtr);
	}

private:
	imtbase::CCollectionInfo m_roles;
};


// Identifiable, as the users of a user collection are
class CRoleUserInfo: public imtauth::CIdentifiableUserInfo
{
public:
	void SetRoleProvider(const imtauth::IRoleInfoProvider* providerPtr) { m_roleProviderPtr = providerPtr; }
};


// As strict as the SQL collection: a failed read is reported, an absent user is an empty selection
class CUserCollectionComp: public icomp::CComponentBase, public imtbase::CObjectCollection
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CUserCollectionComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
	I_END_COMPONENT

	CRoleUserInfo user;
	bool isPresent = true;
	bool isReadable = true;

	bool GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet*) const override
	{
		if (!isPresent || !isReadable || objectId != user.GetId()){
			return false;
		}
		dataPtr.FromUnique(user.CloneMe());
		return dataPtr.IsValid();
	}

	Ids GetElementIds(int, int, const iprm::IParamsSet*, ilog::IMessageConsumer* logPtr) const override
	{
		if (!isReadable){
			if (logPtr != nullptr){
				logPtr->AddMessage(ilog::IMessageConsumer::MessagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, "Read failed", "Test")));
			}
			return Ids();
		}
		return isPresent ? Ids{user.GetId()} : Ids();
	}
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

	bool ValidateSession(const QByteArray& sessionId, bool* isCheckedPtr) const override
	{
		if (isCheckedPtr != nullptr){
			*isCheckedPtr = isSessionChecked;
		}
		return isSessionChecked && isSessionAlive && sessionId == s_sessionId;
	}
	JwtState ValidateJwt(const QByteArray&) const override
	{
		if (duringValidation){
			duringValidation();
		}
		return jwtState;
	}
	bool RefreshToken(const QByteArray&, UserSession&) const override { return false; }
	bool CreateNewSession(const QByteArray&, const QByteArray&, UserSession&) const override { return false; }
	imtauth::ISessionSharedPtr GetSession(const QByteArray&) const override { return imtauth::ISessionSharedPtr(); }
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

	imtauth::ITenantInfoUniquePtr GetTenant(const QByteArray& tenantId, bool* isReadPtr) const override
	{
		if (isReadPtr != nullptr){
			*isReadPtr = isReadable;
		}
		if (!isReadable || !isPresent){
			return imtauth::ITenantInfoUniquePtr();
		}
		imtauth::ITenantInfoUniquePtr tenantPtr(new imtauth::CIdentifiableTenantInfo);
		tenantPtr->SetTenantId(tenantId);
		tenantPtr->SetOwnerId("owner");
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
};


class CTestAuthenticationManager: public imtservergql::CAuthenticationManagerComp
{
public:
	void AccessChanged()
	{
		OnUpdate(nullptr, istd::IChangeable::ChangeSet(istd::IChangeable::CF_ANY));
	}
};


struct CRenewalFixture
{
	CContextFactory factory;
	CRoleProvider roles;
	icomp::TSimSharedComponentPtr<CUserCollectionComp> users;
	icomp::TSimSharedComponentPtr<CSessionControllerComp> sessions;
	icomp::TSimSharedComponentPtr<CMembershipManagerComp> memberships;
	icomp::TSimSharedComponentPtr<CTenantManagerComp> tenants;
	icomp::TSimSharedComponentPtr<CTokenManagerStub> personalTokens;
	icomp::TSimSharedComponentPtr<CTokenManagerStub> serverTokens;
	icomp::TSimSharedComponentPtr<CReceiverComp> receiver;
	icomp::TSimSharedComponentPtr<CTestAuthenticationManager> authentication;

	explicit CRenewalFixture(bool withReceiver = false)
	{
		users->user.SetId(s_userId);
		users->user.SetEnabled(true);
		users->user.SetRoles(s_productId, {"role"});
		users->user.SetRoleProvider(&roles);
		users->InitComponent();
		sessions->InitComponent();
		memberships->InitComponent();
		tenants->InitComponent();
		personalTokens->InitComponent();
		serverTokens->productId = "Receiver";
		serverTokens->clientProductId = "Client";
		serverTokens->InitComponent();
		receiver->managerPtr = &*serverTokens;
		receiver->InitComponent();

		authentication->SetRef("UserCollection", users);
		authentication->SetRef("SlaveJwtSessionController", sessions);
		authentication->SetRef("MembershipManager", memberships);
		authentication->SetRef("TenantManager", tenants);
		authentication->SetRef("PersonalAccessTokenManager", personalTokens);
		if (withReceiver){
			authentication->SetRef("ServerTokenReceiver", receiver);
		}
		authentication->SetFactory("GqlContextFactory", &factory);
		authentication->InitComponent();
	}

	static QByteArray Jwt(const QByteArray& tenantId = QByteArray(), const QByteArray& productId = QByteArray())
	{
		QJsonObject payload;
		payload.insert("userId", QString::fromUtf8(s_userId));
		payload.insert("tenantId", QString::fromUtf8(tenantId));
		if (!productId.isEmpty()){
			payload.insert("productId", QString::fromUtf8(productId));
		}
		payload.insert("sessionId", QString::fromUtf8(s_sessionId));
		payload.insert("exp", double(QDateTime::currentSecsSinceEpoch() + 3600));
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

	Creator::AuthorizationState Renew(const imtgql::IGqlContext::Headers& headers, QByteArray* authorizationPtr = nullptr)
	{
		Creator::AuthorizationState state = Creator::AS_UNAVAILABLE;
		QString error;
		const imtgql::IGqlContextUniquePtr contextPtr = authentication->RenewGqlContext(headers, state, error);
		if (state == Creator::AS_CONFIRMED){
			if (!contextPtr.IsValid()){
				return Creator::AS_UNAVAILABLE;
			}
			if (authorizationPtr != nullptr){
				*authorizationPtr = imtservergql::GetAuthorizationFingerprint(*contextPtr);
			}
		}
		else if (contextPtr.IsValid() || error.isEmpty()){
			// Only a confirmed check gives a context, every other one says why: no state matches this one
			return static_cast<Creator::AuthorizationState>(-1);
		}
		return state;
	}

	Creator::ContextCreationStatus Create(const imtgql::IGqlContext::Headers& headers)
	{
		Creator::ContextCreationError error;
		const imtgql::IGqlContextUniquePtr contextPtr = authentication->CreateGqlContext(headers, error);
		imtgql::CGqlRequestContextManager::Clear();
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
	QVERIFY(before.contains("/Read"));
	QVERIFY(!before.contains("/Write"));
	const istd::IChangeableUniquePtr userBeforePtr = fixture.users->user.CloneMe();
	QVERIFY(userBeforePtr.IsValid());

	// The role changes, the document of the user does not
	fixture.roles.permissions = {"/Read", "/Write"};
	QVERIFY(fixture.users->user.IsEqual(*userBeforePtr));
	QByteArray after;
	QCOMPARE(fixture.Renew(headers, &after), Creator::AS_CONFIRMED);
	QVERIFY(after.contains("/Write"));
	QVERIFY(before != after);

	QByteArray again;
	QCOMPARE(fixture.Renew(headers, &again), Creator::AS_CONFIRMED);
	QCOMPARE(again, after);
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
	CRenewalFixture fixture(server);
	fixture.roles.permissions = {"/Read", "/Write"};
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
	QVERIFY(after.contains("bounded:/Read\n") || after.endsWith("bounded:/Read"));
	QVERIFY(before != after);

	tokens.isValid = false;
	QCOMPARE(fixture.Renew(headers), Creator::AS_DENIED);
	tokens.isValid = true;
	tokens.isCheckFailed = true;
	QCOMPARE(fixture.Renew(headers), Creator::AS_UNAVAILABLE);
}


void CAuthorizationRenewalTest::ExpiredAccessToken_data()
{
	QTest::addColumn<bool>("alive");
	QTest::addColumn<bool>("checked");
	QTest::addColumn<int>("expected");
	QTest::newRow("session-alive") << true << true << int(Creator::AS_CONFIRMED);
	QTest::newRow("session-closed") << false << true << int(Creator::AS_DENIED);
	QTest::newRow("session-not-checked") << true << false << int(Creator::AS_UNAVAILABLE);
}


void CAuthorizationRenewalTest::ExpiredAccessToken()
{
	QFETCH(bool, alive);
	QFETCH(bool, checked);
	QFETCH(int, expected);
	CRenewalFixture fixture;
	fixture.sessions->jwtState = imtauth::IJwtSessionController::JS_EXPIRED;
	fixture.sessions->isSessionAlive = alive;
	fixture.sessions->isSessionChecked = checked;
	const auto headers = CRenewalFixture::Headers(CRenewalFixture::Jwt());

	QCOMPARE(int(fixture.Renew(headers)), expected);

	// A new request with the same token is refused whatever the session: the client has to refresh it
	QCOMPARE(fixture.Create(headers), Creator::CCS_UNAUTHORIZED);
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
		fixture.users->isPresent = false;
		break;
	default:
		fixture.users->user.SetEnabled(false);
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

	// A request goes on without the organization data it could not read, and does not keep that
	fixture.memberships->isReadable = false;
	QCOMPARE(fixture.Create(headers), Creator::CCS_OK);
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


I_ADD_TEST(CAuthorizationRenewalTest);
