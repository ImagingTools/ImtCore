// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CLdapGroupMappingTest.h"

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QRegularExpression>
#include <QtCore/QUuid>

// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TSimComponentWrap.h>
#include <iprm/CEnableableParam.h>
#include <iprm/TParamsPtr.h>
#include <iser/CMemoryReadArchive.h>
#include <iser/CMemoryWriteArchive.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtbase/CObjectCollectionBase.h>
#include <imtbase/IComplexCollectionFilter.h>
#include <imtauth/CRoleCollectionAdapterComp.h>
#include <imtauth/CUserInfo.h>
#include <imtauthgql/CLdapAuthorizationControllerComp.h>
#include <imtauthgql/imtauthgql.h>
#include <imtgql/CGqlContext.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>


namespace
{
const QByteArray Product("Product");
const QByteArray OtherProduct("OtherProduct");
const QByteArray Login("DOMAIN\\member");
const QString GroupSid("S-1-5-21-1");
const QByteArray FirstRole("11111111-1111-4111-8111-111111111111");
const QByteArray SecondRole("22222222-2222-4222-8222-222222222222");
const QByteArray ForeignRole("33333333-3333-4333-8333-333333333333");
const QByteArray MemberObjectId("44444444-4444-4444-8444-444444444444");
const QByteArray NewMemberObjectId("55555555-5555-4555-8555-555555555555");
const QByteArray TenantA("66666666-6666-4666-8666-666666666666");
const QByteArray TenantB("77777777-7777-4777-8777-777777777777");


class CCollection:
			public icomp::CComponentBase,
			public imtbase::CObjectCollectionBase
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CCollection);
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
	I_END_COMPONENT;

	bool failWrite = false;
	bool failRead = false;

	virtual bool SetObjectData(const Id& id, const istd::IChangeable& value,
		CompatibilityMode mode = CM_WITHOUT_REFS, const imtbase::IOperationContext* context = nullptr) override
	{
		return !QUuid(QString::fromUtf8(id)).isNull() && !failWrite &&
			CObjectCollectionBase::SetObjectData(id, value, mode, context);
	}

	virtual bool GetObjectData(const Id& id, DataPtr& data, const iprm::IParamsSet* params = nullptr) const override
	{
		return !QUuid(QString::fromUtf8(id)).isNull() && !failRead &&
			CObjectCollectionBase::GetObjectData(id, data, params);
	}

	virtual Ids GetElementIds(int offset = 0, int count = -1, const iprm::IParamsSet* params = nullptr,
		ilog::IMessageConsumer* log = nullptr) const override
	{
		Ids result;
		iprm::TParamsPtr<imtbase::IComplexCollectionFilter> filter(params, "ComplexFilter", false);
		for (const QByteArray& id : CObjectCollectionBase::GetElementIds(0, -1, nullptr, log)){
			bool matches = true;
			if (filter.IsValid()){
				DataPtr data;
				if (!CObjectCollectionBase::GetObjectData(id, data)){
					return Ids();
				}
				for (const auto& field : filter->GetFilterExpression().fieldFilters){
					const auto* user = dynamic_cast<const imtauth::IUserInfo*>(data.GetPtr());
					const auto* role = dynamic_cast<const imtauth::IRole*>(data.GetPtr());
					if (field.fieldId == "Id" && user != nullptr){
						matches = matches && user->GetId() == field.filterValue.toByteArray();
					}
					else if (field.fieldId == "Sid" && user != nullptr){
						matches = matches && user->GetSid() == field.filterValue.toByteArray();
					}
					else if (field.fieldId == "ProductId" && role != nullptr){
						matches = matches && role->GetProductId() == field.filterValue.toByteArray();
					}
					else{
						// Unknown columns do not match, like a strict SQL backend.
						matches = false;
					}
				}
			}
			if (matches){ result.append(id); }
		}
		return result.mid(offset, count);
	}
};


class CUser: public imtauth::CIdentifiableUserInfo
{
public:
	void SetProvider(const imtauth::IRoleInfoProvider* provider) { m_roleProviderPtr = provider; }
};

class CBindings:
			public icomp::CComponentBase,
			virtual public imtauth::ITenantEntityBindingManager
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CBindings);
		I_REGISTER_INTERFACE(imtauth::ITenantEntityBindingManager);
	I_END_COMPONENT;

	virtual bool AddBinding(const QByteArray& tenant, const QByteArray& type, const QByteArray& id) override
	{
		if (QUuid(QString::fromUtf8(tenant)).isNull() || type.isEmpty() || QUuid(QString::fromUtf8(id)).isNull()){ return false; }
		m_bindings[type][id].insert(tenant);
		return true;
	}
	virtual bool RemoveBinding(const QByteArray& tenant, const QByteArray& type, const QByteArray& id) override
	{
		m_bindings[type][id].remove(tenant);
		return true;
	}
	virtual bool RemoveAllBindingsForEntity(const QByteArray& type, const QByteArray& id) override
	{
		m_bindings[type].remove(id);
		return true;
	}
	virtual bool RemoveAllBindingsForTenant(const QByteArray& tenant, const QByteArray& type) override
	{
		for (auto it = m_bindings[type].begin(); it != m_bindings[type].end(); ++it){ it.value().remove(tenant); }
		return true;
	}
	virtual bool HasBinding(const QByteArray& tenant, const QByteArray& type, const QByteArray& id) const override
	{
		return m_bindings.value(type).value(id).contains(tenant);
	}
	virtual bool HasAnyTenantBinding(const QByteArray& type, const QByteArray& id) const override
	{
		return !m_bindings.value(type).value(id).isEmpty();
	}
	virtual QByteArrayList GetEntityIds(const QByteArray& tenant, const QByteArray& type) const override
	{
		QByteArrayList ids;
		const auto entities = m_bindings.value(type);
		for (auto it = entities.begin(); it != entities.end(); ++it){
			if (it.value().contains(tenant)){ ids.append(it.key()); }
		}
		return ids;
	}
private:
	QMap<QByteArray, QMap<QByteArray, QSet<QByteArray>>> m_bindings;
};


class CAdmin: public imtauth::CUserInfo
{
public:
	virtual bool IsAdmin() const override { return true; }
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override
	{
		istd::IChangeableUniquePtr clone(new CAdmin);
		if (!clone->CopyFrom(*this, mode)){ return nullptr; }
		return clone;
	}
};


class CCredentials: virtual public imtauth::ICredentialController
{
public:
	virtual bool CheckCredential(const QByteArray& login, const QByteArray& password) const override
	{
		return login == Login && password == "valid";
	}
};

typedef icomp::TMakeComponentWrap<CCredentials, imtauth::ICredentialController> CCredentialsComp;
typedef icomp::TMakeComponentWrap<iprm::CEnableableParam, iprm::IEnableableParam> CEnabledComp;


class CController: public imtauthgql::CLdapAuthorizationControllerComp
{
public:
	typedef imtauthgql::CLdapAuthorizationControllerComp BaseClass;
	I_BEGIN_COMPONENT(CController);
	I_END_COMPONENT;

	using BaseClass::PrepareAuthorization;
	using BaseClass::OnAuthorization;
	using BaseClass::OnUserToken;
	bool failDirectory = false;
	const imtauth::IRoleInfoProvider* userRoleProvider = nullptr;
	QByteArrayList memberships{GroupSid.toUtf8()};

	virtual bool DiscoverDirectoryGroups(const QString& pattern, QMap<QByteArray, QString>& groups) const override
	{
		const QMap<QByteArray, QString> available{{GroupSid.toUtf8(), QStringLiteral("Product-Operators")},
			{"S-1-5-21-3", QStringLiteral("Product-Reviewers")}};
		QString expression = QRegularExpression::escape(pattern);
		expression.replace("\\*", ".*");
		QRegularExpression matcher("\\A" + expression + "\\z", QRegularExpression::CaseInsensitiveOption);
		groups.clear();
		for (auto it = available.begin(); it != available.end(); ++it){
			if (matcher.match(it.value()).hasMatch()){ groups.insert(it.key(), it.value()); }
		}
		return !pattern.isEmpty() && pattern.size() <= 256 && !failDirectory;
	}
	virtual bool ReadDirectoryMembership(const QByteArray& userSid, QByteArrayList& groups) const override
	{
		groups = memberships;
		return userSid == "S-1-5-21-2" && !failDirectory;
	}
	virtual QByteArray GetDirectoryUserSid(const QByteArray&) const override
	{
		return QByteArray("S-1-5-21-2");
	}
	virtual istd::TUniqueInterfacePtr<imtauth::IUserInfo> CreateUserInfoFromLdapUser(const QByteArray& login) const override
	{
		CUser* user = new CUser;
		user->SetObjectUuid(NewMemberObjectId);
		user->SetId(login);
		user->SetName("New member");
		user->SetSid("S-1-5-21-2");
		user->SetMail("member@example.invalid");
		user->SetProvider(userRoleProvider);
		return user;
	}
};


class CFixture
{
public:
	CFixture()
	{
		fileName = QStringLiteral(".ldap-group-mapping-%1.json").arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
		users->InitComponent();
		roles->InitComponent();
		provider->SetRef("RoleCollection", roles);
		provider->InitComponent();
		controller->userRoleProvider = &provider.GetImpl();
		enabled->SetEnabled(true);
		enabled->InitComponent();
		credentials->InitComponent();
		controller->SetRef("UserCollection", users);
		controller->SetRef("RoleCollection", roles);
		controller->SetRef("LdapEnableableParam", enabled);
		controller->SetStringAttr("LdapGroupSettingsFile", QDir::current().absoluteFilePath(fileName));
		controller->InsertMultiRef("CredentialControllers", credentials);
		controller->InsertMultiAttr("SystemIds", QByteArray("Ldap"));
		controller->InitComponent();
		AddRole(FirstRole, Product, "/First");
		AddRole(SecondRole, Product, "/Second");
		AddRole(ForeignRole, OtherProduct, "/Foreign");
		user.SetObjectUuid(MemberObjectId);
		user.SetId(Login);
		user.SetName("Member");
		user.SetMail("member@example.invalid");
		user.SetSid("S-1-5-21-2");
		user.SetProvider(&provider.GetImpl());
		user.AddRole(Product, FirstRole);
		imtauth::IUserInfo::SystemInfo system;
		system.systemId = "Ldap";
		system.systemName = "LDAP";
		system.enabled = true;
		user.AddToSystem(system);
		users->InsertNewObject("User", "Member", "", &user, MemberObjectId);
		WriteSettings();
	}

	~CFixture()
	{
		QFile::remove(fileName);
		QFile::remove(fileName + ".lock");
	}

	void AddRole(const QByteArray& id, const QByteArray& product, const QByteArray& permission)
	{
		imtauth::CIdentifiableRoleInfo role;
		role.SetObjectUuid(id);
		role.SetRoleId(id);
		role.SetRoleName(QString::fromUtf8(id));
		role.SetProductId(product);
		role.SetLocalPermissions({permission});
		roles->InsertNewObject("Role", QString::fromUtf8(id), "", &role, id);
	}

	QJsonObject Settings(bool on = true, const QByteArray& role = SecondRole) const
	{
		return QJsonObject{{"enabled", on}, {"pattern", "Product-*"},
			{"mappings", QJsonObject{{GroupSid, QJsonArray{QString::fromUtf8(FirstRole), QString::fromUtf8(role)}}}}};
	}

	bool WriteSettings(bool on = true)
	{
		QFile file(fileName);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){ return false; }
		QByteArray data = QJsonDocument(QJsonObject{{"version", 1},
			{"products", QJsonObject{{QString::fromUtf8(Product), Settings(on)}}}}).toJson();
		return file.write(data) == data.size();
	}

	bool Refresh(QString& error)
	{
		return controller->PrepareAuthorization(user, Product, error);
	}

	imtauth::CUserInfo* Stored(imtbase::IObjectCollection::DataPtr& data)
	{
		if (!users->GetObjectData(MemberObjectId, data)){ return nullptr; }
		return dynamic_cast<imtauth::CUserInfo*>(data.GetPtr());
	}

	sdl::V1_0::imtauth::CAuthorizationPayload SignIn(const QByteArray& command, QString& error)
	{
		imtgql::CGqlRequest request(imtgql::IGqlRequest::RT_QUERY, command);
		imtgql::CGqlParamObject input;
		input.InsertParam("login", QString::fromUtf8(Login));
		input.InsertParam("password", QStringLiteral("valid"));
		input.InsertParam("productId", Product);
		request.AddParam("input", input);
		if (command == "UserToken"){
			sdl::V1_0::imtauth::CUserTokenGqlRequest typed(request, false);
			return controller->OnUserToken(typed, request, error);
		}
		sdl::V1_0::imtauth::CAuthorizationGqlRequest typed(request, false);
		return controller->OnAuthorization(typed, request, error);
	}

	QJsonObject AdminRequest(const QByteArray& command, bool superuser, const QJsonObject& settings, QString& error)
	{
		imtgql::CGqlRequest request(command == "SaveLdapGroupSettings" ? imtgql::IGqlRequest::RT_MUTATION : imtgql::IGqlRequest::RT_QUERY, command);
		imtgql::CGqlParamObject input;
		input.InsertParam("productId", Product);
		input.InsertParam("pattern", QStringLiteral("Product-*"));
		input.InsertParam("settings", QString::fromUtf8(QJsonDocument(settings).toJson()));
		request.AddParam("input", input);
		imtgql::IGqlContextSharedPtr context(new imtgql::CGqlContext);
		if (superuser){ context->SetUserInfo(&admin); }
		request.SetGqlContext(context);
		return controller->CreateResponse(request, error);
	}

	QString fileName;
	CUser user;
	CAdmin admin;
	icomp::TSimSharedComponentPtr<CCollection> users;
	icomp::TSimSharedComponentPtr<CCollection> roles;
	icomp::TSimSharedComponentPtr<imtauth::CRoleCollectionAdapterComp> provider;
	icomp::TSimSharedComponentPtr<CEnabledComp> enabled;
	icomp::TSimSharedComponentPtr<CCredentialsComp> credentials;
	icomp::TSimSharedComponentPtr<CController> controller;
};
}


void CLdapGroupMappingTest::testAllMatchingRolesAndProductIsolation()
{
	CFixture fixture;
	QString error;
	QJsonObject settings = fixture.Settings();
	settings.insert("mappings", QJsonObject{{GroupSid, QJsonArray{QString::fromUtf8(FirstRole)}},
		{"S-1-5-21-3", QJsonArray{QString::fromUtf8(SecondRole)}}});
	QVERIFY(!fixture.AdminRequest("SaveLdapGroupSettings", true, settings, error).isEmpty());
	fixture.controller->memberships.append("S-1-5-21-3");
	// A just-provisioned response object may not carry the persistent factory's provider yet.
	fixture.user.SetProvider(nullptr);
	QVERIFY2(fixture.Refresh(error), qPrintable(error));
	QCOMPARE(fixture.user.GetDirectoryRoles(Product), QByteArrayList({FirstRole, SecondRole}));
	QCOMPARE(fixture.user.GetRoles(Product), QByteArrayList({FirstRole}));
	QCOMPARE(fixture.user.GetPermissions(Product), QByteArrayList({"/First", "/Second"}));
	QVERIFY(fixture.user.GetPermissions(OtherProduct).isEmpty());
	fixture.user.SetRoles("urn:imt:directory-roles:Product", {ForeignRole});
	QCOMPARE(fixture.user.GetDirectoryRoles(Product), QByteArrayList({FirstRole, SecondRole}));
	QVERIFY(fixture.user.GetRoles("urn:imt:directory-roles:Product").isEmpty());
	QVERIFY(!fixture.user.RemoveProduct("urn:imt:directory-roles:Product"));
	fixture.user.SetDirectoryRoles(Product, {ForeignRole});
	QCOMPARE(fixture.user.GetPermissions(Product), QByteArrayList({"/First"}));
}


void CLdapGroupMappingTest::testRevocationKeepsManualAssignments()
{
	CFixture fixture;
	QString error;
	QVERIFY(fixture.Refresh(error));
	fixture.controller->memberships = {"S-1-5-21-99"};
	QVERIFY(fixture.Refresh(error));
	QVERIFY(fixture.user.GetDirectoryRoles(Product).isEmpty());
	QCOMPARE(fixture.user.GetRoles(Product), QByteArrayList({FirstRole}));
	imtbase::IObjectCollection::DataPtr data;
	auto* stored = fixture.Stored(data);
	QVERIFY(stored != nullptr);
	QVERIFY(stored->GetDirectoryRoles(Product).isEmpty());
	QCOMPARE(stored->GetPermissions(Product), QByteArrayList({"/First"}));
}


void CLdapGroupMappingTest::testDirectoryRolesSurviveArchiveCloneAndReset()
{
	CFixture fixture;
	fixture.user.SetDirectoryRoles(Product, {SecondRole});
	iser::CMemoryWriteArchive writer;
	QVERIFY(fixture.user.Serialize(writer));
	iser::CMemoryReadArchive reader(writer);
	imtauth::CIdentifiableUserInfo loaded;
	QVERIFY(loaded.Serialize(reader));
	QCOMPARE(loaded.GetDirectoryRoles(Product), QByteArrayList({SecondRole}));
	QCOMPARE(loaded.GetRoles(Product), QByteArrayList({FirstRole}));
	QCOMPARE(loaded.GetProducts(), QByteArrayList({Product}));
	auto clone = fixture.user.CloneMe();
	auto* copy = dynamic_cast<imtauth::CUserInfo*>(clone.GetPtr());
	QVERIFY(copy != nullptr);
	QCOMPARE(copy->GetPermissions(Product), QByteArrayList({"/First", "/Second"}));
	QVERIFY(copy->ResetData());
	QVERIFY(copy->GetDirectoryRoles(Product).isEmpty());
}


void CLdapGroupMappingTest::testBothLoginCommandsRefreshStoredPermissions()
{
	for (const QByteArray& command : QByteArrayList{"Authorization", "UserToken"}){
		CFixture fixture;
		QString error;
		auto payload = fixture.SignIn(command, error);
		QVERIFY2(error.isEmpty(), qPrintable(error));
		QVERIFY(payload.token.HasValue());
		imtbase::IObjectCollection::DataPtr data;
		auto* stored = fixture.Stored(data);
		QVERIFY(stored != nullptr);
		QVERIFY(stored->GetPermissions(Product).contains("/Second"));
		fixture.controller->memberships = {"S-1-5-21-99"};
		payload = fixture.SignIn(command, error);
		QVERIFY(payload.token.HasValue());
		stored = fixture.Stored(data);
		QVERIFY(stored != nullptr);
		QVERIFY(!stored->GetPermissions(Product).contains("/Second"));
	}
}


void CLdapGroupMappingTest::testDirectoryFailureDeniesBothLoginCommands()
{
	for (const QByteArray& command : QByteArrayList{"Authorization", "UserToken"}){
		CFixture fixture;
		QString error;
		QVERIFY(fixture.Refresh(error));
		fixture.controller->failDirectory = true;
		auto payload = fixture.SignIn(command, error);
		QVERIFY(!error.isEmpty());
		QVERIFY(!payload.token.HasValue());
	}
}

void CLdapGroupMappingTest::testBothLoginCommandsProvisionNewUsers()
{
	for (const QByteArray& command : QByteArrayList{"Authorization", "UserToken"}){
		CFixture fixture;
		QVERIFY(fixture.users->RemoveElements({MemberObjectId}));
		QString error;
		auto payload = fixture.SignIn(command, error);
		QVERIFY2(error.isEmpty(), qPrintable(error));
		QVERIFY(payload.token.HasValue());
		imtbase::IObjectCollection::DataPtr data;
		QVERIFY(fixture.users->GetObjectData(NewMemberObjectId, data));
		auto* user = dynamic_cast<imtauth::CUserInfo*>(data.GetPtr());
		QVERIFY(user != nullptr);
		QCOMPARE(user->GetDirectoryRoles(Product), QByteArrayList({FirstRole, SecondRole}));
		QVERIFY(user->GetPermissions(Product).contains("/First"));
		QVERIFY(user->GetPermissions(Product).contains("/Second"));
	}
}


void CLdapGroupMappingTest::testFailedCollectionReadsDenyLogin()
{
	for (bool userRead : {false, true}){
		CFixture fixture;
		fixture.users->failRead = userRead;
		fixture.roles->failRead = !userRead;
		QString error;
		auto payload = fixture.SignIn("UserToken", error);
		QVERIFY(!payload.token.HasValue());
		QVERIFY(!error.isEmpty());
	}
}


void CLdapGroupMappingTest::testFailedPersistenceDeniesLogin()
{
	CFixture fixture;
	fixture.users->failWrite = true;
	QString error;
	auto payload = fixture.SignIn("UserToken", error);
	QVERIFY(!payload.token.HasValue());
	QVERIFY(!error.isEmpty());
}


void CLdapGroupMappingTest::testDisabledMappingRevokesOnlyDirectoryRoles()
{
	CFixture fixture;
	QString error;
	QVERIFY(fixture.Refresh(error));
	QVERIFY(!fixture.AdminRequest("SaveLdapGroupSettings", true, fixture.Settings(false, "removed-role"), error).isEmpty());
	fixture.controller->failDirectory = true;
	fixture.roles->failRead = true;
	QVERIFY(fixture.Refresh(error));
	QVERIFY(fixture.user.GetDirectoryRoles(Product).isEmpty());
	QCOMPARE(fixture.user.GetRoles(Product), QByteArrayList({FirstRole}));
}


void CLdapGroupMappingTest::testCorruptSettingsDenyLogin()
{
	CFixture fixture;
	QFile file(fixture.fileName);
	QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
	QCOMPARE(file.write("invalid"), qint64(7));
	file.close();
	QString error;
	auto payload = fixture.SignIn("Authorization", error);
	QVERIFY(!payload.token.HasValue());
	QVERIFY(!error.isEmpty());
	QVERIFY(QFile::remove(fixture.fileName));
	error.clear();
	payload = fixture.SignIn("UserToken", error);
	QVERIFY(!payload.token.HasValue());
	QVERIFY(!error.isEmpty());
}


void CLdapGroupMappingTest::testAdministrationRequiresSuperuser()
{
	CFixture fixture;
	for (const QByteArray& command : QByteArrayList{"LdapGroupSettings", "SaveLdapGroupSettings", "DiscoverLdapGroups"}){
		QString error;
		QVERIFY(fixture.AdminRequest(command, false, fixture.Settings(), error).isEmpty());
		QVERIFY(!error.isEmpty());
	}
	QString error;
	QVERIFY(!fixture.AdminRequest("DiscoverLdapGroups", true, fixture.Settings(), error).isEmpty());
	QVERIFY(error.isEmpty());
}


void CLdapGroupMappingTest::testSettingsPersistAndKeepOtherProducts()
{
	CFixture fixture;
	QFile file(fixture.fileName);
	QVERIFY(file.open(QIODevice::ReadOnly));
	QJsonObject document = QJsonDocument::fromJson(file.readAll()).object();
	file.close();
	QJsonObject products = document.value("products").toObject();
	products.insert(QString::fromUtf8(OtherProduct), QJsonObject{{"enabled", false}});
	document.insert("products", products);
	QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
	file.write(QJsonDocument(document).toJson());
	file.close();
	QString error;
	QVERIFY(!fixture.AdminRequest("SaveLdapGroupSettings", true, fixture.Settings(false), error).isEmpty());
	QVERIFY2(error.isEmpty(), qPrintable(error));
	QVERIFY(file.open(QIODevice::ReadOnly));
	document = QJsonDocument::fromJson(file.readAll()).object();
	QVERIFY(document.value("products").toObject().contains(QString::fromUtf8(OtherProduct)));
	QVERIFY(!document.value("products").toObject().value(QString::fromUtf8(Product)).toObject().value("enabled").toBool());
}


void CLdapGroupMappingTest::testForeignProductRoleIsRefused()
{
	CFixture fixture;
	QString error;
	QVERIFY(fixture.AdminRequest("SaveLdapGroupSettings", true, fixture.Settings(true, ForeignRole), error).isEmpty());
	QVERIFY(!error.isEmpty());
}

void CLdapGroupMappingTest::testInheritedRolesStayProductScoped()
{
	CFixture fixture;
	imtbase::IObjectCollection::DataPtr data;
	QVERIFY(fixture.roles->GetObjectData(SecondRole, data));
	auto* role = dynamic_cast<imtauth::IRole*>(data.GetPtr());
	QVERIFY(role != nullptr);
	role->IncludeRole(ForeignRole);
	QVERIFY(fixture.roles->SetObjectData(SecondRole, *role));
	fixture.user.SetDirectoryRoles(Product, {SecondRole});
	QCOMPARE(fixture.user.GetPermissions(Product), QByteArrayList({"/First"}));
	QString error;
	QVERIFY(!fixture.Refresh(error));
	QVERIFY(!error.isEmpty());
}


void CLdapGroupMappingTest::testDisabledMappingAllowsLegacyProductlessLogin()
{
	CFixture fixture;
	QVERIFY(fixture.WriteSettings(false));
	QString error;
	QVERIFY(fixture.controller->PrepareAuthorization(fixture.user, QByteArray(), error));
	QVERIFY(error.isEmpty());
	QVERIFY(fixture.WriteSettings());
	QVERIFY(!fixture.controller->PrepareAuthorization(fixture.user, QByteArray(), error));
	QVERIFY(!error.isEmpty());
}


void CLdapGroupMappingTest::testIdentityAndProviderFailuresDenyMappedGrants()
{
	CFixture fixture;
	fixture.user.SetSid("S-1-5-21-99");
	QString error;
	QVERIFY(!fixture.Refresh(error));
	QVERIFY(!error.isEmpty());
	QVERIFY(fixture.user.GetDirectoryRoles(Product).isEmpty());
	fixture.user.SetSid("S-1-5-21-2");
	CUser stored;
	QVERIFY(stored.CopyFrom(fixture.user));
	stored.SetProvider(nullptr);
	QVERIFY(fixture.users->SetObjectData(MemberObjectId, stored));
	error.clear();
	QVERIFY(!fixture.Refresh(error));
	QVERIFY(!error.isEmpty());
	QVERIFY(fixture.user.GetDirectoryRoles(Product).isEmpty());
}


void CLdapGroupMappingTest::testDirectoryRolesRespectTenantBindings()
{
	CFixture fixture;
	fixture.user.SetDirectoryRoles(Product, {FirstRole, SecondRole});
	CBindings bindings;
	QVERIFY(bindings.AddBinding(TenantA, "Roles", FirstRole));
	QVERIFY(bindings.AddBinding(TenantB, "Roles", SecondRole));
	auto permissionsA = imtauthgql::GetEffectiveUserPermissions(MemberObjectId, fixture.user,
		TenantA, Product, &bindings, nullptr, nullptr, &fixture.provider.GetImpl());
	auto permissionsB = imtauthgql::GetEffectiveUserPermissions(MemberObjectId, fixture.user,
		TenantB, Product, &bindings, nullptr, nullptr, &fixture.provider.GetImpl());
	auto globalPermissions = imtauthgql::GetEffectiveUserPermissions(MemberObjectId, fixture.user,
		QByteArray(), Product, &bindings, nullptr, nullptr, &fixture.provider.GetImpl());
	QCOMPARE(permissionsA, QByteArrayList({"/First"}));
	QCOMPARE(permissionsB, QByteArrayList({"/Second"}));
	QVERIFY(globalPermissions.isEmpty());
	QCOMPARE(fixture.user.GetDirectoryRoles(Product), QByteArrayList({FirstRole, SecondRole}));
	QCOMPARE(fixture.user.GetRoles(Product), QByteArrayList({FirstRole}));
}


I_ADD_TEST(CLdapGroupMappingTest);
