// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CAssistantDocumentActionTest.h"


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/TSingleFactory.h>
#include <icomp/TSimComponentWrap.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtbase/CObjectCollection.h>
#include <imtbase/IDocumentActionContext.h>
#include <imtdb/IDocumentActionJournal.h>
#include <imtbase/IRevisionController.h>
#include <imtbase/imtbase.h>
#include <imtdb/ISqlDatabaseObjectDelegate.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/imtgql.h>
#include <imtassistgql/CAdministrationAddUserToGroupActionComp.h>
#include <imtassistgql/CAssistantApiCatalogComp.h>
#include <imtassistgql/CAssistantApiPlanActionComp.h>
#include <imtassistgql/CAssistantDocumentActionComp.h>


using namespace imtassist;
using namespace imtassistgql;


namespace
{


const QByteArray s_userId = "alice";
const QByteArray s_otherUserId = "bob";
const QByteArray s_tenantId = "tenant-a";
const QByteArray s_productId = "product-1";
const QByteArray s_tableName = "UserGroups";
const QByteArray s_editPermission = "/Administration/Groups/Edit/Change";
const QByteArray s_removePermission = "/Administration/Groups/Edit/Remove";
const QByteArray s_rollbackAnyPermission = "/Administration/Assistant/RollbackAny";


/**
	GraphQL handler of the test: answers queued payloads and records the document action header of the requests.
*/
class CTestHandlerComp: public icomp::CComponentBase, virtual public imtgql::IGqlRequestHandler
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestHandlerComp);
		I_REGISTER_INTERFACE(imtgql::IGqlRequestHandler);
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override
	{
		return payloads.contains(gqlRequest.GetCommandId());
	}

	virtual QJsonObject CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override
	{
		const QByteArray commandId = gqlRequest.GetCommandId();
		const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
		documentActionHeaders.append(qMakePair(commandId, (contextPtr != nullptr) ? contextPtr->GetHeaders().value(imtbase::s_documentActionHeaderId) : QByteArray()));

		if (refusedCommands.contains(commandId)){
			errorMessage = QStringLiteral("Refused");

			return QJsonObject();
		}

		QJsonObject retVal;
		retVal["data"] = payloads.value(commandId);

		return retVal;
	}

	QMap<QByteArray, QJsonObject> payloads;
	QSet<QByteArray> refusedCommands;
	mutable QList<QPair<QByteArray, QByteArray>> documentActionHeaders;
};


/**
	Document action journal of the test, in memory.
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
				const QByteArray& tenantId,
				const QByteArray& ownerId,
				const QByteArray& origin,
				const QString& summary,
				const QByteArray& referenceId = QByteArray()) override
	{
		if (failBegin){
			return QByteArray();
		}

		DocumentActionInfo info;
		info.id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
		info.tenantId = tenantId;
		info.ownerId = ownerId;
		info.origin = origin;
		info.summary = summary;
		info.referenceId = referenceId;
		info.state = DAS_APPLYING;
		info.createdAt = QDateTime::currentDateTimeUtc();
		documentActions.insert(info.id, info);
		order.prepend(info.id);

		return info.id;
	}

	virtual bool SetDocumentActionState(const QByteArray& tenantId, const QByteArray& documentActionId, DocumentActionState state) override
	{
		if (!documentActions.contains(documentActionId) || (documentActions[documentActionId].tenantId != tenantId)){
			return false;
		}

		documentActions[documentActionId].state = state;

		return true;
	}

	virtual bool GetDocumentActionInfo(const QByteArray& tenantId, const QByteArray& documentActionId, DocumentActionInfo& info) const override
	{
		if (!documentActions.contains(documentActionId) || (documentActions[documentActionId].tenantId != tenantId)){
			return false;
		}

		info = documentActions[documentActionId];
		info.entryCount = entries.value(documentActionId).size();

		return true;
	}

	virtual QList<DocumentActionInfo> GetDocumentActions(const QByteArray& tenantId, const QByteArray& ownerId, int maxCount) const override
	{
		QList<DocumentActionInfo> retVal;
		for (const QByteArray& id: order){
			DocumentActionInfo info;
			if (GetDocumentActionInfo(tenantId, id, info) && (ownerId.isEmpty() || (info.ownerId == ownerId)) && (retVal.size() < maxCount)){
				retVal.append(info);
			}
		}

		return retVal;
	}

	virtual bool GetEntries(const QByteArray& tenantId, const QByteArray& documentActionId, QList<Entry>& result) const override
	{
		DocumentActionInfo info;
		if (!GetDocumentActionInfo(tenantId, documentActionId, info)){
			return false;
		}

		result = entries.value(documentActionId);

		return true;
	}

	virtual bool GetDocumentState(
				const QByteArray& /*tableSchema*/,
				const QByteArray& tableName,
				const QByteArray& documentId,
				DocumentState& state) const override
	{
		state = documentStates.value(tableName + '/' + documentId);

		return true;
	}

	QByteArray AddDocumentAction(const QByteArray& ownerId, DocumentActionState state, const QList<Entry>& documentActionEntries, const QString& summary = QStringLiteral("Change the group"))
	{
		const QByteArray id = BeginDocumentAction(s_tenantId, ownerId, "assistant", summary);
		documentActions[id].state = state;
		entries.insert(id, documentActionEntries);

		return id;
	}

	bool failBegin = false;
	QMap<QByteArray, DocumentActionInfo> documentActions;
	QByteArrayList order;
	QMap<QByteArray, QList<Entry>> entries;
	QMap<QByteArray, DocumentState> documentStates;
};


/**
	Document of the test collection.
*/
class CTestDocument: public istd::IChangeable
{
public:
	explicit CTestDocument(const QString& value = QString())
		:text(value)
	{
	}

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override
	{
		return SO_COPY | SO_CLONE;
	}

	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/ = CM_WITHOUT_REFS) override
	{
		const CTestDocument* documentPtr = dynamic_cast<const CTestDocument*>(&object);
		if (documentPtr == nullptr){
			return false;
		}

		istd::CChangeNotifier changeNotifier(this);
		text = documentPtr->text;

		return true;
	}

	virtual istd::TUniqueInterfacePtr<istd::IChangeable> CloneMe(CompatibilityMode /*mode*/ = CM_WITHOUT_REFS) const override
	{
		return istd::TUniqueInterfacePtr<istd::IChangeable>(new CTestDocument(text));
	}

	QString text;
};


/**
	Collection of the test: records the document action of every write.
*/
class CTestCollectionComp: public icomp::CComponentBase, public imtbase::CObjectCollection
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestCollectionComp);
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
	I_END_COMPONENT;

	CTestCollectionComp()
	{
		RegisterFactory<istd::TSingleFactory<istd::IChangeable, CTestDocument>>("Group");
	}

	virtual bool SetObjectData(
				const Id& objectId,
				const istd::IChangeable& object,
				CompatibilityMode mode = CM_WITHOUT_REFS,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override
	{
		Record("set", objectId, operationContextPtr);

		return imtbase::CObjectCollection::SetObjectData(objectId, object, mode, operationContextPtr);
	}

	virtual bool RemoveElements(const Ids& elementIds, const imtbase::IOperationContext* operationContextPtr = nullptr) override
	{
		for (const Id& id: elementIds){
			Record("remove", id, operationContextPtr);
		}

		return imtbase::CObjectCollection::RemoveElements(elementIds, operationContextPtr);
	}

	QString GetText(const QByteArray& id) const
	{
		DataPtr dataPtr;
		if (!GetObjectData(id, dataPtr)){
			return QString();
		}

		const CTestDocument* documentPtr = dynamic_cast<const CTestDocument*>(dataPtr.GetPtr());

		return (documentPtr != nullptr) ? documentPtr->text : QString();
	}

	struct Write
	{
		QByteArray kind;
		QByteArray documentId;
		QByteArray documentActionId;
		QByteArray tenantId;
		QByteArray ownerId;
	};

	QList<Write> writes;

private:
	void Record(const QByteArray& kind, const QByteArray& documentId, const imtbase::IOperationContext* operationContextPtr)
	{
		Write write;
		write.kind = kind;
		write.documentId = documentId;
		const imtbase::IDocumentActionContext* documentActionContextPtr = dynamic_cast<const imtbase::IDocumentActionContext*>(operationContextPtr);
		if (documentActionContextPtr != nullptr){
			write.documentActionId = documentActionContextPtr->GetDocumentActionId();
		}
		if (operationContextPtr != nullptr){
			write.tenantId = operationContextPtr->GetTenantId();
			write.ownerId = operationContextPtr->GetOperationOwnerId().id;
		}

		writes.append(write);
	}
};


/**
	Document delegate of the test: the table and the stored revisions of the documents.
*/
class CTestDelegateComp:
			public icomp::CComponentBase,
			virtual public imtdb::ISqlDatabaseObjectDelegate,
			virtual public imtbase::IRevisionController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestDelegateComp);
		I_REGISTER_INTERFACE(imtdb::ISqlDatabaseObjectDelegate);
		I_REGISTER_INTERFACE(imtbase::IRevisionController);
	I_END_COMPONENT;

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual QByteArray GetTableName() const override { return tableName; }
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

	// reimplemented (imtbase::IRevisionController)
	virtual RevisionInfoList GetRevisionInfoList(const imtbase::IObjectCollection& /*collection*/, const imtbase::ICollectionInfo::Id& /*objectId*/) const override { return RevisionInfoList(); }
	virtual int BackupRevision(const imtbase::IObjectCollection& /*collection*/, const imtbase::ICollectionInfo::Id& /*objectId*/, const QString& /*userComment*/ = QString()) const override { return -1; }
	virtual bool RestoreRevision(imtbase::IObjectCollection& /*collection*/, const imtbase::ICollectionInfo::Id& /*objectId*/, int /*revision*/) const override { return false; }
	virtual bool ExportRevision(const imtbase::IObjectCollection& /*collection*/, const imtbase::ICollectionInfo::Id& /*objectId*/, int /*revision*/, const QString& /*filePath*/) const override { return false; }
	virtual bool DeleteRevision(imtbase::IObjectCollection& /*collection*/, const imtbase::ICollectionInfo::Id& /*objectId*/, int /*revision*/) const override { return false; }

	virtual bool GetRevisionData(
				const imtbase::IObjectCollection& /*collection*/,
				const imtbase::ICollectionInfo::Id& objectId,
				int revision,
				istd::IChangeableSharedPtr& dataPtr,
				idoc::MetaInfoPtr& dataMetaInfoPtr) const override
	{
		dataPtr.Reset();
		dataMetaInfoPtr.Reset();

		const QString key = QStringLiteral("%1/%2").arg(QString::fromUtf8(objectId)).arg(revision);
		if (!revisions.contains(key)){
			return false;
		}

		dataPtr.SetPtr(new CTestDocument(revisions.value(key)));

		return true;
	}

	QByteArray tableName = s_tableName;
	QMap<QString, QString> revisions;
};


typedef icomp::TSimSharedComponentPtr<CTestHandlerComp> TestHandler;
typedef icomp::TSimSharedComponentPtr<CTestJournalComp> TestJournal;
typedef icomp::TSimSharedComponentPtr<CTestCollectionComp> TestCollection;
typedef icomp::TSimSharedComponentPtr<CTestDelegateComp> TestDelegate;
typedef icomp::TSimComponentWrap<CAdministrationAddUserToGroupActionComp> AddUserToGroupAction;
typedef icomp::TSimComponentWrap<CAssistantDocumentActionComp> DocumentAction;
typedef icomp::TSimSharedComponentPtr<CAssistantApiCatalogComp> Catalog;
typedef icomp::TSimComponentWrap<CAssistantApiPlanActionComp> PlanAction;


CAssistantRequestContext CreateContext(const QByteArrayList& permissions, const QByteArray& userId = s_userId)
{
	imtauth::CUserInfo user;
	user.SetId(userId);
	user.SetName(QStringLiteral("Alice"));
	user.SetLocalPermissions(s_productId, permissions);

	istd::TUniqueInterfacePtr<imtgql::CGqlContext> gqlContextPtr(new imtgql::CGqlContext);
	gqlContextPtr->SetUserId(userId);
	gqlContextPtr->SetTenantId(s_tenantId);
	gqlContextPtr->SetProductId(s_productId);
	gqlContextPtr->SetTenantPermissions(permissions);
	gqlContextPtr->SetUserInfo(&user);

	CAssistantRequestContext retVal(userId, s_tenantId);
	retVal.SetTransportContext(istd::IChangeableSharedPtr(std::move(gqlContextPtr)));

	return retVal;
}


imtdb::IDocumentActionJournal::Entry CreateEntry(const QByteArray& documentId, imtdb::IDocumentActionJournal::EntryOperation operation, qlonglong before, qlonglong after, const QByteArray& tableName = s_tableName)
{
	imtdb::IDocumentActionJournal::Entry entry;
	entry.tableName = tableName;
	entry.documentId = documentId;
	entry.operation = operation;
	entry.revisionBefore = before;
	entry.revisionAfter = after;

	return entry;
}


imtdb::IDocumentActionJournal::DocumentState CreateState(qlonglong revision, bool isRemoved = false)
{
	imtdb::IDocumentActionJournal::DocumentState state;
	state.exists = true;
	state.revision = revision;
	state.isRemoved = isRemoved;

	return state;
}


QJsonObject CreateGroupPayload()
{
	QJsonObject retVal;
	retVal["id"] = QStringLiteral("g1");
	retVal["name"] = QStringLiteral("Support");
	retVal["description"] = QStringLiteral("Support team");
	retVal["roles"] = QJsonArray();
	retVal["users"] = QJsonArray();
	retVal["parentGroups"] = QJsonArray();

	return retVal;
}


const QByteArrayList s_permissions = {"/Administration/Groups/View", s_editPermission, s_removePermission};


struct AddFixture
{
	AddFixture()
	{
		handler->InitComponent();
		journal->InitComponent();
		handler->payloads["GroupItem"] = CreateGroupPayload();
		handler->payloads["UserItem"] = QJsonObject({{"id", "u2"}, {"name", "Bernd Clausen"}});
		handler->payloads["GroupUpdate"] = QJsonObject({{"id", "g1"}});

		action.InsertMultiRef("RequestHandlers", handler);
		action.SetRef("DocumentActionJournal", journal);
		action.InitComponent();

		parameters["userId"] = QStringLiteral("u2");
		parameters["groupId"] = QStringLiteral("g1");
	}

	TestHandler handler;
	TestJournal journal;
	AddUserToGroupAction action;
	QJsonObject parameters;
};


struct RollbackFixture
{
	explicit RollbackFixture(const QByteArray& kind = "rollback")
	{
		handler->InitComponent();
		journal->InitComponent();
		collection->InitComponent();
		groupDelegate->InitComponent();

		collection->InsertNewObject("Group", QStringLiteral("Support"), QString(), &newDocument, "g1");
		groupDelegate->revisions.insert(QStringLiteral("g1/1"), QStringLiteral("old"));
		journal->documentStates.insert(s_tableName + "/g1", CreateState(2));

		action.SetIdAttr("Kind", kind);
		action.InsertMultiRef("RequestHandlers", handler);
		action.SetRef("DocumentActionJournal", journal);
		action.InsertMultiRef("Collections", collection);
		action.InsertMultiRef("Delegates", groupDelegate);
		action.InsertMultiAttr("EditPermissions", s_editPermission);
		action.InsertMultiAttr("RemovePermissions", s_removePermission);
		action.InitComponent();
	}

	QByteArray AddUpdate(const QByteArray& ownerId = s_userId)
	{
		const QByteArray id = journal->AddDocumentAction(ownerId, imtdb::IDocumentActionJournal::DAS_APPLIED, {CreateEntry("g1", imtdb::IDocumentActionJournal::EO_UPDATED, 1, 2)});

		return id;
	}

	CTestDocument newDocument = CTestDocument(QStringLiteral("new"));
	TestHandler handler;
	TestJournal journal;
	TestCollection collection;
	TestDelegate groupDelegate;
	DocumentAction action;
};


struct PlanFixture
{
	PlanFixture()
	{
		// The schemas of the repository
		const QDir repositoryDir = QFileInfo(QStringLiteral(__FILE__)).dir();
		catalog->InsertMultiAttr("SchemaPaths", repositoryDir.absoluteFilePath(QStringLiteral("../../Sdl/imtauth/1.0")));
		catalog->InsertMultiAttr("SchemaPaths", repositoryDir.absoluteFilePath(QStringLiteral("../../Sdl/imtbase/1.0")));
		catalog->InsertMultiAttr("AllowedMutations", QByteArray("GroupAdd"));
		catalog->InsertMultiAttr("AllowedMutations", QByteArray("GroupUpdate"));
		catalog->InitComponent();

		handler->InitComponent();
		journal->InitComponent();
		handler->payloads["GroupAdd"] = QJsonObject({{"id", "g-created"}});
		handler->payloads["GroupUpdate"] = QJsonObject({{"id", "g-created"}});

		plan.SetRef("ApiCatalog", catalog);
		plan.InsertMultiRef("RequestHandlers", handler);
		plan.SetRef("DocumentActionJournal", journal);
		plan.InitComponent();

		parameters["summary"] = QStringLiteral("Create Support and rename it");
		parameters["steps"] = QJsonArray({CreateStep("GroupAdd", "new", "Support"), CreateStep("GroupUpdate", "$step1.id", "Customer Support")});
	}

	static QJsonObject CreateStep(const QString& command, const QString& id, const QString& name)
	{
		QJsonObject item;
		item["id"] = id;
		item["productId"] = QString::fromUtf8(s_productId);
		item["name"] = name;

		QJsonObject input;
		input["id"] = id;
		input["typeId"] = QStringLiteral("Group");
		input["productId"] = QString::fromUtf8(s_productId);
		input["name"] = name;
		input["item"] = item;

		QJsonObject retVal;
		retVal["command"] = command;
		retVal["inputJson"] = QString::fromUtf8(QJsonDocument(input).toJson(QJsonDocument::Compact));
		retVal["description"] = name;

		return retVal;
	}

	Catalog catalog;
	TestHandler handler;
	TestJournal journal;
	PlanAction plan;
	QJsonObject parameters;
};


QJsonObject CreateRollbackParameters(const QByteArray& documentActionId)
{
	QJsonObject retVal;
	retVal["documentActionId"] = QString::fromUtf8(documentActionId);

	return retVal;
}


} // anonymous namespace


void CAssistantDocumentActionTest::testAddUserToGroup_WritesIntoDocumentAction()
{
	AddFixture fixture;
	const CAssistantRequestContext context = CreateContext(s_permissions);

	IAssistantAction::ChangePreview preview;
	QString error;
	QCOMPARE(fixture.action.PrepareChange(context, fixture.parameters, preview, error), IAssistantAction::ES_SUCCEEDED);

	IAssistantAction::Result result;
	QCOMPARE(fixture.action.ApplyChange(context, fixture.parameters, preview.stateToken, result, error), IAssistantAction::ES_SUCCEEDED);

	const QByteArray documentActionId = result.data.value("documentActionId").toString().toUtf8();
	QVERIFY(!documentActionId.isEmpty());
	QVERIFY(fixture.action.GetResultSchema().Validate(result.data));

	// Only the changing request carries the document action
	bool isUpdateFound = false;
	for (const QPair<QByteArray, QByteArray>& call: fixture.handler->documentActionHeaders){
		if (call.first == "GroupUpdate"){
			QCOMPARE(call.second, documentActionId);
			isUpdateFound = true;
		}
		else{
			QVERIFY(call.second.isEmpty());
		}
	}
	QVERIFY(isUpdateFound);

	const imtdb::IDocumentActionJournal::DocumentActionInfo info = fixture.journal->documentActions.value(documentActionId);
	QCOMPARE(info.state, imtdb::IDocumentActionJournal::DAS_APPLIED);
	QCOMPARE(info.ownerId, s_userId);
	QCOMPARE(info.tenantId, s_tenantId);
	QCOMPARE(info.origin, QByteArray("assistant"));
	QVERIFY(info.summary.contains(QStringLiteral("Support")));
}


void CAssistantDocumentActionTest::testAddUserToGroup_FailedDocumentAction()
{
	AddFixture fixture;
	const CAssistantRequestContext context = CreateContext(s_permissions);

	IAssistantAction::ChangePreview preview;
	QString error;
	QCOMPARE(fixture.action.PrepareChange(context, fixture.parameters, preview, error), IAssistantAction::ES_SUCCEEDED);

	fixture.handler->refusedCommands.insert("GroupUpdate");

	IAssistantAction::Result result;
	QCOMPARE(fixture.action.ApplyChange(context, fixture.parameters, preview.stateToken, result, error), IAssistantAction::ES_FAILED);

	QCOMPARE(fixture.journal->documentActions.size(), 1);
	QCOMPARE(fixture.journal->documentActions.first().state, imtdb::IDocumentActionJournal::DAS_FAILED);
}


void CAssistantDocumentActionTest::testAddUserToGroup_NothingChangedWithoutDocumentAction()
{
	AddFixture fixture;
	const CAssistantRequestContext context = CreateContext(s_permissions);

	IAssistantAction::ChangePreview preview;
	QString error;
	QCOMPARE(fixture.action.PrepareChange(context, fixture.parameters, preview, error), IAssistantAction::ES_SUCCEEDED);

	fixture.journal->failBegin = true;
	fixture.handler->documentActionHeaders.clear();

	IAssistantAction::Result result;
	QCOMPARE(fixture.action.ApplyChange(context, fixture.parameters, preview.stateToken, result, error), IAssistantAction::ES_FAILED);
	QVERIFY(error.contains(QStringLiteral("nothing was changed")));

	for (const QPair<QByteArray, QByteArray>& call: fixture.handler->documentActionHeaders){
		QVERIFY(call.first != "GroupUpdate");
	}
}


void CAssistantDocumentActionTest::testPlan_DocumentActionOfAllSteps()
{
	PlanFixture fixture;
	const CAssistantRequestContext context = CreateContext(s_permissions);

	IAssistantAction::ChangePreview preview;
	QString error;
	QCOMPARE(fixture.plan.PrepareChange(context, fixture.parameters, preview, error), IAssistantAction::ES_SUCCEEDED);
	QVERIFY(fixture.journal->documentActions.isEmpty());

	IAssistantAction::Result result;
	QCOMPARE(fixture.plan.ApplyChange(context, fixture.parameters, preview.stateToken, result, error), IAssistantAction::ES_SUCCEEDED);
	QVERIFY(fixture.plan.GetResultSchema().Validate(result.data));

	const QByteArray documentActionId = result.data.value("documentActionId").toString().toUtf8();
	QVERIFY(!documentActionId.isEmpty());
	QCOMPARE(fixture.handler->documentActionHeaders.size(), 2);
	QCOMPARE(fixture.handler->documentActionHeaders.at(0).second, documentActionId);
	QCOMPARE(fixture.handler->documentActionHeaders.at(1).second, documentActionId);

	const imtdb::IDocumentActionJournal::DocumentActionInfo info = fixture.journal->documentActions.value(documentActionId);
	QCOMPARE(info.state, imtdb::IDocumentActionJournal::DAS_APPLIED);
	QCOMPARE(info.summary, QStringLiteral("Create Support and rename it"));
}


void CAssistantDocumentActionTest::testPlan_PartiallyAppliedDocumentAction()
{
	PlanFixture fixture;
	fixture.handler->refusedCommands.insert("GroupUpdate");
	const CAssistantRequestContext context = CreateContext(s_permissions);

	IAssistantAction::ChangePreview preview;
	QString error;
	QCOMPARE(fixture.plan.PrepareChange(context, fixture.parameters, preview, error), IAssistantAction::ES_SUCCEEDED);

	IAssistantAction::Result result;
	QCOMPARE(fixture.plan.ApplyChange(context, fixture.parameters, preview.stateToken, result, error), IAssistantAction::ES_FAILED);

	QCOMPARE(fixture.journal->documentActions.size(), 1);
	const imtdb::IDocumentActionJournal::DocumentActionInfo info = fixture.journal->documentActions.first();
	QCOMPARE(info.state, imtdb::IDocumentActionJournal::DAS_PARTIALLY_APPLIED);

	// The user learns which document action holds the applied step
	QVERIFY(error.contains(QString::fromUtf8(info.id)));

	// The first step failing: nothing applied
	PlanFixture failing;
	failing.handler->refusedCommands.insert("GroupAdd");
	QCOMPARE(failing.plan.PrepareChange(context, failing.parameters, preview, error), IAssistantAction::ES_SUCCEEDED);
	QCOMPARE(failing.plan.ApplyChange(context, failing.parameters, preview.stateToken, result, error), IAssistantAction::ES_FAILED);
	QCOMPARE(failing.journal->documentActions.first().state, imtdb::IDocumentActionJournal::DAS_FAILED);
}


void CAssistantDocumentActionTest::testServerHeadersAreRemovedFromClientRequests()
{
	imtgql::IGqlContext::Headers headers;
	headers.insert("x-authentication-token", "token");
	headers.insert(imtbase::s_documentActionHeaderId, "11111111-1111-1111-1111-111111111111");
	headers.insert("X-IMT-Document-Action", "22222222-2222-2222-2222-222222222222");

	const imtgql::IGqlContext::Headers filtered = imtgql::RemoveServerHeaders(headers);
	QCOMPARE(filtered.size(), 1);
	QCOMPARE(filtered.value("x-authentication-token"), QByteArray("token"));
}


void CAssistantDocumentActionTest::testList_OwnDocumentActionsAndReversibility()
{
	RollbackFixture fixture("list");
	const QByteArray reversibleId = fixture.AddUpdate();
	const QByteArray foreignTableId = fixture.journal->AddDocumentAction(s_userId, imtdb::IDocumentActionJournal::DAS_APPLIED, {CreateEntry("u1", imtdb::IDocumentActionJournal::EO_UPDATED, 3, 4, "Users")});
	const QByteArray otherUserId = fixture.AddUpdate(s_otherUserId);

	IAssistantAction::Result result;
	QString error;
	QCOMPARE(fixture.action.Execute(CreateContext(s_permissions), QJsonObject(), result, error), IAssistantAction::ES_SUCCEEDED);
	QVERIFY(fixture.action.GetResultSchema().Validate(result.data));

	const QJsonArray items = result.data.value("documentActions").toArray();
	QCOMPARE(items.size(), 2);

	QMap<QString, QJsonObject> byId;
	for (const QJsonValue& value: items){
		byId.insert(value.toObject().value("documentActionId").toString(), value.toObject());
	}

	QVERIFY(byId.value(QString::fromUtf8(reversibleId)).value("reversible").toBool());
	QVERIFY(!byId.value(QString::fromUtf8(foreignTableId)).value("reversible").toBool());
	QVERIFY(!byId.value(QString::fromUtf8(foreignTableId)).value("reason").toString().isEmpty());
	QVERIFY(!byId.contains(QString::fromUtf8(otherUserId)));
}


void CAssistantDocumentActionTest::testList_AllUsersNeedsPermission()
{
	RollbackFixture fixture("list");
	fixture.AddUpdate();
	fixture.AddUpdate(s_otherUserId);

	QJsonObject parameters;
	parameters["allUsers"] = true;

	QVERIFY(!fixture.action.CheckAccess(CreateContext(s_permissions), parameters));

	const CAssistantRequestContext context = CreateContext(QByteArrayList(s_permissions) << s_rollbackAnyPermission);
	QVERIFY(fixture.action.CheckAccess(context, parameters));

	IAssistantAction::Result result;
	QString error;
	QCOMPARE(fixture.action.Execute(context, parameters, result, error), IAssistantAction::ES_SUCCEEDED);
	QCOMPARE(result.data.value("documentActions").toArray().size(), 2);
}


void CAssistantDocumentActionTest::testRollback_RestoresFormerRevision()
{
	RollbackFixture fixture;
	const QByteArray documentActionId = fixture.AddUpdate();
	const CAssistantRequestContext context = CreateContext(s_permissions);
	const QJsonObject parameters = CreateRollbackParameters(documentActionId);

	QVERIFY(fixture.action.CheckAccess(context, parameters));

	IAssistantAction::ChangePreview preview;
	QString error;
	QCOMPARE(fixture.action.PrepareChange(context, parameters, preview, error), IAssistantAction::ES_SUCCEEDED);
	QCOMPARE(preview.changes.size(), 1);
	QCOMPARE(preview.changes.at(0).toObject().value("field").toString(), QStringLiteral("Support"));
	QVERIFY(fixture.collection->writes.isEmpty());

	IAssistantAction::Result result;
	QCOMPARE(fixture.action.ApplyChange(context, parameters, preview.stateToken, result, error), IAssistantAction::ES_SUCCEEDED);
	QVERIFY(fixture.action.GetResultSchema().Validate(result.data));
	QCOMPARE(result.data.value("restoredDocuments").toInt(), 1);

	QCOMPARE(fixture.collection->GetText("g1"), QStringLiteral("old"));

	// The rollback is a document action of its own, written with the context of the user
	const QByteArray rollbackId = result.data.value("documentActionId").toString().toUtf8();
	QCOMPARE(fixture.collection->writes.size(), 1);
	QCOMPARE(fixture.collection->writes.at(0).kind, QByteArray("set"));
	QCOMPARE(fixture.collection->writes.at(0).documentActionId, rollbackId);
	QCOMPARE(fixture.collection->writes.at(0).tenantId, s_tenantId);
	QCOMPARE(fixture.collection->writes.at(0).ownerId, s_userId);

	const imtdb::IDocumentActionJournal::DocumentActionInfo rollbackInfo = fixture.journal->documentActions.value(rollbackId);
	QCOMPARE(rollbackInfo.origin, QByteArray("rollback"));
	QCOMPARE(rollbackInfo.referenceId, documentActionId);
	QCOMPARE(rollbackInfo.state, imtdb::IDocumentActionJournal::DAS_APPLIED);
	QCOMPARE(fixture.journal->documentActions.value(documentActionId).state, imtdb::IDocumentActionJournal::DAS_ROLLED_BACK);
}


void CAssistantDocumentActionTest::testRollback_RemovesCreatedDocument()
{
	RollbackFixture fixture;
	// Created and then changed in the same document action: the rollback removes it
	const QByteArray documentActionId = fixture.journal->AddDocumentAction(
				s_userId,
				imtdb::IDocumentActionJournal::DAS_APPLIED,
				{CreateEntry("g1", imtdb::IDocumentActionJournal::EO_CREATED, -1, 1), CreateEntry("g1", imtdb::IDocumentActionJournal::EO_UPDATED, 1, 2)});
	const CAssistantRequestContext context = CreateContext(s_permissions);
	const QJsonObject parameters = CreateRollbackParameters(documentActionId);

	IAssistantAction::ChangePreview preview;
	QString error;
	QCOMPARE(fixture.action.PrepareChange(context, parameters, preview, error), IAssistantAction::ES_SUCCEEDED);
	QCOMPARE(preview.changes.size(), 1);
	QCOMPARE(preview.changes.at(0).toObject().value("newValue").toString(), QStringLiteral("removed"));

	IAssistantAction::Result result;
	QCOMPARE(fixture.action.ApplyChange(context, parameters, preview.stateToken, result, error), IAssistantAction::ES_SUCCEEDED);

	QCOMPARE(fixture.collection->writes.size(), 1);
	QCOMPARE(fixture.collection->writes.at(0).kind, QByteArray("remove"));
	QVERIFY(!fixture.collection->GetElementIds().contains("g1"));
}


void CAssistantDocumentActionTest::testRollback_RefusedIfDocumentChangedLater()
{
	RollbackFixture fixture;
	const QByteArray documentActionId = fixture.AddUpdate();
	fixture.journal->documentStates.insert(s_tableName + "/g1", CreateState(3));

	IAssistantAction::ChangePreview preview;
	QString error;
	QCOMPARE(fixture.action.PrepareChange(CreateContext(s_permissions), CreateRollbackParameters(documentActionId), preview, error), IAssistantAction::ES_FAILED);
	QVERIFY(error.contains(QStringLiteral("Support")));

	// Removed later counts as changed
	fixture.journal->documentStates.insert(s_tableName + "/g1", CreateState(2, true));
	QCOMPARE(fixture.action.PrepareChange(CreateContext(s_permissions), CreateRollbackParameters(documentActionId), preview, error), IAssistantAction::ES_FAILED);
	QVERIFY(fixture.collection->writes.isEmpty());
}


void CAssistantDocumentActionTest::testRollback_DocumentActionOfOtherUser()
{
	RollbackFixture fixture;
	const QByteArray documentActionId = fixture.AddUpdate(s_otherUserId);

	IAssistantAction::ChangePreview preview;
	QString error;
	QCOMPARE(fixture.action.PrepareChange(CreateContext(s_permissions), CreateRollbackParameters(documentActionId), preview, error), IAssistantAction::ES_FAILED);
	QCOMPARE(error, QStringLiteral("The document action was not found."));

	QCOMPARE(fixture.action.PrepareChange(CreateContext(QByteArrayList(s_permissions) << s_rollbackAnyPermission), CreateRollbackParameters(documentActionId), preview, error), IAssistantAction::ES_SUCCEEDED);
}


void CAssistantDocumentActionTest::testRollback_NeedsPermissionOfCollection()
{
	RollbackFixture fixture;
	const QByteArray documentActionId = fixture.AddUpdate();

	IAssistantAction::ChangePreview preview;
	QString error;
	QCOMPARE(fixture.action.PrepareChange(CreateContext({"/Administration/Groups/View", s_removePermission}), CreateRollbackParameters(documentActionId), preview, error), IAssistantAction::ES_ACCESS_DENIED);

	// A created document needs the remove permission
	const QByteArray createdId = fixture.journal->AddDocumentAction(s_userId, imtdb::IDocumentActionJournal::DAS_APPLIED, {CreateEntry("g1", imtdb::IDocumentActionJournal::EO_CREATED, -1, 2)});
	QCOMPARE(fixture.action.PrepareChange(CreateContext({"/Administration/Groups/View", s_editPermission}), CreateRollbackParameters(createdId), preview, error), IAssistantAction::ES_ACCESS_DENIED);
	QVERIFY(fixture.collection->writes.isEmpty());
}


void CAssistantDocumentActionTest::testRollback_StaleAfterPreview()
{
	RollbackFixture fixture;
	const QByteArray documentActionId = fixture.AddUpdate();
	const CAssistantRequestContext context = CreateContext(s_permissions);
	const QJsonObject parameters = CreateRollbackParameters(documentActionId);

	IAssistantAction::ChangePreview preview;
	QString error;
	QCOMPARE(fixture.action.PrepareChange(context, parameters, preview, error), IAssistantAction::ES_SUCCEEDED);

	// Changed after the preview: refused at the application (the check of the current documents)
	fixture.journal->documentStates.insert(s_tableName + "/g1", CreateState(3));

	IAssistantAction::Result result;
	QCOMPARE(fixture.action.ApplyChange(context, parameters, preview.stateToken, result, error), IAssistantAction::ES_FAILED);

	// A token of another state is stale
	fixture.journal->documentStates.insert(s_tableName + "/g1", CreateState(2));
	QCOMPARE(fixture.action.ApplyChange(context, parameters, "other", result, error), IAssistantAction::ES_STALE);
	QVERIFY(fixture.collection->writes.isEmpty());
	QCOMPARE(fixture.journal->documentActions.value(documentActionId).state, imtdb::IDocumentActionJournal::DAS_APPLIED);
}


void CAssistantDocumentActionTest::testRollback_RefusedForRemovalsAndUnrecordedWrites()
{
	RollbackFixture fixture;
	const CAssistantRequestContext context = CreateContext(s_permissions);

	const QByteArray removedId = fixture.journal->AddDocumentAction(
				s_userId,
				imtdb::IDocumentActionJournal::DAS_APPLIED,
				{CreateEntry("g1", imtdb::IDocumentActionJournal::EO_UPDATED, 1, 2), CreateEntry("g2", imtdb::IDocumentActionJournal::EO_REMOVED, 4, 4)});
	const QByteArray unrecordedId = fixture.journal->AddDocumentAction(
				s_userId,
				imtdb::IDocumentActionJournal::DAS_APPLIED,
				{CreateEntry("g1", imtdb::IDocumentActionJournal::EO_UPDATED, 1, 2), CreateEntry(QByteArray(), imtdb::IDocumentActionJournal::EO_UNRECORDED, -1, -1, "Tenants")});

	IAssistantAction::ChangePreview preview;
	QString error;
	QCOMPARE(fixture.action.PrepareChange(context, CreateRollbackParameters(removedId), preview, error), IAssistantAction::ES_FAILED);
	QVERIFY(error.contains(QStringLiteral("removed")));

	QCOMPARE(fixture.action.PrepareChange(context, CreateRollbackParameters(unrecordedId), preview, error), IAssistantAction::ES_FAILED);
	QVERIFY(error.contains(QStringLiteral("without revisions")));
	QVERIFY(fixture.collection->writes.isEmpty());
}


void CAssistantDocumentActionTest::testRollback_RefusedTwice()
{
	RollbackFixture fixture;
	const QByteArray documentActionId = fixture.AddUpdate();
	const CAssistantRequestContext context = CreateContext(s_permissions);
	const QJsonObject parameters = CreateRollbackParameters(documentActionId);

	IAssistantAction::ChangePreview preview;
	QString error;
	QCOMPARE(fixture.action.PrepareChange(context, parameters, preview, error), IAssistantAction::ES_SUCCEEDED);

	IAssistantAction::Result result;
	QCOMPARE(fixture.action.ApplyChange(context, parameters, preview.stateToken, result, error), IAssistantAction::ES_SUCCEEDED);

	QCOMPARE(fixture.action.PrepareChange(context, parameters, preview, error), IAssistantAction::ES_FAILED);
	QCOMPARE(error, QStringLiteral("The document action was rolled back already."));
}


I_ADD_TEST(CAssistantDocumentActionTest);


