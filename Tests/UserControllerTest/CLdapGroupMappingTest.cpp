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
#include <imtauth/CLdapGroupChangeListener.h>
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
const QByteArray TenantBoundParentRole("88888888-8888-4888-8888-888888888888");
const QByteArray MemberObjectId("44444444-4444-4444-8444-444444444444");
const QByteArray NewMemberObjectId("55555555-5555-4555-8555-555555555555");
const QByteArray TenantA("66666666-6666-4666-8666-666666666666");
const QByteArray TenantB("77777777-7777-4777-8777-777777777777");


class CUser: public imtauth::CIdentifiableUserInfo
{
public:
	void SetProvider(const imtauth::IRoleInfoProvider* provider) { m_roleProviderPtr = provider; }
};


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
	// Emulates the user factory of a persistent collection, which attaches the role provider on load.
	const imtauth::IRoleInfoProvider* userRoleProvider = nullptr;

	virtual bool SetObjectData(const Id& id, const istd::IChangeable& value,
		CompatibilityMode mode = CM_WITHOUT_REFS, const imtbase::IOperationContext* context = nullptr) override
	{
		return !QUuid(QString::fromUtf8(id)).isNull() && !failWrite &&
			CObjectCollectionBase::SetObjectData(id, value, mode, context);
	}

	virtual bool GetObjectData(const Id& id, DataPtr& data, const iprm::IParamsSet* params = nullptr) const override
	{
		if (QUuid(QString::fromUtf8(id)).isNull() || failRead || !CObjectCollectionBase::GetObjectData(id, data, params)){
			return false;
		}
		const auto* user = dynamic_cast<const imtauth::CIdentifiableUserInfo*>(data.GetPtr());
		if (user == nullptr || user->GetRoleProvider() != nullptr || userRoleProvider == nullptr){
			return true;
		}
		istd::IChangeableUniquePtr loaded(new CUser);
		if (!loaded->CopyFrom(*user)){
			return false;
		}
		static_cast<CUser*>(loaded.GetPtr())->SetProvider(userRoleProvider);
		data.FromUnique(std::move(loaded));
		return true;
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
	using BaseClass::OnDirectoryChangeEvent;
	bool failDirectory = false;
	QByteArrayList memberships{GroupSid.toUtf8()};
	QMap<QByteArray, QString> available{{GroupSid.toUtf8(), QStringLiteral("Product-Operators")},
		{"S-1-5-21-3", QStringLiteral("Product-Reviewers")}};

	virtual bool DiscoverDirectoryGroups(const QString& pattern, QMap<QByteArray, QString>& groups) const override
	{
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
		return user;
	}
};


class CFixture
{
public:
	explicit CFixture(bool notifications = false)
	{
		fileName = QStringLiteral(".ldap-group-mapping-%1.json").arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
		users->InitComponent();
		roles->InitComponent();
		provider->SetRef("RoleCollection", roles);
		provider->InitComponent();
		users->userRoleProvider = &provider.GetImpl();
		enabled->SetEnabled(true);
		enabled->InitComponent();
		credentials->InitComponent();
		controller->SetRef("UserCollection", users);
		controller->SetRef("RoleCollection", roles);
		controller->SetRef("LdapEnableableParam", enabled);
		controller->SetStringAttr("LdapGroupSettingsFile", QDir::current().absoluteFilePath(fileName));
		// A real listener would connect to the domain of a Windows test machine.
		controller->SetBoolAttr("LdapGroupChangeNotifications", notifications);
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
		QFile::remove(fileName + ".groups.json");
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

	QJsonObject AdminRequest(const QByteArray& command, bool superuser, const QJsonObject& settings, QString& error,
		const QByteArray& productId = Product)
	{
		imtgql::CGqlRequest request(command == "SaveLdapGroupSettings" ? imtgql::IGqlRequest::RT_MUTATION : imtgql::IGqlRequest::RT_QUERY, command);
		imtgql::CGqlParamObject input;
		input.InsertParam("productId", productId);
		input.InsertParam("pattern", QStringLiteral("Product-*"));
		input.InsertParam("settings", QString::fromUtf8(QJsonDocument(settings).toJson()));
		request.AddParam("input", input);
		imtgql::IGqlContextSharedPtr context(new imtgql::CGqlContext);
		if (superuser){ context->SetUserInfo(&admin); }
		request.SetGqlContext(context);
		return controller->CreateResponse(request, error);
	}

	QJsonObject GroupSettings(const QByteArray& productId = Product)
	{
		QString error;
		QJsonObject data = AdminRequest("LdapGroupSettings", true, QJsonObject(), error, productId).value("data").toObject();
		if (!error.isEmpty()){
			return QJsonObject();
		}

		return data;
	}

	static QMap<QString, QString> Groups(const QJsonObject& data)
	{
		QMap<QString, QString> groups;
		for (const QJsonValue& row : data.value("groups").toArray()){
			groups.insert(row.toObject().value("id").toString(), row.toObject().value("name").toString());
		}

		return groups;
	}

	static QStringList GroupIds(const QJsonObject& data)
	{
		QStringList ids;
		for (const QJsonValue& row : data.value("groups").toArray()){
			ids.append(row.toObject().value("id").toString());
		}
		ids.sort();

		return ids;
	}

	void Notify(imtauth::CLdapGroupChangeListener::Event event, const QString& message = QString())
	{
		controller->OnDirectoryChangeEvent(event, message);
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


void CLdapGroupMappingTest::testGlobalDirectoryRoleInheritanceRespectsTenantBindings()
{
	CFixture fixture;
	fixture.AddRole(TenantBoundParentRole, Product, "/TenantBound");
	imtbase::IObjectCollection::DataPtr roleData;
	QVERIFY(fixture.roles->GetObjectData(FirstRole, roleData));
	imtauth::IRole* role = dynamic_cast<imtauth::IRole*>(roleData.GetPtr());
	QVERIFY(role != nullptr);
	QVERIFY(role->IncludeRole(TenantBoundParentRole));
	QVERIFY(fixture.roles->SetObjectData(FirstRole, *role));
	fixture.user.SetDirectoryRoles(Product, {FirstRole});

	CBindings bindings;
	QVERIFY(bindings.AddBinding(TenantB, "Roles", TenantBoundParentRole));
	auto globalPermissions = imtauthgql::GetEffectiveUserPermissions(MemberObjectId, fixture.user,
		QByteArray(), Product, &bindings, nullptr, nullptr, &fixture.provider.GetImpl());

	QCOMPARE(globalPermissions, QByteArrayList({"/First"}));
}


void CLdapGroupMappingTest::testNotificationsSynchronizeGroupList()
{
	CFixture fixture;
	QVERIFY(CFixture::Groups(fixture.GroupSettings()).isEmpty());
	fixture.Notify(imtauth::CLdapGroupChangeListener::E_CONNECTED);
	QJsonObject data = fixture.GroupSettings();
	QCOMPARE(data.value("notificationState").toString(), QStringLiteral("Connected"));
	QVERIFY(!data.value("synchronizedAt").toString().isEmpty());
	QVERIFY(data.value("synchronizationError").toString().isEmpty());
	QCOMPARE(CFixture::Groups(data), (QMap<QString, QString>{
		{GroupSid, QStringLiteral("Product-Operators")},
		{QStringLiteral("S-1-5-21-3"), QStringLiteral("Product-Reviewers")}}));

	// Rename, creation, and a new group outside the pattern.
	fixture.controller->available.insert("S-1-5-21-3", QStringLiteral("Product-Auditors"));
	fixture.controller->available.insert("S-1-5-21-4", QStringLiteral("Product-New"));
	fixture.controller->available.insert("S-1-5-21-5", QStringLiteral("Other-Group"));
	fixture.Notify(imtauth::CLdapGroupChangeListener::E_GROUPS_CHANGED);
	QCOMPARE(CFixture::Groups(fixture.GroupSettings()), (QMap<QString, QString>{
		{GroupSid, QStringLiteral("Product-Operators")},
		{QStringLiteral("S-1-5-21-3"), QStringLiteral("Product-Auditors")},
		{QStringLiteral("S-1-5-21-4"), QStringLiteral("Product-New")}}));

	// Deletion of a mapped group and a rename out of the pattern.
	fixture.controller->available.remove(GroupSid.toUtf8());
	fixture.controller->available.insert("S-1-5-21-3", QStringLiteral("Retired-Auditors"));
	fixture.Notify(imtauth::CLdapGroupChangeListener::E_GROUPS_CHANGED);
	data = fixture.GroupSettings();
	QCOMPARE(CFixture::Groups(data), (QMap<QString, QString>{{QStringLiteral("S-1-5-21-4"), QStringLiteral("Product-New")}}));

	// The administrator-owned mapping of the deleted group stays; the new group is not mapped automatically.
	QJsonObject settings = QJsonDocument::fromJson(data.value("settings").toString().toUtf8()).object();
	QCOMPARE(settings.value("mappings").toObject(), fixture.Settings().value("mappings").toObject());
	imtbase::IObjectCollection::DataPtr stored;
	QVERIFY(fixture.Stored(stored) != nullptr);
	QCOMPARE(fixture.Stored(stored)->GetRoles(Product), QByteArrayList({FirstRole}));
	QVERIFY(fixture.Stored(stored)->GetDirectoryRoles(Product).isEmpty());

	// The deleted group no longer grants roles at sign-in; manual roles are kept.
	QString error;
	QVERIFY2(fixture.Refresh(error), qPrintable(error));
	QVERIFY(fixture.user.GetDirectoryRoles(Product).isEmpty());
	QCOMPARE(fixture.user.GetRoles(Product), QByteArrayList({FirstRole}));
}


void CLdapGroupMappingTest::testFailedDirectoryReadKeepsGroupList()
{
	CFixture fixture;
	fixture.Notify(imtauth::CLdapGroupChangeListener::E_CONNECTED);
	QCOMPARE(int(CFixture::Groups(fixture.GroupSettings()).size()), 2);
	fixture.controller->available.remove(GroupSid.toUtf8());
	fixture.controller->failDirectory = true;
	fixture.Notify(imtauth::CLdapGroupChangeListener::E_GROUPS_CHANGED);
	QJsonObject data = fixture.GroupSettings();
	QCOMPARE(int(CFixture::Groups(data).size()), 2);
	QVERIFY(!data.value("synchronizationError").toString().isEmpty());
	fixture.controller->failDirectory = false;
	fixture.Notify(imtauth::CLdapGroupChangeListener::E_GROUPS_CHANGED);
	data = fixture.GroupSettings();
	QCOMPARE(CFixture::GroupIds(data), QStringList({QStringLiteral("S-1-5-21-3")}));
	QVERIFY(data.value("synchronizationError").toString().isEmpty());
}


void CLdapGroupMappingTest::testCorruptCatalogRecoversOnRead()
{
	CFixture fixture;
	QFile catalog(fixture.fileName + QStringLiteral(".groups.json"));
	QVERIFY(catalog.open(QIODevice::WriteOnly | QIODevice::Truncate));
	QCOMPARE(catalog.write(QByteArrayLiteral("{invalid")), qint64(8));
	catalog.close();

	QJsonObject data = fixture.GroupSettings();

	QCOMPARE(CFixture::GroupIds(data), QStringList({GroupSid, QStringLiteral("S-1-5-21-3")}));
}


void CLdapGroupMappingTest::testSaveWithCorruptCatalogRecoversAllProducts()
{
	CFixture fixture;
	fixture.controller->available.insert("S-1-5-21-5", QStringLiteral("Other-Group"));
	QFile settingsFile(fixture.fileName);
	QVERIFY(settingsFile.open(QIODevice::WriteOnly | QIODevice::Truncate));
	QJsonObject otherSettings{{"enabled", false}, {"pattern", "Other-*"}, {"mappings", QJsonObject()}};
	QByteArray settingsData = QJsonDocument(QJsonObject{{"version", 1}, {"products", QJsonObject{
		{QString::fromUtf8(Product), fixture.Settings()}, {QString::fromUtf8(OtherProduct), otherSettings}}}}).toJson();
	QCOMPARE(settingsFile.write(settingsData), qint64(settingsData.size()));
	settingsFile.close();

	QFile catalog(fixture.fileName + QStringLiteral(".groups.json"));
	QVERIFY(catalog.open(QIODevice::WriteOnly | QIODevice::Truncate));
	QCOMPARE(catalog.write(QByteArrayLiteral("{invalid")), qint64(8));
	catalog.close();

	QString error;
	fixture.AdminRequest("SaveLdapGroupSettings", true, fixture.Settings(false), error);
	QVERIFY(error.isEmpty());
	QCOMPARE(CFixture::GroupIds(fixture.GroupSettings(OtherProduct)), QStringList({QStringLiteral("S-1-5-21-5")}));
}


void CLdapGroupMappingTest::testReconnectResynchronizesGroupList()
{
	CFixture fixture;
	fixture.Notify(imtauth::CLdapGroupChangeListener::E_CONNECTED);
	fixture.controller->available.insert("S-1-5-21-3", QStringLiteral("Product-Auditors"));
	fixture.Notify(imtauth::CLdapGroupChangeListener::E_DISCONNECTED, QStringLiteral("Connection lost"));
	QJsonObject data = fixture.GroupSettings();
	QCOMPARE(data.value("notificationState").toString(), QStringLiteral("Disconnected"));
	QVERIFY(data.value("synchronizationError").toString().contains(QStringLiteral("Connection lost")));
	QCOMPARE(CFixture::Groups(data).value(QStringLiteral("S-1-5-21-3")), QStringLiteral("Product-Reviewers"));
	fixture.Notify(imtauth::CLdapGroupChangeListener::E_CONNECTED);
	data = fixture.GroupSettings();
	QCOMPARE(data.value("notificationState").toString(), QStringLiteral("Connected"));
	QCOMPARE(CFixture::Groups(data).value(QStringLiteral("S-1-5-21-3")), QStringLiteral("Product-Auditors"));
}


void CLdapGroupMappingTest::testGroupListIsProductScopedAndServerOwned()
{
	CFixture fixture;
	QFile file(fixture.fileName);
	QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
	QJsonObject other{{"enabled", false}, {"pattern", "Other-*"}, {"mappings", QJsonObject()}};
	QByteArray document = QJsonDocument(QJsonObject{{"version", 1}, {"products", QJsonObject{
		{QString::fromUtf8(Product), fixture.Settings()}, {QString::fromUtf8(OtherProduct), other}}}}).toJson();
	QCOMPARE(file.write(document), qint64(document.size()));
	file.close();
	fixture.controller->available.insert("S-1-5-21-5", QStringLiteral("Other-Group"));
	fixture.Notify(imtauth::CLdapGroupChangeListener::E_CONNECTED);
	QCOMPARE(CFixture::GroupIds(fixture.GroupSettings(OtherProduct)), QStringList({QStringLiteral("S-1-5-21-5")}));
	QCOMPARE(int(CFixture::Groups(fixture.GroupSettings()).size()), 2);

	// A client cannot store group names; the list follows the saved pattern.
	QJsonObject settings = fixture.Settings();
	settings.insert("pattern", QStringLiteral("Product-Op*"));
	settings.insert("groups", QJsonObject{{QStringLiteral("S-1-5-21-9"), QStringLiteral("Forged")}});
	QString error;
	QJsonObject data = fixture.AdminRequest("SaveLdapGroupSettings", true, settings, error).value("data").toObject();
	QVERIFY2(error.isEmpty(), qPrintable(error));
	QCOMPARE(CFixture::GroupIds(data), QStringList({GroupSid}));
	QVERIFY(!QJsonDocument::fromJson(data.value("settings").toString().toUtf8()).object().contains("groups"));
	QCOMPARE(CFixture::GroupIds(fixture.GroupSettings()), QStringList({GroupSid}));
	QCOMPARE(CFixture::GroupIds(fixture.GroupSettings(OtherProduct)), QStringList({QStringLiteral("S-1-5-21-5")}));
	QVERIFY(file.open(QIODevice::ReadOnly));
	QJsonObject saved = QJsonDocument::fromJson(file.readAll()).object().value("products").toObject();
	QVERIFY(!saved.value(QString::fromUtf8(Product)).toObject().contains("groups"));
}


void CLdapGroupMappingTest::testUnsupportedNotificationsAreReported()
{
	CFixture disabled;
	QCOMPARE(disabled.GroupSettings().value("notificationState").toString(), QStringLiteral("Disabled"));
#ifdef Q_OS_WIN
	QSKIP("Windows uses the Active Directory change-notification listener");
#else
	QVERIFY(!imtauth::CLdapGroupChangeListener::IsSupported());
	imtauth::CLdapGroupChangeListener listener;
	QString error;
	QVERIFY(!listener.Start([](imtauth::CLdapGroupChangeListener::Event, const QString&){}, error));
	QVERIFY(!error.isEmpty());
	QVERIFY(!listener.IsRunning());
	CFixture fixture(true);
	QJsonObject data = fixture.GroupSettings();
	QCOMPARE(data.value("notificationState").toString(), QStringLiteral("Unsupported"));
	QVERIFY(!data.value("synchronizationError").toString().isEmpty());
	QVERIFY(data.value("synchronizedAt").toString().isEmpty());
	QVERIFY(CFixture::Groups(data).isEmpty());
#endif
}


void CLdapGroupMappingTest::testDirectoryOnlyProductKeepsDefaultRoleProvisioning()
{
	CFixture fixture;
	QVERIFY(fixture.user.RemoveRole(Product, FirstRole));
	fixture.user.SetDirectoryRoles(Product, {FirstRole, SecondRole});
	QVERIFY(fixture.users->SetObjectData(MemberObjectId, fixture.user));
	QString error;
	auto payload = fixture.SignIn("Authorization", error);
	QVERIFY2(error.isEmpty(), qPrintable(error));
	QVERIFY(payload.token.HasValue());
	imtbase::IObjectCollection::DataPtr data;
	auto* stored = fixture.Stored(data);
	QVERIFY(stored != nullptr);
	QVERIFY(stored->HasManualProduct(Product));
	QCOMPARE(stored->GetRoles(Product).size(), 1);
	QVERIFY(!stored->GetRoles(Product).contains(FirstRole));
	QCOMPARE(stored->GetDirectoryRoles(Product), QByteArrayList({FirstRole, SecondRole}));
}


I_ADD_TEST(CLdapGroupMappingTest);
