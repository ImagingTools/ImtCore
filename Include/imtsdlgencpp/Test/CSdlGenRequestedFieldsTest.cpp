// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSdlGenTest.h"


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtTest/QTest>

// ImtCore includes
#include <GeneratedFiles/sdlgentestsdl/SDL/1.0/CPP/RequestedFields.h>
#include <imtgql/CGqlRequest.h>


namespace imtsdlgentest
{


namespace
{


using CItemsListGqlRequest = sdl::V1_0::sdlgentest::CItemsListGqlRequest;
using CReportGqlRequest = sdl::V1_0::sdlgentest::CReportGqlRequest;
using CSaveItemGqlRequest = sdl::V1_0::sdlgentest::CSaveItemGqlRequest;
using ItemsListRequestInfo = sdl::V1_0::sdlgentest::ItemsListRequestInfo;
using ReportRequestInfo = sdl::V1_0::sdlgentest::ReportRequestInfo;
using SaveItemRequestInfo = sdl::V1_0::sdlgentest::SaveItemRequestInfo;

typedef QMap<QByteArray, bool> FlagMap;


// ItemsListRequestInfo::itemsRequestInfo and SaveItemRequestInfo describe the same type "Item"
template <typename ItemRequestInfo>
void AddItemFlags(FlagMap& flags, const QByteArray& prefix, const ItemRequestInfo& info)
{
	flags[prefix + "id"] = info.isIdRequested;
	flags[prefix + "name"] = info.isNameRequested;
	flags[prefix + "status"] = info.isStatusRequested;
	flags[prefix + "keywords"] = info.isKeywordsRequested;
	flags[prefix + "info"] = info.isInfoRequested;
	flags[prefix + "data"] = info.isDataRequested;
	flags[prefix + "content"] = info.isContentRequested;
	flags[prefix + "node"] = info.isNodeRequested;
	flags[prefix + "cycle"] = info.isCycleRequested;

	flags[prefix + "info.description"] = info.info.isDescriptionRequested;
	flags[prefix + "info.owner"] = info.info.isOwnerRequested;
	flags[prefix + "info.tags"] = info.info.isTagsRequested;
	flags[prefix + "info.owner.login"] = info.info.owner.isLoginRequested;
	flags[prefix + "info.owner.name"] = info.info.owner.isNameRequested;
	flags[prefix + "info.owner.contact"] = info.info.owner.isContactRequested;
	flags[prefix + "info.owner.contact.email"] = info.info.owner.contact.isEmailRequested;
	flags[prefix + "info.owner.contact.phone"] = info.info.owner.contact.isPhoneRequested;
	flags[prefix + "info.owner.contact.address"] = info.info.owner.contact.isAddressRequested;
	flags[prefix + "info.owner.contact.address.city"] = info.info.owner.contact.address.isCityRequested;
	flags[prefix + "info.owner.contact.address.street"] = info.info.owner.contact.address.isStreetRequested;
	flags[prefix + "info.owner.contact.address.zip"] = info.info.owner.contact.address.isZipRequested;
	flags[prefix + "info.tags.name"] = info.info.tags.isNameRequested;
	flags[prefix + "info.tags.color"] = info.info.tags.isColorRequested;

	flags[prefix + "data.data"] = info.data.isDataRequested;
	flags[prefix + "data.info"] = info.data.isInfoRequested;
	flags[prefix + "data.label"] = info.data.isLabelRequested;
	flags[prefix + "data.data.value"] = info.data.data.isValueRequested;
	flags[prefix + "data.data.note"] = info.data.data.isNoteRequested;
	flags[prefix + "data.info.value"] = info.data.info.isValueRequested;
	flags[prefix + "data.info.note"] = info.data.info.isNoteRequested;

	flags[prefix + "node.name"] = info.node.isNameRequested;
	flags[prefix + "node.parentNode"] = info.node.isParentNodeRequested;
	flags[prefix + "node.childNodes"] = info.node.isChildNodesRequested;

	flags[prefix + "cycle.name"] = info.cycle.isNameRequested;
	flags[prefix + "cycle.b"] = info.cycle.isBRequested;
	flags[prefix + "cycle.b.title"] = info.cycle.b.isTitleRequested;
	flags[prefix + "cycle.b.a"] = info.cycle.b.isARequested;
}


FlagMap GetFlags(const ItemsListRequestInfo& info)
{
	FlagMap flags;
	flags["items"] = info.isItemsRequested;
	flags["totalCount"] = info.isTotalCountRequested;
	flags["notification"] = info.isNotificationRequested;
	flags["notification.text"] = info.notification.isTextRequested;
	flags["notification.level"] = info.notification.isLevelRequested;
	AddItemFlags(flags, "items.", info.items);

	return flags;
}


FlagMap GetFlags(const SaveItemRequestInfo& info)
{
	FlagMap flags;
	AddItemFlags(flags, QByteArray(), info);

	return flags;
}


FlagMap GetFlags(const ReportRequestInfo& info)
{
	FlagMap flags;
	flags["Report"] = info.isReportRequested;
	flags["summary"] = info.isSummaryRequested;
	flags["Report.title"] = info.Report.isTitleRequested;
	flags["Report.pages"] = info.Report.isPagesRequested;

	return flags;
}


// Every flag is expected to be set, except the listed ones
QString CompareFlags(const FlagMap& actualFlags, const QByteArrayList& expectedUnsetFlags)
{
	QStringList errors;
	for (const QByteArray& flagPath: expectedUnsetFlags){
		if (!actualFlags.contains(flagPath)){
			errors << QStringLiteral("unknown flag '%1'").arg(QString::fromUtf8(flagPath));
		}
	}

	for (auto it = actualFlags.cbegin(); it != actualFlags.cend(); ++it){
		const bool expectedValue = !expectedUnsetFlags.contains(it.key());
		if (it.value() != expectedValue){
			errors << QStringLiteral("%1: expected %2").arg(QString::fromUtf8(it.key()), expectedValue ? QStringLiteral("true") : QStringLiteral("false"));
		}
	}

	return errors.join(QStringLiteral("; "));
}


bool ParseRequest(const QByteArray& queryText, imtgql::CGqlRequest& request)
{
	QJsonObject rootObject;
	rootObject[QStringLiteral("query")] = QString::fromUtf8(queryText);

	qsizetype errorPosition = -1;

	return request.ParseQuery(QJsonDocument(rootObject).toJson(QJsonDocument::Compact), errorPosition);
}


// All fields of "Item" except the given ones; "id" is required and is therefore always set
QByteArrayList GetUnsetItemFields(const QByteArray& prefix, const QByteArrayList& requestedFieldIds)
{
	static const QByteArrayList optionalFieldIds = {"name", "status", "keywords", "info", "data", "content", "node", "cycle"};

	QByteArrayList retVal;
	for (const QByteArray& fieldId: optionalFieldIds){
		if (!requestedFieldIds.contains(fieldId)){
			retVal << prefix + fieldId;
		}
	}

	return retVal;
}


} // namespace


void CSdlGenTest::TestRequestedFields_data()
{
	QTest::addColumn<QByteArray>("query");
	QTest::addColumn<QByteArrayList>("expectedUnsetFlags");

	const QByteArray header = "query ItemsList {ItemsList(input: {offset: 10, count: 5}) ";
	const QByteArrayList notTopLevelItems = {"totalCount", "notification"};

	QTest::newRow("no selection set")
				<< header + "{}}"
				<< QByteArrayList();

	QTest::newRow("top-level scalar only")
				<< header + "{totalCount}}"
				<< QByteArrayList{"items", "notification"};

	QTest::newRow("nested flags of a not requested object keep their defaults")
				<< header + "{notification {text}}}"
				<< QByteArrayList{"items", "totalCount", "notification.level"};

	QTest::newRow("list items with scalars")
				<< header + "{items {id name}}}"
				<< notTopLevelItems + GetUnsetItemFields("items.", {"name"});

	QTest::newRow("required field is always set")
				<< header + "{items {name}}}"
				<< notTopLevelItems + GetUnsetItemFields("items.", {"name"});

	QTest::newRow("enum and scalar array")
				<< header + "{items {status keywords}}}"
				<< notTopLevelItems + GetUnsetItemFields("items.", {"status", "keywords"});

	QTest::newRow("five levels deep")
				<< header + "{items {info {owner {contact {address {city}}}}}}}"
				<< notTopLevelItems + GetUnsetItemFields("items.", {"info"}) + QByteArrayList{
						"items.info.description",
						"items.info.tags",
						"items.info.owner.name",
						"items.info.owner.contact.email",
						"items.info.owner.contact.phone",
						"items.info.owner.contact.address.street",
						"items.info.owner.contact.address.zip"};

	QTest::newRow("siblings on every level")
				<< header + "{totalCount notification {level} items {name info {description tags {color} owner {name contact {phone address {zip street}}}}}}}"
				<< GetUnsetItemFields("items.", {"name", "info"}) + QByteArrayList{
						"notification.text",
						"items.info.tags.name",
						"items.info.owner.contact.email",
						"items.info.owner.contact.address.city"};

	QTest::newRow("same field name on neighbouring levels")
				<< header + "{items {data {data {value} info {note}}}}}"
				<< notTopLevelItems + GetUnsetItemFields("items.", {"data"}) + QByteArrayList{
						"items.data.label",
						"items.data.data.note",
						"items.data.info.value"};

	QTest::newRow("same field name on neighbouring levels, inner selected after sibling")
				<< header + "{items {data {label info {value note} data {note}}}}}"
				<< notTopLevelItems + GetUnsetItemFields("items.", {"data"}) + QByteArrayList{
						"items.data.data.value"};

	QTest::newRow("self-nested type")
				<< header + "{items {node {name childNodes {name childNodes {name}}}}}}"
				<< notTopLevelItems + GetUnsetItemFields("items.", {"node"}) + QByteArrayList{
						"items.node.parentNode"};

	QTest::newRow("indirectly self-nested type")
				<< header + "{items {cycle {b {a {name b {title}}}}}}}"
				<< notTopLevelItems + GetUnsetItemFields("items.", {"cycle"}) + QByteArrayList{
						"items.cycle.name",
						"items.cycle.b.title"};

	QTest::newRow("union with inline fragments")
				<< header + "{items {content {... on TextContent {text} ... on ImageContent {url}} name}}}"
				<< notTopLevelItems + GetUnsetItemFields("items.", {"content", "name"});

	QTest::newRow("commas, line breaks and tabs between fields")
				<< header + "{\n\titems {\n\t\tname,\n\t\tinfo {\r\n\t\t\tdescription\n\t\t}\n\t},\n\ttotalCount\n}}"
				<< GetUnsetItemFields("items.", {"name", "info"}) + QByteArrayList{
						"notification",
						"items.info.owner",
						"items.info.tags"};

	QTest::newRow("format of the QML request builder")
				<< QByteArray("query ItemsList {ItemsList(input: {offset: 10, count: 5}) {items {id name info {owner {login}}} totalCount}}")
				<< GetUnsetItemFields("items.", {"name", "info"}) + QByteArrayList{
						"notification",
						"items.info.description",
						"items.info.tags",
						"items.info.owner.name",
						"items.info.owner.contact"};
}


void CSdlGenTest::TestRequestedFields()
{
	QFETCH(QByteArray, query);
	QFETCH(QByteArrayList, expectedUnsetFlags);

	imtgql::CGqlRequest gqlRequest;
	QVERIFY2(ParseRequest(query, gqlRequest), query.constData());
	QCOMPARE(gqlRequest.GetCommandId(), CItemsListGqlRequest::GetCommandId());

	const CItemsListGqlRequest itemsListRequest(gqlRequest, false);
	QVERIFY(itemsListRequest.IsValid());

	// the selection set must not disturb reading of the input arguments
	const auto& arguments = itemsListRequest.GetRequestedArguments();
	QVERIFY(arguments.input.HasValue());
	QVERIFY(arguments.input->offset.HasValue());
	QCOMPARE(*arguments.input->offset, 10);
	QVERIFY(arguments.input->count.HasValue());
	QCOMPARE(*arguments.input->count, 5);

	const QString errors = CompareFlags(GetFlags(itemsListRequest.GetRequestInfo()), expectedUnsetFlags);
	QVERIFY2(errors.isEmpty(), qPrintable(errors));
}


void CSdlGenTest::TestRequestedFieldsWithComplexArguments()
{
	// braces, brackets and keywords inside of the arguments must not leak into the selection set
	const QByteArray query =
				"query ItemsList {ItemsList(input: {offset: 1, filter: {text: \"items {name} [x] ... on Item\", statuses: [\"ACTIVE\", \"ARCHIVED\"]}}) "
				"{items {data {label}}}}";

	imtgql::CGqlRequest gqlRequest;
	QVERIFY(ParseRequest(query, gqlRequest));

	const CItemsListGqlRequest itemsListRequest(gqlRequest, false);
	QVERIFY(itemsListRequest.IsValid());

	const auto& arguments = itemsListRequest.GetRequestedArguments();
	QVERIFY(arguments.input.HasValue());
	QVERIFY(arguments.input->filter.HasValue());
	QVERIFY(arguments.input->filter->text.HasValue());
	QCOMPARE(*arguments.input->filter->text, QStringLiteral("items {name} [x] ... on Item"));
	QVERIFY(arguments.input->filter->statuses.HasValue());
	QCOMPARE(arguments.input->filter->statuses->count(), 2);

	const QByteArrayList expectedUnsetFlags = GetUnsetItemFields("items.", {"data"}) + QByteArrayList{
				"totalCount",
				"notification",
				"items.data.data",
				"items.data.info"};
	const QString errors = CompareFlags(GetFlags(itemsListRequest.GetRequestInfo()), expectedUnsetFlags);
	QVERIFY2(errors.isEmpty(), qPrintable(errors));
}


void CSdlGenTest::TestRequestedFieldsOfMutation()
{
	// the output type of the command is the object itself, its fields are on the top level
	const QByteArray query = "mutation SaveItem {SaveItem(input: {offset: 0}) {id name data {info {value}} node {parentNode {name}}}}";

	imtgql::CGqlRequest gqlRequest;
	QVERIFY(ParseRequest(query, gqlRequest));
	QCOMPARE(gqlRequest.GetRequestType(), imtgql::IGqlRequest::RT_MUTATION);

	const CSaveItemGqlRequest saveItemRequest(gqlRequest, false);
	QVERIFY(saveItemRequest.IsValid());

	const QByteArrayList expectedUnsetFlags = GetUnsetItemFields(QByteArray(), {"name", "data", "node"}) + QByteArrayList{
				"data.data",
				"data.label",
				"data.info.note",
				"node.name",
				"node.childNodes"};
	const QString errors = CompareFlags(GetFlags(saveItemRequest.GetRequestInfo()), expectedUnsetFlags);
	QVERIFY2(errors.isEmpty(), qPrintable(errors));
}


void CSdlGenTest::TestRequestedFieldsWithFieldNamedAsCommand()
{
	// the only requested field has the name of the command and must not be taken for a wrapper of the selection set
	const QByteArray query = "query Report {Report(input: {offset: 0}) {Report {title}}}";

	imtgql::CGqlRequest gqlRequest;
	QVERIFY(ParseRequest(query, gqlRequest));

	const CReportGqlRequest reportRequest(gqlRequest, false);
	QVERIFY(reportRequest.IsValid());

	const QString errors = CompareFlags(GetFlags(reportRequest.GetRequestInfo()), {"summary", "Report.pages"});
	QVERIFY2(errors.isEmpty(), qPrintable(errors));
}


void CSdlGenTest::TestRequestedFieldsOfBuiltRequest()
{
	// a request composed on the server side and the same request after a round trip through its query text
	imtgql::CGqlFieldObject addressFields;
	addressFields.InsertField("zip");

	imtgql::CGqlFieldObject contactFields;
	contactFields.InsertField("email");
	contactFields.InsertField("address", addressFields);

	imtgql::CGqlFieldObject ownerFields;
	ownerFields.InsertField("contact", contactFields);

	imtgql::CGqlFieldObject infoFields;
	infoFields.InsertField("owner", ownerFields);

	imtgql::CGqlFieldObject leafFields;
	leafFields.InsertField("note");

	imtgql::CGqlFieldObject dataFields;
	dataFields.InsertField("data", leafFields);

	imtgql::CGqlFieldObject itemFields;
	itemFields.InsertField("id");
	itemFields.InsertField("info", infoFields);
	itemFields.InsertField("data", dataFields);

	imtgql::CGqlRequest builtRequest(imtgql::IGqlRequest::RT_QUERY, CItemsListGqlRequest::GetCommandId());
	builtRequest.AddField("items", itemFields);
	builtRequest.AddSimpleField("totalCount");

	const QByteArrayList expectedUnsetFlags = GetUnsetItemFields("items.", {"info", "data"}) + QByteArrayList{
				"notification",
				"items.info.description",
				"items.info.tags",
				"items.info.owner.name",
				"items.info.owner.contact.phone",
				"items.info.owner.contact.address.city",
				"items.info.owner.contact.address.street",
				"items.data.info",
				"items.data.label",
				"items.data.data.value"};

	const CItemsListGqlRequest directRequest(builtRequest, false);
	QVERIFY(directRequest.IsValid());
	QString errors = CompareFlags(GetFlags(directRequest.GetRequestInfo()), expectedUnsetFlags);
	QVERIFY2(errors.isEmpty(), qPrintable(errors));

	const QByteArray queryData = builtRequest.GetQuery();
	imtgql::CGqlRequest reparsedRequest;
	qsizetype errorPosition = -1;
	QVERIFY2(reparsedRequest.ParseQuery(queryData, errorPosition), queryData.constData());

	const CItemsListGqlRequest reparsedItemsListRequest(reparsedRequest, false);
	QVERIFY(reparsedItemsListRequest.IsValid());
	errors = CompareFlags(GetFlags(reparsedItemsListRequest.GetRequestInfo()), expectedUnsetFlags);
	QVERIFY2(errors.isEmpty(), qPrintable(errors));
}


} // namespace imtsdlgentest
