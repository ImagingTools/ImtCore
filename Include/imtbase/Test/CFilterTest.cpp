// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CFilterTest.h"


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

// ImtCore includes
#include <imtbase/CFilter.h>
#include <imtbase/CFilterSerializer.h>
#include <imtbase/CFilterSdlConverter.h>
#include <imtbase/CPaginatedFilter.h>


void CFilterTest::CFilter_Search()
{
    imtbase::CFilter filter;
    filter.SetSearch(QStringLiteral("hello"), { "Name", "Email" });

    QCOMPARE(filter.GetSearch().text, QStringLiteral("hello"));
    QCOMPARE(filter.GetSearch().scopes.count(), 2);
    QVERIFY(filter.GetSearch().IsActive());
}


void CFilterTest::CFilter_Rules()
{
    imtbase::CFilter filter;
    filter.AddRule(imtbase::CFilter::Rule("Status", QStringLiteral("is"), QStringLiteral("active")));

    QCOMPARE(filter.GetRules().rules.size(), 1);
    QCOMPARE(filter.GetRules().rules.first().path, QByteArray("Status"));
    QCOMPARE(filter.GetRules().rules.first().predicate, QStringLiteral("is"));
}


void CFilterTest::CFilter_RuleSets()
{
    imtbase::CFilter::RuleSet any(imtbase::CFilter::RuleSet::Any);
    any.rules << imtbase::CFilter::Rule("Status", QStringLiteral("is"), QStringLiteral("active"));

    imtbase::CFilter filter;
    filter.AddRuleSet(any);

    QCOMPARE(filter.GetRules().children.size(), 1);
    QCOMPARE(filter.GetRules().children.first().join, imtbase::CFilter::RuleSet::Any);
}


void CFilterTest::CFilter_FluentApi()
{
    const imtbase::CFilter filter = imtbase::CFilter()
        .search(QStringLiteral("john"), { "Name", "Email" })
        .where("Status", QStringLiteral("is"), QStringLiteral("active"))
        .any(imtbase::CFilter::AnyOf({
            imtbase::CFilter::Rule("Name", QStringLiteral("like"), QStringLiteral("john")),
            imtbase::CFilter::Rule("Email", QStringLiteral("like"), QStringLiteral("john"))
        }))
        .orderBy("CreatedAt", true)
        .page(2, 50)
        .build();

    QCOMPARE(filter.GetSearch().text, QStringLiteral("john"));
    QCOMPARE(filter.GetSearch().scopes, QByteArrayList({ "Name", "Email" }));
    QCOMPARE(filter.GetRules().rules.size(), 1);
    QCOMPARE(filter.GetRules().children.size(), 1);
    QCOMPARE(filter.GetRules().children.first().join, imtbase::CFilter::RuleSet::Any);
    QCOMPARE(filter.GetOrders().first().path, QByteArray("CreatedAt"));
    QVERIFY(filter.GetOrders().first().descending);
    QCOMPARE(filter.GetWindow().first, 50);
    QCOMPARE(filter.GetWindow().count, 50);
}


void CFilterTest::CFilter_Orders()
{
    imtbase::CFilter filter;
    filter.AddOrder(imtbase::CFilter::Order("CreatedAt", true));

    QCOMPARE(filter.GetOrders().size(), 1);
    QCOMPARE(filter.GetOrders().first().path, QByteArray("CreatedAt"));
    QVERIFY(filter.GetOrders().first().descending);
}


void CFilterTest::CFilter_Window()
{
    imtbase::CFilter filter;
    filter.SetWindow(50, 25);

    QVERIFY(filter.HasWindow());
    QCOMPARE(filter.GetLimit(), 25);
    QCOMPARE(filter.GetOffset(), 50);
}


void CFilterTest::CFilter_Clear()
{
    imtbase::CFilter filter;
    filter.SetSearch(QStringLiteral("hello"), { "Name" });
    filter.AddOrder(imtbase::CFilter::Order("Name"));
    filter.SetWindow(10, 10);

    filter.Clear();

    QVERIFY(!filter.GetSearch().IsActive());
    QVERIFY(filter.GetRules().IsEmpty());
    QVERIFY(filter.GetOrders().isEmpty());
    QVERIFY(!filter.HasWindow());
}


void CFilterTest::CPaginatedFilter_DefaultValues()
{
    imtbase::CPaginatedFilter pager;
    QCOMPARE(pager.GetPage(), 1);
    QCOMPARE(pager.GetPageSize(), 50);
    QCOMPARE(pager.GetTotalCount(), -1);
    QCOMPARE(pager.GetOffset(), 0);
    QCOMPARE(pager.GetLimit(), 50);
}


void CFilterTest::CPaginatedFilter_SetPage()
{
    imtbase::CPaginatedFilter pager;
    pager.SetPage(3);
    QCOMPARE(pager.GetPage(), 3);
    QCOMPARE(pager.GetFilter().GetWindow().first, 100);
    pager.SetPage(0);
    QCOMPARE(pager.GetPage(), 1);
    QCOMPARE(pager.GetFilter().GetWindow().first, 0);
}


void CFilterTest::CPaginatedFilter_SetFilter()
{
    imtbase::CFilter filter;
    filter.SetSearch(QStringLiteral("test"), { "Name" });

    imtbase::CPaginatedFilter pager;
    pager.SetFilter(filter);

    QCOMPARE(pager.GetFilter().GetSearch().text, QStringLiteral("test"));
    QVERIFY(pager.GetFilter().HasWindow());
    QCOMPARE(pager.GetFilter().GetWindow().first, 0);
    QCOMPARE(pager.GetFilter().GetWindow().count, 50);
}


void CFilterTest::CPaginatedFilter_Offset()
{
    imtbase::CPaginatedFilter pager;
    pager.SetPage(3);
    pager.SetPageSize(25);
    QCOMPARE(pager.GetOffset(), 50);
    QCOMPARE(pager.GetFilter().GetWindow().first, 50);
    QCOMPARE(pager.GetFilter().GetWindow().count, 25);
}


void CFilterTest::CPaginatedFilter_TotalPages()
{
    imtbase::CPaginatedFilter pager;
    pager.SetPageSize(50);
    pager.SetTotalCount(312);
    QCOMPARE(pager.GetTotalPages(), 7);
}


void CFilterTest::CPaginatedFilter_HasNextPage()
{
    imtbase::CPaginatedFilter pager;
    pager.SetPageSize(50);
    pager.SetTotalCount(312);
    pager.SetPage(6);
    QVERIFY(pager.HasNextPage());
    pager.SetPage(7);
    QVERIFY(!pager.HasNextPage());
}


void CFilterTest::CPaginatedFilter_HasPreviousPage()
{
    imtbase::CPaginatedFilter pager;
    pager.SetPage(1);
    QVERIFY(!pager.HasPreviousPage());
    pager.SetPage(2);
    QVERIFY(pager.HasPreviousPage());
}


void CFilterTest::ToQueryString_Search()
{
    imtbase::CFilter filter;
    filter.SetSearch(QStringLiteral("hello"), { "Name" });

    const QString qs = imtbase::CFilterSerializer::ToQueryString(filter);
    QVERIFY(qs.contains(QStringLiteral("search=hello")));
    QVERIFY(qs.contains(QStringLiteral("scope=Name")));
}


void CFilterTest::ToQueryString_Window()
{
    imtbase::CFilter filter;
    filter.SetWindow(50, 25);

    const QString qs = imtbase::CFilterSerializer::ToQueryString(filter);
    QVERIFY(qs.contains(QStringLiteral("offset=50")));
    QVERIFY(qs.contains(QStringLiteral("count=25")));
}


void CFilterTest::ToQueryString_Orders()
{
    imtbase::CFilter filter;
    filter.AddOrder(imtbase::CFilter::Order("Name"));

    const QString qs = imtbase::CFilterSerializer::ToQueryString(filter);
    QVERIFY(qs.contains(QStringLiteral("order%5B0%5D.path=Name")) || qs.contains(QStringLiteral("order[0].path=Name")));
    QVERIFY(qs.contains(QStringLiteral("order%5B0%5D.dir=up")) || qs.contains(QStringLiteral("order[0].dir=up")));
}


void CFilterTest::ToQueryString_Rules()
{
    imtbase::CFilter filter;
    filter.AddRule(imtbase::CFilter::Rule("Status", QStringLiteral("is"), QStringLiteral("active")));

    const QString qs = imtbase::CFilterSerializer::ToQueryString(filter);
    QVERIFY(qs.contains(QStringLiteral("rules=")));
    QVERIFY(qs.contains(QStringLiteral("Status")));
}


void CFilterTest::ToQueryString_GroupRules()
{
    imtbase::CFilter filter;
    filter.any(imtbase::CFilter::AnyOf({
        imtbase::CFilter::Rule("Name", QStringLiteral("like"), QStringLiteral("john")),
        imtbase::CFilter::Rule("Email", QStringLiteral("like"), QStringLiteral("john"))
    }));

    const QString qs = imtbase::CFilterSerializer::ToQueryString(filter);
    QVERIFY(qs.contains(QStringLiteral("rules=")));
    QVERIFY(qs.contains(QStringLiteral("any")));
}


void CFilterTest::FromQueryString_Search()
{
    imtbase::CFilter filter;
    QVERIFY(imtbase::CFilterSerializer::FromQueryString(QStringLiteral("search=hello&scope=Name"), filter));
    QCOMPARE(filter.GetSearch().text, QStringLiteral("hello"));
    QCOMPARE(filter.GetSearch().scopes.first(), QByteArray("Name"));
}


void CFilterTest::FromQueryString_Orders()
{
    imtbase::CFilter filter;
    QVERIFY(imtbase::CFilterSerializer::FromQueryString(QStringLiteral("order[0].path=Name&order[0].dir=down"), filter));

    QCOMPARE(filter.GetOrders().size(), 1);
    QCOMPARE(filter.GetOrders().first().path, QByteArray("Name"));
    QVERIFY(filter.GetOrders().first().descending);
}


void CFilterTest::FromQueryString_Rules()
{
    imtbase::CFilter filter;
    QVERIFY(imtbase::CFilterSerializer::FromQueryString(QStringLiteral("rule[0].path=Status&rule[0].pred=is&rule[0].arg=active"), filter));

    QCOMPARE(filter.GetRules().rules.size(), 1);
    QCOMPARE(filter.GetRules().rules.first().path, QByteArray("Status"));
    QCOMPARE(filter.GetRules().rules.first().predicate, QStringLiteral("is"));
    QCOMPARE(filter.GetRules().rules.first().argument.toString(), QStringLiteral("active"));
}


void CFilterTest::FromQueryString_GroupRules()
{
    imtbase::CFilter source;
    source.any(imtbase::CFilter::AnyOf({
        imtbase::CFilter::Rule("Name", QStringLiteral("like"), QStringLiteral("john")),
        imtbase::CFilter::Rule("Email", QStringLiteral("like"), QStringLiteral("john"))
    }));

    imtbase::CFilter filter;
    QVERIFY(imtbase::CFilterSerializer::FromQueryString(imtbase::CFilterSerializer::ToQueryString(source), filter));
    QCOMPARE(filter.GetRules().children.size(), 1);
    QCOMPARE(filter.GetRules().children.first().join, imtbase::CFilter::RuleSet::Any);
    QCOMPARE(filter.GetRules().children.first().rules.size(), 2);
}


void CFilterTest::FromQueryString_LeadingQuestionMark()
{
    imtbase::CFilter filter;
    QVERIFY(imtbase::CFilterSerializer::FromQueryString(QStringLiteral("?search=world&scope=Name"), filter));
    QCOMPARE(filter.GetSearch().text, QStringLiteral("world"));
}


void CFilterTest::ToJson_Search()
{
    imtbase::CFilter filter;
    filter.SetSearch(QStringLiteral("hello"), { "Name" });

    const QJsonObject json = imtbase::CFilterSerializer::ToJson(filter);
    QCOMPARE(json[QStringLiteral("search")].toObject()[QStringLiteral("text")].toString(), QStringLiteral("hello"));
}


void CFilterTest::ToJson_Orders()
{
    imtbase::CFilter filter;
    filter.AddOrder(imtbase::CFilter::Order("CreatedAt", true));

    const QJsonArray orders = imtbase::CFilterSerializer::ToJson(filter)[QStringLiteral("orders")].toArray();
    QCOMPARE(orders.size(), 1);
    QCOMPARE(orders[0].toObject()[QStringLiteral("path")].toString(), QStringLiteral("CreatedAt"));
    QVERIFY(orders[0].toObject()[QStringLiteral("descending")].toBool());
}


void CFilterTest::ToJson_Rules()
{
    imtbase::CFilter filter;
    filter.AddRule(imtbase::CFilter::Rule("Status", QStringLiteral("is"), QStringLiteral("active")));

    const QJsonObject json = imtbase::CFilterSerializer::ToJson(filter);
    QVERIFY(json.contains(QStringLiteral("rules")));
    QCOMPARE(json[QStringLiteral("rules")].toObject()[QStringLiteral("items")].toArray().size(), 1);
}


void CFilterTest::FromJson_Search()
{
    QJsonArray scopes;
    scopes << QStringLiteral("Name");
    QJsonObject search;
    search[QStringLiteral("text")] = QStringLiteral("hello");
    search[QStringLiteral("scope")] = scopes;

    QJsonObject json;
    json[QStringLiteral("search")] = search;

    imtbase::CFilter filter;
    QVERIFY(imtbase::CFilterSerializer::FromJson(json, filter));
    QCOMPARE(filter.GetSearch().text, QStringLiteral("hello"));
    QCOMPARE(filter.GetSearch().scopes.first(), QByteArray("Name"));
}


void CFilterTest::FromJson_Orders()
{
    QJsonArray orders;
    QJsonObject order;
    order[QStringLiteral("path")] = QStringLiteral("Name");
    order[QStringLiteral("descending")] = true;
    orders << order;

    QJsonObject json;
    json[QStringLiteral("orders")] = orders;

    imtbase::CFilter filter;
    QVERIFY(imtbase::CFilterSerializer::FromJson(json, filter));
    QCOMPARE(filter.GetOrders().size(), 1);
    QCOMPARE(filter.GetOrders().first().path, QByteArray("Name"));
    QVERIFY(filter.GetOrders().first().descending);
}


void CFilterTest::FromJson_Rules()
{
    QJsonArray items;
    QJsonObject item;
    item[QStringLiteral("path")] = QStringLiteral("Status");
    item[QStringLiteral("pred")] = QStringLiteral("is");
    item[QStringLiteral("arg")] = QStringLiteral("active");
    items << item;

    QJsonObject rules;
    rules[QStringLiteral("join")] = QStringLiteral("all");
    rules[QStringLiteral("items")] = items;

    QJsonObject json;
    json[QStringLiteral("rules")] = rules;

    imtbase::CFilter filter;
    QVERIFY(imtbase::CFilterSerializer::FromJson(json, filter));
    QCOMPARE(filter.GetRules().rules.size(), 1);
    QCOMPARE(filter.GetRules().rules.first().path, QByteArray("Status"));
    QCOMPARE(filter.GetRules().rules.first().argument.toString(), QStringLiteral("active"));
}


void CFilterTest::FromJson_Empty()
{
    imtbase::CFilter filter;
    QVERIFY(imtbase::CFilterSerializer::FromJson(QJsonObject(), filter));
    QVERIFY(!filter.GetSearch().IsActive());
    QVERIFY(filter.GetRules().IsEmpty());
    QVERIFY(filter.GetOrders().isEmpty());
}


void CFilterTest::SdlConverter_FromQmlModel()
{
    QVariantMap search;
    search[QStringLiteral("text")] = QStringLiteral("john");
    search[QStringLiteral("scope")] = QStringList({ QStringLiteral("Name"), QStringLiteral("Email") });

    QVariantMap rule;
    rule[QStringLiteral("path")] = QStringLiteral("Status");
    rule[QStringLiteral("pred")] = QStringLiteral("is");
    rule[QStringLiteral("arg")] = QStringLiteral("active");

    QVariantMap rules;
    rules[QStringLiteral("join")] = QStringLiteral("all");
    rules[QStringLiteral("items")] = QVariantList({ rule });

    QVariantMap window;
    window[QStringLiteral("first")] = 0;
    window[QStringLiteral("count")] = 50;

    QVariantMap model;
    model[QStringLiteral("search")] = search;
    model[QStringLiteral("rules")] = rules;
    model[QStringLiteral("window")] = window;

    imtbase::CFilter filter;
    QVERIFY(imtbase::CFilterSdlConverter::FromQmlModel(model, filter));
    QCOMPARE(filter.GetSearch().text, QStringLiteral("john"));
    QCOMPARE(filter.GetSearch().scopes, QByteArrayList({ "Name", "Email" }));
    QCOMPARE(filter.GetRules().rules.size(), 1);
    QCOMPARE(filter.GetRules().rules.first().path, QByteArray("Status"));
    QCOMPARE(filter.GetWindow().count, 50);
}


I_ADD_TEST(CFilterTest);


