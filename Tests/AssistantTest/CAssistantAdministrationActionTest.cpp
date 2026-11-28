// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CAssistantAdministrationActionTest.h"


// Qt includes
#include <QtCore/QJsonArray>

// ACF includes
#include <icomp/TSimComponentWrap.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequest.h>
#include <imtassistgql/CAdministrationAddUserToGroupActionComp.h>
#include <imtassistgql/CAdministrationSearchActionComp.h>


using namespace imtassist;
using namespace imtassistgql;


namespace
{


const QByteArray s_userId = "alice";
const QByteArray s_tenantId = "tenant-a";
const QByteArray s_productId = "product-1";


/**
	GraphQL handler of the test: answers queued payloads and records the requests.
*/
class CTestHandlerComp: public icomp::CComponentBase, virtual public imtgql::IGqlRequestHandler
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestHandlerComp);
		I_REGISTER_INTERFACE(imtgql::IGqlRequestHandler);
	I_END_COMPONENT;

	struct Call
	{
		QByteArray commandId;
		QByteArray userId;
		QByteArray tenantId;
		QJsonObject input;
	};

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override
	{
		return payloads.contains(gqlRequest.GetCommandId());
	}

	virtual QJsonObject CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override
	{
		Call call;
		call.commandId = gqlRequest.GetCommandId();
		const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
		if (contextPtr != nullptr){
			call.userId = contextPtr->GetUserId();
			call.tenantId = contextPtr->GetTenantId();
		}

		const imtgql::CGqlParamObject* inputPtr = gqlRequest.GetParamObject("input");
		if (inputPtr != nullptr){
			call.input = ToJson(*inputPtr);
		}

		calls.append(call);

		if (refusedCommands.contains(call.commandId)){
			errorMessage = QStringLiteral("Invalid permissions");

			return QJsonObject();
		}

		QJsonObject retVal;
		retVal["data"] = payloads.value(call.commandId);

		return retVal;
	}

	static QJsonObject ToJson(const imtgql::CGqlParamObject& object)
	{
		QJsonObject retVal;
		for (const QByteArray& paramId: object.GetParamIds()){
			const QString key = QString::fromUtf8(paramId);
			if (IsList(paramId)){
				QJsonArray array;
				for (const imtgql::CGqlParamObject* objectPtr: object.GetParamArgumentObjectPtrList(paramId)){
					array.append(ToJson(*objectPtr));
				}

				retVal[key] = array;
			}
			else if (object.GetParamArgumentObjectPtr(paramId) != nullptr){
				retVal[key] = ToJson(*object.GetParamArgumentObjectPtr(paramId));
			}
			else{
				retVal[key] = QJsonValue::fromVariant(object.GetParamArgumentValue(paramId));
			}
		}

		return retVal;
	}

	static bool IsList(const QByteArray& paramId)
	{
		return (paramId == "users") || (paramId == "roles") || (paramId == "parentGroups");
	}

	QMap<QByteArray, QJsonObject> payloads;
	QSet<QByteArray> refusedCommands;
	mutable QList<Call> calls;
};


typedef icomp::TSimSharedComponentPtr<CTestHandlerComp> TestHandler;
typedef icomp::TSimComponentWrap<CAdministrationSearchActionComp> SearchAction;
typedef icomp::TSimComponentWrap<CAdministrationAddUserToGroupActionComp> AddUserToGroupAction;


CAssistantRequestContext CreateContext(const QByteArrayList& permissions, const QByteArray& userId = s_userId, const QByteArray& tenantId = s_tenantId)
{
	imtauth::CUserInfo user;
	user.SetId(userId);
	user.SetName(QStringLiteral("Alice"));
	user.SetLocalPermissions(s_productId, permissions);

	istd::TUniqueInterfacePtr<imtgql::CGqlContext> gqlContextPtr(new imtgql::CGqlContext);
	gqlContextPtr->SetUserId(userId);
	gqlContextPtr->SetTenantId(tenantId);
	gqlContextPtr->SetProductId(s_productId);
	gqlContextPtr->SetTenantPermissions(permissions);
	gqlContextPtr->SetUserInfo(&user);

	CAssistantRequestContext retVal(s_userId, s_tenantId);
	retVal.SetTransportContext(istd::IChangeableSharedPtr(std::move(gqlContextPtr)));

	return retVal;
}


QJsonObject CreateItem(const QString& id, const QString& name, const QString& userId)
{
	QJsonObject retVal;
	retVal["id"] = id;
	retVal["name"] = name;
	retVal["userId"] = userId;
	retVal["mail"] = userId + QStringLiteral("@example.com");

	return retVal;
}


QJsonObject CreateUsersPayload()
{
	QJsonArray items;
	items.append(CreateItem("u1", "Anna Berg", "aberg"));
	items.append(CreateItem("u2", "Bernd Clausen", "bclausen"));
	items.append(CreateItem("u3", "Anja Dorn", "adorn"));

	QJsonObject retVal;
	retVal["items"] = items;

	return retVal;
}


QJsonObject CreateAssignment(const QString& id, bool isDirect)
{
	QJsonObject retVal;
	retVal["id"] = id;
	retVal["direct"] = isDirect;

	return retVal;
}


QJsonObject CreateGroupPayload(const QJsonArray& users)
{
	QJsonObject retVal;
	retVal["id"] = QStringLiteral("g1");
	retVal["name"] = QStringLiteral("Support");
	retVal["description"] = QStringLiteral("Support team");
	retVal["roles"] = QJsonArray({CreateAssignment("r1", true), CreateAssignment("r2", false)});
	retVal["users"] = users;
	retVal["parentGroups"] = QJsonArray({CreateAssignment("g0", true)});

	return retVal;
}


QJsonObject CreateUserPayload()
{
	QJsonObject retVal;
	retVal["id"] = QStringLiteral("u2");
	retVal["name"] = QStringLiteral("Bernd Clausen");

	return retVal;
}


const QByteArrayList s_viewPermissions = {"/Administration/Users/View", "/Administration/Groups/View", "/Administration/Roles/View"};
const QByteArrayList s_editPermissions = {"/Administration/Users/View", "/Administration/Groups/View", "/Administration/Groups/Edit/Change"};


struct AddFixture
{
	AddFixture()
	{
		handler->InitComponent();
		handler->payloads["GroupItem"] = CreateGroupPayload(QJsonArray({CreateAssignment("u1", true), CreateAssignment("u9", false)}));
		handler->payloads["UserItem"] = CreateUserPayload();
		handler->payloads["GroupUpdate"] = QJsonObject({{"id", "g1"}});

		action.InsertMultiRef("RequestHandlers", handler);
		action.InitComponent();

		parameters["userId"] = QStringLiteral("u2");
		parameters["groupId"] = QStringLiteral("g1");
	}

	TestHandler handler;
	AddUserToGroupAction action;
	QJsonObject parameters;
};


} // anonymous namespace


void CAssistantAdministrationActionTest::testSearch_UsesContextOfUser()
{
	TestHandler handler;
	handler->InitComponent();
	handler->payloads["UsersList"] = CreateUsersPayload();

	SearchAction action;
	action.SetIdAttr("Kind", "users");
	action.InsertMultiRef("RequestHandlers", handler);
	action.InitComponent();

	QCOMPARE(action.GetActionId(), QByteArray("administration.searchUsers"));
	QCOMPARE(action.GetActionKind(), IAssistantAction::AK_READ);

	const CAssistantRequestContext context = CreateContext(s_viewPermissions);
	QVERIFY(action.CheckAccess(context, QJsonObject()));

	IAssistantAction::Result result;
	QString userError;
	QCOMPARE(action.Execute(context, QJsonObject(), result, userError), IAssistantAction::ES_SUCCEEDED);

	QCOMPARE(handler->calls.size(), 1);
	QCOMPARE(handler->calls[0].commandId, QByteArray("UsersList"));
	QCOMPARE(handler->calls[0].userId, s_userId);
	QCOMPARE(handler->calls[0].tenantId, s_tenantId);
	QCOMPARE(handler->calls[0].input.value("productId").toString(), QString::fromUtf8(s_productId));

	const QJsonArray items = result.data.value("items").toArray();
	QCOMPARE(items.size(), 3);
	QCOMPARE(items[0].toObject().value("description").toString(), QStringLiteral("aberg"));

	// No e-mail addresses are passed to the model.
	QVERIFY(!QJsonDocument(result.data).toJson().contains("example.com"));
	QVERIFY(!QJsonDocument(result.representation.data).toJson().contains("example.com"));
	QVERIFY(action.GetResultSchema().Validate(result.data, &userError));
}


void CAssistantAdministrationActionTest::testSearch_FiltersAndLimits()
{
	TestHandler handler;
	handler->InitComponent();
	handler->payloads["UsersList"] = CreateUsersPayload();

	SearchAction action;
	action.SetIdAttr("Kind", "users");
	action.InsertMultiRef("RequestHandlers", handler);
	action.InitComponent();

	const CAssistantRequestContext context = CreateContext(s_viewPermissions);

	QJsonObject parameters;
	parameters["text"] = QStringLiteral("an");
	IAssistantAction::Result result;
	QString userError;
	QCOMPARE(action.Execute(context, parameters, result, userError), IAssistantAction::ES_SUCCEEDED);
	QCOMPARE(result.data.value("items").toArray().size(), 2);
	QVERIFY(!result.data.value("hasMore").toBool());

	// The text is never part of the query, only of the filter in the action.
	parameters["text"] = QStringLiteral("\") { evil }");
	QCOMPARE(action.Execute(context, parameters, result, userError), IAssistantAction::ES_SUCCEEDED);
	QCOMPARE(result.data.value("items").toArray().size(), 0);

	parameters = QJsonObject();
	parameters["limit"] = 1;
	QCOMPARE(action.Execute(context, parameters, result, userError), IAssistantAction::ES_SUCCEEDED);
	QCOMPARE(result.data.value("items").toArray().size(), 1);
	QVERIFY(result.data.value("hasMore").toBool());
}


void CAssistantAdministrationActionTest::testSearch_RefusedWithoutContextOrPermission()
{
	TestHandler handler;
	handler->InitComponent();
	handler->payloads["UsersList"] = CreateUsersPayload();

	SearchAction action;
	action.SetIdAttr("Kind", "users");
	action.InsertMultiRef("RequestHandlers", handler);
	action.InitComponent();

	// Without the GraphQL context of the user, e.g. a demo principal
	const CAssistantRequestContext withoutContext(s_userId, s_tenantId);
	QVERIFY(!action.CheckAccess(withoutContext, QJsonObject()));

	IAssistantAction::Result result;
	QString userError;
	QCOMPARE(action.Execute(withoutContext, QJsonObject(), result, userError), IAssistantAction::ES_FAILED);
	QVERIFY(handler->calls.isEmpty());

	// Without permission
	QVERIFY(!action.CheckAccess(CreateContext({"/Administration/Groups/View"}), QJsonObject()));

	// A GraphQL context of another user or tenant is not used.
	QVERIFY(!action.CheckAccess(CreateContext(s_viewPermissions, "mallory"), QJsonObject()));
	QVERIFY(!action.CheckAccess(CreateContext(s_viewPermissions, s_userId, "tenant-b"), QJsonObject()));
	QCOMPARE(action.Execute(CreateContext(s_viewPermissions, s_userId, "tenant-b"), QJsonObject(), result, userError), IAssistantAction::ES_FAILED);
	QVERIFY(handler->calls.isEmpty());

	// The handler refuses: the action fails without details for the user.
	handler->refusedCommands.insert("UsersList");
	QCOMPARE(action.Execute(CreateContext(s_viewPermissions), QJsonObject(), result, userError), IAssistantAction::ES_FAILED);
	QVERIFY(!userError.contains(QStringLiteral("permissions")));
}


void CAssistantAdministrationActionTest::testSearch_KindsOfObjects()
{
	TestHandler handler;
	handler->InitComponent();

	QJsonObject role;
	role["id"] = QStringLiteral("r1");
	role["roleName"] = QStringLiteral("Administrator");
	role["roleDescription"] = QStringLiteral("All rights");
	handler->payloads["RolesList"] = QJsonObject({{"items", QJsonArray({role})}});

	QJsonObject group;
	group["id"] = QStringLiteral("g1");
	group["name"] = QStringLiteral("Support");
	handler->payloads["GroupsList"] = QJsonObject({{"items", QJsonArray({group})}});

	SearchAction roles;
	roles.SetIdAttr("Kind", "roles");
	roles.InsertMultiRef("RequestHandlers", handler);
	roles.InitComponent();
	QCOMPARE(roles.GetActionId(), QByteArray("administration.searchRoles"));

	SearchAction groups;
	groups.SetIdAttr("Kind", "groups");
	groups.InsertMultiRef("RequestHandlers", handler);
	groups.InitComponent();
	QCOMPARE(groups.GetActionId(), QByteArray("administration.searchGroups"));

	const CAssistantRequestContext context = CreateContext(s_viewPermissions);
	IAssistantAction::Result result;
	QString userError;
	QCOMPARE(roles.Execute(context, QJsonObject(), result, userError), IAssistantAction::ES_SUCCEEDED);
	QCOMPARE(result.data.value("items").toArray()[0].toObject().value("name").toString(), QStringLiteral("Administrator"));

	QCOMPARE(groups.Execute(context, QJsonObject(), result, userError), IAssistantAction::ES_SUCCEEDED);
	QCOMPARE(result.data.value("items").toArray()[0].toObject().value("name").toString(), QStringLiteral("Support"));
}


void CAssistantAdministrationActionTest::testAddUserToGroup_PreviewAndApply()
{
	AddFixture fixture;

	QCOMPARE(fixture.action.GetActionKind(), IAssistantAction::AK_CHANGE);

	const CAssistantRequestContext context = CreateContext(s_editPermissions);
	QVERIFY(fixture.action.CheckAccess(context, fixture.parameters));
	QVERIFY(!fixture.action.CheckAccess(CreateContext(s_viewPermissions), fixture.parameters));
	QVERIFY(fixture.action.GetParameterSchema().Validate(fixture.parameters));

	IAssistantAction::ChangePreview preview;
	QString userError;
	QCOMPARE(fixture.action.PrepareChange(context, fixture.parameters, preview, userError), IAssistantAction::ES_SUCCEEDED);
	QVERIFY(preview.summary.contains(QStringLiteral("Bernd Clausen")));
	QVERIFY(preview.summary.contains(QStringLiteral("Support")));
	QCOMPARE(preview.targetId, QByteArray("g1"));
	QVERIFY(!preview.stateToken.isEmpty());

	// Preparing changes nothing.
	for (const CTestHandlerComp::Call& call: fixture.handler->calls){
		QVERIFY(call.commandId != "GroupUpdate");
	}

	IAssistantAction::Result result;
	QCOMPARE(fixture.action.ApplyChange(context, fixture.parameters, preview.stateToken, result, userError), IAssistantAction::ES_SUCCEEDED);
	QVERIFY(fixture.action.GetResultSchema().Validate(result.data));

	const CTestHandlerComp::Call update = fixture.handler->calls.last();
	QCOMPARE(update.commandId, QByteArray("GroupUpdate"));
	QCOMPARE(update.userId, s_userId);
	QCOMPARE(update.input.value("id").toString(), QStringLiteral("g1"));
	QCOMPARE(update.input.value("productId").toString(), QString::fromUtf8(s_productId));

	const QJsonObject item = update.input.value("item").toObject();
	QCOMPARE(item.value("name").toString(), QStringLiteral("Support"));

	// The direct assignments are kept, the user is added; inherited assignments are not written.
	QStringList userIds;
	for (const QJsonValue& value: item.value("users").toArray()){
		userIds.append(value.toObject().value("id").toString());
		QVERIFY(value.toObject().value("direct").toBool());
	}

	QCOMPARE(userIds, QStringList({"u1", "u2"}));
	QCOMPARE(item.value("roles").toArray().size(), 1);
	QCOMPARE(item.value("roles").toArray()[0].toObject().value("id").toString(), QStringLiteral("r1"));
	QCOMPARE(item.value("parentGroups").toArray().size(), 1);
}


void CAssistantAdministrationActionTest::testAddUserToGroup_AlreadyMember()
{
	AddFixture fixture;
	fixture.parameters["userId"] = QStringLiteral("u1");

	IAssistantAction::ChangePreview preview;
	QString userError;
	QCOMPARE(fixture.action.PrepareChange(CreateContext(s_editPermissions), fixture.parameters, preview, userError), IAssistantAction::ES_FAILED);
	QVERIFY(!userError.isEmpty());
}


void CAssistantAdministrationActionTest::testAddUserToGroup_StaleGroup()
{
	AddFixture fixture;
	const CAssistantRequestContext context = CreateContext(s_editPermissions);

	IAssistantAction::ChangePreview preview;
	QString userError;
	QCOMPARE(fixture.action.PrepareChange(context, fixture.parameters, preview, userError), IAssistantAction::ES_SUCCEEDED);

	// Somebody else changed the members in the meantime.
	fixture.handler->payloads["GroupItem"] = CreateGroupPayload(QJsonArray({CreateAssignment("u1", true), CreateAssignment("u7", true)}));

	IAssistantAction::Result result;
	QCOMPARE(fixture.action.ApplyChange(context, fixture.parameters, preview.stateToken, result, userError), IAssistantAction::ES_STALE);
	QVERIFY(fixture.handler->calls.last().commandId != "GroupUpdate");
}


void CAssistantAdministrationActionTest::testAddUserToGroup_HandlerRefuses()
{
	AddFixture fixture;
	const CAssistantRequestContext context = CreateContext(s_editPermissions);

	IAssistantAction::ChangePreview preview;
	QString userError;
	QCOMPARE(fixture.action.PrepareChange(context, fixture.parameters, preview, userError), IAssistantAction::ES_SUCCEEDED);

	fixture.handler->refusedCommands.insert("GroupUpdate");

	IAssistantAction::Result result;
	QCOMPARE(fixture.action.ApplyChange(context, fixture.parameters, preview.stateToken, result, userError), IAssistantAction::ES_FAILED);
	QVERIFY(!userError.isEmpty());
}


I_ADD_TEST(CAssistantAdministrationActionTest);


