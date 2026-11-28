// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CAssistantAgentTest.h"


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QUuid>

// ACF includes
#include <icomp/TSimComponentWrap.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtdb/IDocumentActionJournal.h>
#include <imtdb/ISqlDatabaseObjectDelegate.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequest.h>
#include <imtassist/CAssistantKnowledgeActionComp.h>
#include <imtassist/CMarkdownKnowledgeBaseComp.h>
#include <imtassistgql/CAssistantApiActionComp.h>
#include <imtassistgql/CAssistantApiCatalogComp.h>
#include <imtassistgql/CAssistantApiPlanActionComp.h>


using namespace imtassist;
using namespace imtassistgql;


namespace
{


const QByteArray s_userId = "alice";
const QByteArray s_tenantId = "tenant-a";
const QByteArray s_productId = "product-1";


QString GetRepositoryPath(const QString& relativePath)
{
	// The tests read the schemas and documents of the repository.
	return QFileInfo(QStringLiteral(__FILE__)).dir().absoluteFilePath(QStringLiteral("../../") + relativePath);
}


/**
	GraphQL handler of the test: answers queued payloads per command and records the requests.
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
		QJsonObject variables;
		QByteArrayList fields;
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
			call.variables["id"] = inputPtr->GetParamArgumentValue("id").toString();
			call.variables["name"] = inputPtr->GetParamArgumentValue("name").toString();
		}

		call.fields = gqlRequest.GetFields().GetFieldIds();

		calls.append(call);

		if (refusedCommands.contains(call.commandId)){
			errorMessage = QStringLiteral("Invalid permissions");

			return QJsonObject();
		}

		QJsonObject retVal;
		retVal["data"] = payloads.value(call.commandId);

		return retVal;
	}

	QMap<QByteArray, QJsonObject> payloads;
	QSet<QByteArray> refusedCommands;
	mutable QList<Call> calls;
};


/**
	Document action journal of the test: the current state of the documents, no history.
*/
class CTestJournalComp: public icomp::CComponentBase, virtual public imtdb::IDocumentActionJournal
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestJournalComp);
		I_REGISTER_INTERFACE(imtdb::IDocumentActionJournal);
	I_END_COMPONENT;

	// reimplemented (imtdb::IDocumentActionJournal)
	virtual QByteArray BeginDocumentAction(
				const QByteArray& /*tenantId*/,
				const QByteArray& /*ownerId*/,
				const QByteArray& /*origin*/,
				const QString& /*summary*/,
				const QByteArray& /*referenceId*/ = QByteArray()) override
	{
		return QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	virtual bool SetDocumentActionState(const QByteArray& /*tenantId*/, const QByteArray& /*documentActionId*/, DocumentActionState /*state*/) override { return true; }
	virtual bool GetDocumentActionInfo(const QByteArray& /*tenantId*/, const QByteArray& /*documentActionId*/, DocumentActionInfo& /*info*/) const override { return false; }
	virtual QList<DocumentActionInfo> GetDocumentActions(const QByteArray& /*tenantId*/, const QByteArray& /*ownerId*/, int /*maxCount*/) const override { return QList<DocumentActionInfo>(); }
	virtual bool GetEntries(const QByteArray& /*tenantId*/, const QByteArray& /*documentActionId*/, QList<Entry>& /*result*/) const override { return false; }

	virtual bool GetDocumentState(
				const QByteArray& /*tableSchema*/,
				const QByteArray& /*tableName*/,
				const QByteArray& documentId,
				DocumentState& state) const override
	{
		state = documentStates.value(documentId);

		return true;
	}

	QMap<QByteArray, DocumentState> documentStates;
};


/**
	Document delegate of the test: only the table of the groups.
*/
class CTestDelegateComp: public icomp::CComponentBase, virtual public imtdb::ISqlDatabaseObjectDelegate
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestDelegateComp);
		I_REGISTER_INTERFACE(imtdb::ISqlDatabaseObjectDelegate);
	I_END_COMPONENT;

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual QByteArray GetTableName() const override { return QByteArrayLiteral("UserGroups"); }
	virtual QByteArray GetTableScheme() const override { return QByteArray(); }
	virtual QByteArray GetObjectIdFromRecord(const QSqlRecord& /*record*/) const override { return QByteArray(); }
	virtual QByteArray GetObjectTypeIdFromRecord(const QSqlRecord& /*record*/) const override { return QByteArray(); }
	virtual bool CreateObjectInfoFromRecord(const QSqlRecord& /*record*/, idoc::MetaInfoPtr& /*objectMetaInfoPtr*/, idoc::MetaInfoPtr& /*collectionItemMetaInfoPtr*/) const override { return false; }
	virtual istd::IChangeableUniquePtr CreateObjectFromRecord(const QSqlRecord& /*record*/, const iprm::IParamsSet* /*paramsPtr*/ = nullptr) const override { return nullptr; }
	virtual QVariant GetElementInfoFromRecord(const QSqlRecord& /*record*/, const QByteArray& /*infoId*/) const override { return QVariant(); }
	virtual QByteArray CreateUpdateMetaInfoQuery(const QSqlRecord& /*record*/) const override { return QByteArray(); }

	// reimplemented (imtdb::IDatabaseObjectDelegate)
	virtual const iprm::IOptionsList* GetObjectTypeInfos() const override { return nullptr; }
	virtual QByteArray GetObjectTypeId(const QByteArray& /*objectId*/) const override { return QByteArray(); }
	virtual QByteArray GetCountQuery(const iprm::IParamsSet* /*paramsPtr*/ = nullptr) const override { return QByteArray(); }
	virtual QByteArray GetSelectionQuery(const QByteArray& /*objectId*/ = QByteArray(), int /*offset*/ = 0, int /*count*/ = -1, const iprm::IParamsSet* /*paramsPtr*/ = nullptr) const override { return QByteArray(); }
	virtual NewObjectQuery CreateNewObjectQuery(const QByteArray& /*typeId*/, const QByteArray& /*proposedObjectId*/, const QString& /*objectName*/, const QString& /*objectDescription*/, const istd::IChangeable* /*valuePtr*/, const imtbase::IOperationContext* /*operationContextPtr*/) const override { return NewObjectQuery(); }
	virtual QByteArray CreateDeleteObjectsQuery(const imtbase::IObjectCollection& /*collection*/, const imtbase::ICollectionInfo::Ids& /*objectIds*/, const imtbase::IOperationContext* /*operationContextPtr*/) const override { return QByteArray(); }
	virtual QByteArray CreateDeleteObjectSetQuery(const imtbase::IObjectCollection& /*collection*/, const iprm::IParamsSet* /*paramsPtr*/ = nullptr, const imtbase::IOperationContext* /*operationContextPtr*/ = nullptr) const override { return QByteArray(); }
	virtual QByteArray CreateRestoreObjectsQuery(const imtbase::IObjectCollection& /*collection*/, const imtbase::ICollectionInfo::Ids& /*objectIds*/, const imtbase::IOperationContext* /*operationContextPtr*/ = nullptr) const override { return QByteArray(); }
	virtual QByteArray CreateRestoreObjectSetQuery(const imtbase::IObjectCollection& /*collection*/, const iprm::IParamsSet* /*paramsPtr*/ = nullptr, const imtbase::IOperationContext* /*operationContextPtr*/ = nullptr) const override { return QByteArray(); }
	virtual QByteArray CreateUpdateObjectQuery(const imtbase::IObjectCollection& /*collection*/, const QByteArray& /*objectId*/, const istd::IChangeable& /*object*/, const imtbase::IOperationContext* /*operationContextPtr*/, bool /*useExternDelegate*/ = true) const override { return QByteArray(); }
	virtual NewObjectQuery CreateUpdateObjectQueryWithParameters(const imtbase::IObjectCollection& /*collection*/, const QByteArray& /*objectId*/, const istd::IChangeable& /*object*/, const imtbase::IOperationContext* /*operationContextPtr*/, bool /*useExternDelegate*/ = true) const override { return NewObjectQuery(); }
	virtual QByteArray CreateRenameObjectQuery(const imtbase::IObjectCollection& /*collection*/, const QByteArray& /*objectId*/, const QString& /*newObjectName*/, const imtbase::IOperationContext* /*operationContextPtr*/) const override { return QByteArray(); }
	virtual QByteArray CreateDescriptionObjectQuery(const imtbase::IObjectCollection& /*collection*/, const QByteArray& /*objectId*/, const QString& /*description*/, const imtbase::IOperationContext* /*operationContextPtr*/) const override { return QByteArray(); }
	virtual QByteArray CreateResetQuery(const imtbase::IObjectCollection& /*collection*/) const override { return QByteArray(); }
	virtual QByteArray CreateDataMetaInfoQuery(const imtbase::IObjectCollection& /*collection*/, const QByteArray& /*objectId*/, const idoc::IDocumentMetaInfo* /*dataMetaInfoPtr*/) const override { return QByteArray(); }
	virtual QByteArray CreateCollectionItemMetaInfoQuery(const imtbase::IObjectCollection& /*collection*/, const QByteArray& /*objectId*/, const idoc::IDocumentMetaInfo* /*collectionItemMetaInfoPtr*/) const override { return QByteArray(); }
	virtual QByteArray GetSelectionByMetaInfoQuery(const QByteArray& /*metaInfoId*/, const QVariant& /*metaInfoValue*/) const override { return QByteArray(); }
};


typedef icomp::TSimSharedComponentPtr<CTestHandlerComp> TestHandler;
typedef icomp::TSimSharedComponentPtr<CTestJournalComp> TestJournal;
typedef icomp::TSimSharedComponentPtr<CTestDelegateComp> TestDelegate;
typedef icomp::TSimSharedComponentPtr<CAssistantApiCatalogComp> Catalog;
typedef icomp::TSimSharedComponentPtr<CMarkdownKnowledgeBaseComp> KnowledgeBase;
typedef icomp::TSimComponentWrap<CAssistantApiActionComp> ApiAction;
typedef icomp::TSimComponentWrap<CAssistantApiPlanActionComp> PlanAction;
typedef icomp::TSimComponentWrap<CAssistantKnowledgeActionComp> KnowledgeAction;


CAssistantRequestContext CreateContext(const QByteArray& userId = s_userId, const QByteArray& tenantId = s_tenantId)
{
	imtauth::CUserInfo user;
	user.SetId(userId);
	user.SetName(QStringLiteral("Alice"));

	istd::TUniqueInterfacePtr<imtgql::CGqlContext> gqlContextPtr(new imtgql::CGqlContext);
	gqlContextPtr->SetUserId(userId);
	gqlContextPtr->SetTenantId(tenantId);
	gqlContextPtr->SetProductId(s_productId);
	gqlContextPtr->SetUserInfo(&user);

	CAssistantRequestContext retVal(s_userId, s_tenantId);
	retVal.SetTransportContext(istd::IChangeableSharedPtr(std::move(gqlContextPtr)));

	return retVal;
}


void InitCatalog(Catalog& catalog)
{
	catalog->InsertMultiAttr("SchemaPaths", GetRepositoryPath("Sdl/imtauth/1.0"));
	catalog->InsertMultiAttr("SchemaPaths", GetRepositoryPath("Sdl/imtbase/1.0"));
	for (const char* query: {"UsersList", "UserItem", "GroupsList", "GroupItem", "RolesList", "SendEmailCode"}){
		catalog->InsertMultiAttr("AllowedQueries", QByteArray(query));
	}

	for (const char* mutation: {"GroupAdd", "GroupUpdate", "ChangePassword"}){
		catalog->InsertMultiAttr("AllowedMutations", QByteArray(mutation));
	}

	catalog->InsertMultiAttr("ForbiddenCommands", QByteArray("SendEmailCode"));
	catalog->InitComponent();
}


struct ApiFixture
{
	ApiFixture()
	{
		InitCatalog(catalog);
		handler->InitComponent();
		journal->InitComponent();
		groupDelegate->InitComponent();

		InitAction(list, "list");
		InitAction(describe, "describe");
		InitAction(query, "query");

		plan.SetRef("ApiCatalog", catalog);
		plan.InsertMultiRef("RequestHandlers", handler);
		plan.SetRef("DocumentActionJournal", journal);
		plan.InsertMultiRef("Delegates", groupDelegate);
		plan.InitComponent();
	}

	void InitAction(ApiAction& action, const QByteArray& kind)
	{
		action.SetRef("ApiCatalog", catalog);
		action.SetIdAttr("Kind", kind);
		action.InsertMultiRef("RequestHandlers", handler);
		action.InitComponent();
	}

	Catalog catalog;
	TestHandler handler;
	TestJournal journal;
	TestDelegate groupDelegate;
	ApiAction list;
	ApiAction describe;
	ApiAction query;
	PlanAction plan;
};


QJsonObject CreateStep(const QString& command, const QJsonObject& input, const QString& description)
{
	QJsonObject retVal;
	retVal["command"] = command;
	retVal["inputJson"] = QString::fromUtf8(QJsonDocument(input).toJson(QJsonDocument::Compact));
	retVal["description"] = description;

	return retVal;
}


QJsonObject CreateGroupInput(const QString& id, const QString& name)
{
	QJsonObject item;
	item["id"] = id;
	item["productId"] = QString::fromUtf8(s_productId);
	item["name"] = name;

	QJsonObject retVal;
	retVal["id"] = id;
	retVal["typeId"] = QStringLiteral("Group");
	retVal["productId"] = QString::fromUtf8(s_productId);
	retVal["name"] = name;
	retVal["item"] = item;

	return retVal;
}


} // anonymous namespace


void CAssistantAgentTest::testKnowledge_SectionsAndSearch()
{
	CMarkdownKnowledgeBase knowledge;
	const QString markdown = QStringLiteral(
				"Introduction text.\n\n"
				"# Groups\n\nA group has members and roles.\n#hashtag is no heading\n\n"
				"## Adding members\n\nAdd a user to a group with GroupUpdate.\n\n"
				"```\n# not a heading\n```\n\n"
				"# Роли\n\nРоль содержит права.\n");
	QCOMPARE(knowledge.AddDocument(QStringLiteral("Admin.md"), markdown), 4);

	IAssistantKnowledgeBase::Hits hits = knowledge.Search(QStringLiteral("add members group"), 5);
	QVERIFY(!hits.isEmpty());
	QCOMPARE(hits.first().title, QStringLiteral("Groups > Adding members"));
	QVERIFY(hits.first().snippet.contains(QStringLiteral("GroupUpdate")));

	// Any language, case-insensitive, word prefixes
	hits = knowledge.Search(QStringLiteral("РОЛЬ"), 5);
	QCOMPARE(hits.size(), 1);
	QCOMPARE(hits.first().title, QStringLiteral("Роли"));

	QString title;
	QString source;
	QString text;
	QVERIFY(knowledge.ReadSection(hits.first().sectionId, title, source, text));
	QCOMPARE(source, QStringLiteral("Admin.md"));
	QVERIFY(text.contains(QStringLiteral("права")));
	QVERIFY(!knowledge.ReadSection(QStringLiteral("Admin.md#99"), title, source, text));

	QVERIFY(knowledge.Search(QStringLiteral("a"), 5).isEmpty());

	// A long section is split into parts of limited size.
	CMarkdownKnowledgeBase longKnowledge;
	QString longText = QStringLiteral("# Long\n\n");
	for (int i = 0; i < 400; ++i){
		longText += QStringLiteral("Paragraph number %1 with some text about permissions.\n\n").arg(i);
	}

	QVERIFY(longKnowledge.AddDocument(QStringLiteral("Long.md"), longText) > 1);
	for (const IAssistantKnowledgeBase::Hit& hit: longKnowledge.Search(QStringLiteral("permissions"), 20)){
		QVERIFY(longKnowledge.ReadSection(hit.sectionId, title, source, text));
		QVERIFY(text.size() <= CMarkdownKnowledgeBase::MAX_SECTION_LENGTH);
	}
}


void CAssistantAgentTest::testKnowledge_Actions()
{
	KnowledgeBase knowledge;
	knowledge->InsertMultiAttr("DocumentPaths", GetRepositoryPath("Docs/Assistant/Knowledge"));
	knowledge->InitComponent();
	QVERIFY(knowledge->GetSectionsCount() > 3);

	KnowledgeAction search;
	search.SetRef("KnowledgeBase", knowledge);
	search.SetIdAttr("Kind", "search");
	search.InitComponent();

	KnowledgeAction read;
	read.SetRef("KnowledgeBase", knowledge);
	read.SetIdAttr("Kind", "read");
	read.InitComponent();

	QCOMPARE(search.GetActionId(), QByteArray("knowledge.search"));
	QCOMPARE(read.GetActionId(), QByteArray("knowledge.read"));

	const CAssistantRequestContext context(s_userId, s_tenantId);
	QVERIFY(search.CheckAccess(context, QJsonObject()));
	QVERIFY(!search.CheckAccess(CAssistantRequestContext(), QJsonObject()));

	QJsonObject parameters;
	parameters["text"] = QStringLiteral("update replaces whole object");
	QVERIFY(search.GetParameterSchema().Validate(parameters));

	IAssistantAction::Result result;
	QString userError;
	QCOMPARE(search.Execute(context, parameters, result, userError), IAssistantAction::ES_SUCCEEDED);
	QVERIFY(search.GetResultSchema().Validate(result.data, &userError));

	const QJsonArray hits = result.data.value("hits").toArray();
	QVERIFY(!hits.isEmpty());

	QJsonObject readParameters;
	readParameters["sectionId"] = hits.first().toObject().value("sectionId").toString();
	IAssistantAction::Result readResult;
	QCOMPARE(read.Execute(context, readParameters, readResult, userError), IAssistantAction::ES_SUCCEEDED);
	QVERIFY(read.GetResultSchema().Validate(readResult.data, &userError));
	QVERIFY(readResult.data.value("text").toString().contains(QStringLiteral("GroupUpdate")));

	readParameters["sectionId"] = QStringLiteral("missing#1");
	QCOMPARE(read.Execute(context, readParameters, readResult, userError), IAssistantAction::ES_FAILED);
}


void CAssistantAgentTest::testCatalog_ReadsImtCoreSchemas()
{
	CSdlApiCatalog catalog;
	int filesCount = 0;
	for (const QString& directory: {QStringLiteral("Sdl/imtauth/1.0"), QStringLiteral("Sdl/imtbase/1.0")}){
		const QDir dir(GetRepositoryPath(directory));
		for (const QString& fileName: dir.entryList(QStringList() << QStringLiteral("*.sdl"), QDir::Files)){
			QFile file(dir.absoluteFilePath(fileName));
			QVERIFY(file.open(QIODevice::ReadOnly));

			QString error;
			QVERIFY2(catalog.AddSchema(fileName, QString::fromUtf8(file.readAll()), &error), qPrintable(error));
			++filesCount;
		}
	}

	QVERIFY(filesCount > 20);

	const CSdlApiCatalog::Command* groupItemPtr = catalog.FindCommand(QStringLiteral("GroupItem"));
	QVERIFY(groupItemPtr != nullptr);
	QCOMPARE(groupItemPtr->kind, CSdlApiCatalog::CK_QUERY);
	QCOMPARE(groupItemPtr->field.type.name, QStringLiteral("GroupData"));

	const CSdlApiCatalog::Command* groupUpdatePtr = catalog.FindCommand(QStringLiteral("GroupUpdate"));
	QVERIFY(groupUpdatePtr != nullptr);
	QCOMPARE(groupUpdatePtr->kind, CSdlApiCatalog::CK_MUTATION);
	QCOMPARE(groupUpdatePtr->field.arguments.size(), 1);
	QCOMPARE(groupUpdatePtr->field.arguments.first().second.name, QStringLiteral("GroupDataInput"));

	const CSdlApiCatalog::TypeInfo* groupDataPtr = catalog.FindType(QStringLiteral("GroupData"));
	QVERIFY(groupDataPtr != nullptr);
	const CSdlApiCatalog::Field* usersPtr = groupDataPtr->FindField(QStringLiteral("users"));
	QVERIFY(usersPtr != nullptr);
	QVERIFY(usersPtr->type.isList);
	QCOMPARE(usersPtr->type.name, QStringLiteral("Assignment"));

	// Comments become descriptions.
	QVERIFY(!groupDataPtr->FindField(QStringLiteral("systemInfos"))->description.isEmpty());

	QVERIFY(catalog.IsScalar(QStringLiteral("ID")));
	QVERIFY(!catalog.IsScalar(QStringLiteral("GroupData")));

	const QString description = catalog.DescribeCommand(QStringLiteral("GroupUpdate"));
	QVERIFY(description.startsWith(QStringLiteral("mutation GroupUpdate(input: GroupDataInput): UpdatedNotificationPayload!")));
	QVERIFY(description.contains(QStringLiteral("item: GroupData!")));
	QVERIFY(description.contains(QStringLiteral("direct: Bool")));

	QString error;
	QVERIFY(!catalog.AddSchema(QStringLiteral("broken.sdl"), QStringLiteral("type Broken { name String }"), &error));
	QVERIFY(error.contains(QStringLiteral("broken.sdl")));
}


void CAssistantAgentTest::testCatalog_Policy()
{
	Catalog catalog;
	InitCatalog(catalog);

	QVERIFY(catalog->IsCommandAllowed(QStringLiteral("GroupItem")));
	QVERIFY(catalog->IsCommandAllowed(QStringLiteral("GroupUpdate")));

	// Not listed: not allowed; queries need the list as well, because some have effects.
	QVERIFY(!catalog->IsCommandAllowed(QStringLiteral("RoleUpdate")));
	QVERIFY(!catalog->IsCommandAllowed(QStringLiteral("GetTenantList")));

	// Forbidden explicitly, or by the words of authentication, even when listed.
	QVERIFY(!catalog->IsCommandAllowed(QStringLiteral("SendEmailCode")));
	QVERIFY(!catalog->IsCommandAllowed(QStringLiteral("ChangePassword")));
	QVERIFY(!catalog->IsCommandAllowed(QStringLiteral("Unknown")));

	QVERIFY(catalog->IsFieldForbidden(QStringLiteral("password")));
	QVERIFY(catalog->IsFieldForbidden(QStringLiteral("accessToken")));
	QVERIFY(catalog->IsFieldForbidden(QStringLiteral("passwordHash")));
	QVERIFY(!catalog->IsFieldForbidden(QStringLiteral("name")));
}


void CAssistantAgentTest::testApi_ListAndDescribe()
{
	ApiFixture fixture;
	const CAssistantRequestContext context = CreateContext();

	QJsonObject parameters;
	parameters["text"] = QStringLiteral("group");
	IAssistantAction::Result result;
	QString userError;
	QVERIFY(fixture.list.CheckAccess(context, parameters));
	QCOMPARE(fixture.list.Execute(context, parameters, result, userError), IAssistantAction::ES_SUCCEEDED);
	QVERIFY(fixture.list.GetResultSchema().Validate(result.data, &userError));

	QStringList names;
	for (const QJsonValue& command: result.data.value("commands").toArray()){
		names.append(command.toObject().value("name").toString());
	}

	QVERIFY(names.contains(QStringLiteral("GroupsList")));
	QVERIFY(names.contains(QStringLiteral("GroupUpdate")));
	QVERIFY(!names.contains(QStringLiteral("UsersList")));
	QVERIFY(!names.contains(QStringLiteral("RoleUpdate")));

	parameters = QJsonObject();
	parameters["command"] = QStringLiteral("GroupUpdate");
	QCOMPARE(fixture.describe.Execute(context, parameters, result, userError), IAssistantAction::ES_SUCCEEDED);
	QVERIFY(result.data.value("description").toString().contains(QStringLiteral("GroupDataInput")));

	parameters["command"] = QStringLiteral("ChangePassword");
	QCOMPARE(fixture.describe.Execute(context, parameters, result, userError), IAssistantAction::ES_FAILED);
}


void CAssistantAgentTest::testApi_QueryWithContextOfUser()
{
	ApiFixture fixture;
	QJsonObject user;
	user["id"] = QStringLiteral("u1");
	user["name"] = QStringLiteral("Anna");
	user["password"] = QStringLiteral("must-not-leak");
	user["email"] = QStringLiteral("anna@example.com");
	fixture.handler->payloads["UserItem"] = user;

	const CAssistantRequestContext context = CreateContext();

	QJsonObject parameters;
	parameters["command"] = QStringLiteral("UserItem");
	parameters["inputJson"] = QStringLiteral("{\"id\": \"u1\", \"productId\": \"product-1\"}");
	parameters["fields"] = QJsonArray({QStringLiteral("id"), QStringLiteral("name"), QStringLiteral("groups")});
	QVERIFY(fixture.query.GetParameterSchema().Validate(parameters));
	QVERIFY(fixture.query.CheckAccess(context, parameters));

	IAssistantAction::Result result;
	QString userError;
	QCOMPARE(fixture.query.Execute(context, parameters, result, userError), IAssistantAction::ES_SUCCEEDED);
	QVERIFY(fixture.query.GetResultSchema().Validate(result.data, &userError));

	QCOMPARE(fixture.handler->calls.size(), 1);
	const CTestHandlerComp::Call call = fixture.handler->calls.first();
	QCOMPARE(call.commandId, QByteArray("UserItem"));
	QCOMPARE(call.userId, s_userId);
	QCOMPARE(call.tenantId, s_tenantId);
	QCOMPARE(call.variables.value("id").toString(), QStringLiteral("u1"));
	QVERIFY(call.fields.contains("name"));
	QVERIFY(call.fields.contains("groups"));
	QVERIFY(!call.fields.contains("password"));

	// A secret in the answer of the handler never reaches the model.
	const QString resultJson = result.data.value("resultJson").toString();
	QVERIFY(resultJson.contains(QStringLiteral("Anna")));
	QVERIFY(!resultJson.contains(QStringLiteral("must-not-leak")));

	// Without fields: the simple fields of the result, never secrets.
	parameters.remove("fields");
	QCOMPARE(fixture.query.Execute(context, parameters, result, userError), IAssistantAction::ES_SUCCEEDED);
	QVERIFY(fixture.handler->calls.last().fields.contains("name"));
	QVERIFY(!fixture.handler->calls.last().fields.contains("password"));
}


void CAssistantAgentTest::testApi_QueryRefusesAndFilters()
{
	ApiFixture fixture;
	fixture.handler->payloads["GroupItem"] = QJsonObject({{"id", "g1"}});
	const CAssistantRequestContext context = CreateContext();

	QJsonObject parameters;
	parameters["command"] = QStringLiteral("GroupItem");
	parameters["inputJson"] = QStringLiteral("{\"id\": \"g1\", \"productId\": \"product-1\"}");

	// Unknown field: the error lists the fields, so the model can correct the call.
	parameters["fields"] = QJsonArray({QStringLiteral("members")});
	IAssistantAction::Result result;
	QString userError;
	QCOMPARE(fixture.query.Execute(context, parameters, result, userError), IAssistantAction::ES_FAILED);
	QVERIFY(userError.contains(QStringLiteral("users")));
	QVERIFY(fixture.handler->calls.isEmpty());

	// Secret fields are not readable.
	parameters["fields"] = QJsonArray({QStringLiteral("users.password")});
	QCOMPARE(fixture.query.Execute(context, parameters, result, userError), IAssistantAction::ES_FAILED);

	// Invalid JSON
	parameters["fields"] = QJsonArray();
	parameters["inputJson"] = QStringLiteral("{id:");
	QCOMPARE(fixture.query.Execute(context, parameters, result, userError), IAssistantAction::ES_FAILED);

	// Mutations, forbidden and not allowed commands are not queries of the assistant.
	for (const QString& command: {QStringLiteral("GroupUpdate"), QStringLiteral("SendEmailCode"), QStringLiteral("GetTenantList")}){
		parameters["command"] = command;
		QVERIFY2(!fixture.query.CheckAccess(context, parameters), qPrintable(command));
	}

	// Without the context of the user, or with the context of another user
	parameters["command"] = QStringLiteral("GroupItem");
	QVERIFY(!fixture.query.CheckAccess(CAssistantRequestContext(s_userId, s_tenantId), parameters));
	QVERIFY(!fixture.query.CheckAccess(CreateContext("mallory"), parameters));

	// A refused query fails without the text of the handler.
	parameters["inputJson"] = QStringLiteral("{\"id\": \"g1\", \"productId\": \"product-1\"}");
	fixture.handler->refusedCommands.insert("GroupItem");
	QCOMPARE(fixture.query.Execute(context, parameters, result, userError), IAssistantAction::ES_FAILED);
	QVERIFY(!userError.contains(QStringLiteral("permissions")));

	// Long lists are shortened.
	QJsonArray items;
	for (int i = 0; i < 150; ++i){
		items.append(QJsonObject({{"id", QString::number(i)}, {"name", QStringLiteral("Group %1").arg(i)}}));
	}

	fixture.handler->payloads["GroupsList"] = QJsonObject({{"items", items}});
	parameters["command"] = QStringLiteral("GroupsList");
	parameters["inputJson"] = QStringLiteral("{}");
	QCOMPARE(fixture.query.Execute(context, parameters, result, userError), IAssistantAction::ES_SUCCEEDED);
	QCOMPARE(result.data.value("omittedItems").toInt(), 50);
}


void CAssistantAgentTest::testPlan_PreviewShowsEverySteps()
{
	ApiFixture fixture;
	const CAssistantRequestContext context = CreateContext();

	QJsonObject parameters;
	parameters["summary"] = QStringLiteral("Create the group Support and rename Sales");
	parameters["steps"] = QJsonArray({
				CreateStep(QStringLiteral("GroupAdd"), CreateGroupInput(QStringLiteral("new"), QStringLiteral("Support")), QStringLiteral("Create the group Support")),
				CreateStep(QStringLiteral("GroupUpdate"), CreateGroupInput(QStringLiteral("g2"), QStringLiteral("Sales EU")), QStringLiteral("Rename Sales"))});
	QVERIFY(fixture.plan.GetParameterSchema().Validate(parameters));
	QCOMPARE(fixture.plan.GetActionKind(), IAssistantAction::AK_CHANGE);
	QVERIFY(fixture.plan.CheckAccess(context, parameters));

	IAssistantAction::ChangePreview preview;
	QString userError;
	QCOMPARE(fixture.plan.PrepareChange(context, parameters, preview, userError), IAssistantAction::ES_SUCCEEDED);
	QCOMPARE(preview.summary, QStringLiteral("Create the group Support and rename Sales"));
	QCOMPARE(preview.changes.size(), 2);

	// Every step shows its command and the data as it will be sent, not only the words of the model.
	const QJsonObject second = preview.changes.at(1).toObject();
	QVERIFY(second.value("field").toString().contains(QStringLiteral("GroupUpdate")));
	QVERIFY(second.value("newValue").toString().contains(QStringLiteral("Sales EU")));
	QVERIFY(!preview.stateToken.isEmpty());

	// Preparing changes nothing.
	QVERIFY(fixture.handler->calls.isEmpty());

	// A changed plan does not match the confirmed preview.
	QJsonObject changed = parameters;
	changed["summary"] = QStringLiteral("Something else");
	IAssistantAction::Result result;
	QCOMPARE(fixture.plan.ApplyChange(context, changed, preview.stateToken, result, userError), IAssistantAction::ES_STALE);
	QVERIFY(fixture.handler->calls.isEmpty());
}


void CAssistantAgentTest::testPlan_AppliesInOrderWithReferences()
{
	ApiFixture fixture;
	fixture.handler->payloads["GroupAdd"] = QJsonObject({{"id", "g-created"}});
	fixture.handler->payloads["GroupUpdate"] = QJsonObject({{"id", "g-created"}});
	const CAssistantRequestContext context = CreateContext();

	QJsonObject parameters;
	parameters["summary"] = QStringLiteral("Create Support and rename it");
	parameters["steps"] = QJsonArray({
				CreateStep(QStringLiteral("GroupAdd"), CreateGroupInput(QStringLiteral("new"), QStringLiteral("Support")), QStringLiteral("Create the group")),
				CreateStep(QStringLiteral("GroupUpdate"), CreateGroupInput(QStringLiteral("$step1.id"), QStringLiteral("Customer Support")), QStringLiteral("Rename it"))});

	IAssistantAction::ChangePreview preview;
	QString userError;
	QCOMPARE(fixture.plan.PrepareChange(context, parameters, preview, userError), IAssistantAction::ES_SUCCEEDED);

	IAssistantAction::Result result;
	QCOMPARE(fixture.plan.ApplyChange(context, parameters, preview.stateToken, result, userError), IAssistantAction::ES_SUCCEEDED);
	QVERIFY(fixture.plan.GetResultSchema().Validate(result.data, &userError));
	QCOMPARE(result.data.value("appliedSteps").toInt(), 2);

	QCOMPARE(fixture.handler->calls.size(), 2);
	QCOMPARE(fixture.handler->calls.at(0).commandId, QByteArray("GroupAdd"));
	QCOMPARE(fixture.handler->calls.at(1).commandId, QByteArray("GroupUpdate"));
	QCOMPARE(fixture.handler->calls.at(1).userId, s_userId);

	// The reference was replaced by the ID created in step 1.
	QCOMPARE(fixture.handler->calls.at(1).variables.value("id").toString(), QStringLiteral("g-created"));
	QCOMPARE(fixture.handler->calls.at(1).variables.value("name").toString(), QStringLiteral("Customer Support"));

	// References to later or missing results are refused.
	QJsonValue resolved;
	QString error;
	QVERIFY(!CAssistantApiPlanActionComp::ResolveReferences(QJsonValue(QStringLiteral("$step2.id")), {QJsonObject({{"id", "x"}})}, resolved, error));
	QVERIFY(!CAssistantApiPlanActionComp::ResolveReferences(QJsonValue(QStringLiteral("$step1.missing")), {QJsonObject({{"id", "x"}})}, resolved, error));
	QVERIFY(CAssistantApiPlanActionComp::ResolveReferences(QJsonValue(QStringLiteral("$step1.items.1.id")), {QJsonObject({{"items", QJsonArray({QJsonObject({{"id", "a"}}), QJsonObject({{"id", "b"}})})}})}, resolved, error));
	QCOMPARE(resolved.toString(), QStringLiteral("b"));
}


void CAssistantAgentTest::testPlan_StopsAtFirstFailure()
{
	ApiFixture fixture;
	fixture.handler->payloads["GroupAdd"] = QJsonObject({{"id", "g-created"}});
	fixture.handler->payloads["GroupUpdate"] = QJsonObject({{"id", "g2"}});
	fixture.handler->refusedCommands.insert("GroupUpdate");
	const CAssistantRequestContext context = CreateContext();

	QJsonObject parameters;
	parameters["summary"] = QStringLiteral("Three changes");
	parameters["steps"] = QJsonArray({
				CreateStep(QStringLiteral("GroupAdd"), CreateGroupInput(QStringLiteral("new"), QStringLiteral("A")), QStringLiteral("Create A")),
				CreateStep(QStringLiteral("GroupUpdate"), CreateGroupInput(QStringLiteral("g2"), QStringLiteral("B")), QStringLiteral("Rename to B")),
				CreateStep(QStringLiteral("GroupAdd"), CreateGroupInput(QStringLiteral("new"), QStringLiteral("C")), QStringLiteral("Create C"))});

	IAssistantAction::ChangePreview preview;
	QString userError;
	QCOMPARE(fixture.plan.PrepareChange(context, parameters, preview, userError), IAssistantAction::ES_SUCCEEDED);

	IAssistantAction::Result result;
	QCOMPARE(fixture.plan.ApplyChange(context, parameters, preview.stateToken, result, userError), IAssistantAction::ES_FAILED);

	// Step 3 never ran; the user learns exactly what happened.
	QCOMPARE(fixture.handler->calls.size(), 2);
	QVERIFY(userError.contains(QStringLiteral("1 of 3")));
	QVERIFY(userError.contains(QStringLiteral("GroupUpdate")));
}


void CAssistantAgentTest::testPlan_RefusesForbiddenCommandsAndFields()
{
	ApiFixture fixture;
	const CAssistantRequestContext context = CreateContext();

	QJsonObject parameters;
	parameters["summary"] = QStringLiteral("Change a password");
	parameters["steps"] = QJsonArray({CreateStep(QStringLiteral("ChangePassword"), QJsonObject(), QStringLiteral("Change it"))});
	QVERIFY(!fixture.plan.CheckAccess(context, parameters));

	IAssistantAction::ChangePreview preview;
	QString userError;
	QCOMPARE(fixture.plan.PrepareChange(context, parameters, preview, userError), IAssistantAction::ES_FAILED);

	// A query is not a step of a plan.
	parameters["steps"] = QJsonArray({CreateStep(QStringLiteral("GroupItem"), QJsonObject({{"id", "g1"}}), QStringLiteral("Read"))});
	QVERIFY(!fixture.plan.CheckAccess(context, parameters));

	// A secret field in the input is refused.
	QJsonObject input = CreateGroupInput(QStringLiteral("g1"), QStringLiteral("Support"));
	input["password"] = QStringLiteral("x");
	parameters["steps"] = QJsonArray({CreateStep(QStringLiteral("GroupUpdate"), input, QStringLiteral("Update"))});
	QVERIFY(fixture.plan.CheckAccess(context, parameters));
	QCOMPARE(fixture.plan.PrepareChange(context, parameters, preview, userError), IAssistantAction::ES_FAILED);
	QVERIFY(userError.contains(QStringLiteral("password")));

	// A missing required argument is found before the confirmation.
	parameters["steps"] = QJsonArray({QJsonObject({{"command", "GroupUpdate"}, {"description", "Update"}})});
	QCOMPARE(fixture.plan.PrepareChange(context, parameters, preview, userError), IAssistantAction::ES_FAILED);

	// Without the context of the user nothing is planned.
	QVERIFY(!fixture.plan.CheckAccess(CAssistantRequestContext(s_userId, s_tenantId), parameters));
	QVERIFY(fixture.handler->calls.isEmpty());
}


I_ADD_TEST(CAssistantAgentTest);


