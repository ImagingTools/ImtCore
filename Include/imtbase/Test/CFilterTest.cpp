// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CFilterTest.h"


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtbase/CFilter.h>
#include <imtbase/CFilterSerializer.h>
#include <imtbase/CPaginatedFilter.h>


void CFilterTest::CFilter_TextAndFields()
{
    imtbase::CFilter filter;
    filter.SetTextFilter(QStringLiteral("hello"));
    filter.SetTextFieldIds({ "Name", "Email" });

    QCOMPARE(filter.GetTextFilter(), QStringLiteral("hello"));
    QCOMPARE(filter.GetTextFieldIds().count(), 2);
    QVERIFY(filter.GetTextFieldIds().contains("Name"));
    QVERIFY(filter.GetTextFieldIds().contains("Email"));
}


void CFilterTest::CFilter_FieldExpression()
{
    imtbase::CFilter filter;
    filter.AddFieldFilter(imtbase::CFilter::FieldFilter(
        "Status", QStringLiteral("active"), imtbase::CFilter::FO_EQUAL));

    QCOMPARE(filter.GetFilterExpression().fieldFilters.size(), 1);
    QCOMPARE(filter.GetFilterExpression().fieldFilters.first().fieldId, QByteArray("Status"));
    QCOMPARE(filter.GetFilterExpression().fieldFilters.first().operation, imtbase::CFilter::FO_EQUAL);
}


void CFilterTest::CFilter_SortFields()
{
    imtbase::CFilter filter;
    filter.AddSortField(imtbase::CFilter::SortField("CreatedAt", imtbase::CFilter::SO_DESC));

    QCOMPARE(filter.GetSortFields().size(), 1);
    QCOMPARE(filter.GetSortFields().first().fieldId, QByteArray("CreatedAt"));
    QCOMPARE(filter.GetSortFields().first().sortingOrder, imtbase::CFilter::SO_DESC);
}


void CFilterTest::CFilter_Pagination()
{
    imtbase::CFilter filter;
    filter.SetPage(3);
    filter.SetPageSize(25);

    QVERIFY(filter.HasPagination());
    QCOMPARE(filter.GetLimit(), 25);
    QCOMPARE(filter.GetOffset(), 50);
}


void CFilterTest::CFilter_Clear()
{
    imtbase::CFilter filter;
    filter.SetTextFilter(QStringLiteral("hello"));
    filter.SetTextFieldIds({ "Name" });
    filter.AddSortField(imtbase::CFilter::SortField("Name", imtbase::CFilter::SO_ASC));
    filter.SetPage(2);
    filter.SetPageSize(10);

    filter.Clear();

    QVERIFY(filter.GetTextFilter().isEmpty());
    QVERIFY(filter.GetTextFieldIds().isEmpty());
    QVERIFY(filter.GetSortFields().isEmpty());
    QVERIFY(!filter.HasPagination());
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
    pager.SetPage(0);
    QCOMPARE(pager.GetPage(), 1);
}


void CFilterTest::CPaginatedFilter_SetFilter()
{
    imtbase::CFilter filter;
    filter.SetTextFilter(QStringLiteral("test"));

    imtbase::CPaginatedFilter pager;
    pager.SetFilter(filter);

    QCOMPARE(pager.GetFilter().GetTextFilter(), QStringLiteral("test"));
    QVERIFY(pager.GetFilter().HasPagination());
}


void CFilterTest::CPaginatedFilter_Offset()
{
    imtbase::CPaginatedFilter pager;
    pager.SetPage(3);
    pager.SetPageSize(25);
    QCOMPARE(pager.GetOffset(), 50);
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


void CFilterTest::ToQueryString_TextFilter()
{
    imtbase::CFilter filter;
    filter.SetTextFilter(QStringLiteral("hello"));

    const QString qs = imtbase::CFilterSerializer::ToQueryString(filter);
    QVERIFY(qs.contains(QStringLiteral("text=hello")));
}


void CFilterTest::ToQueryString_WithPagination()
{
    imtbase::CFilter filter;
    filter.SetPage(2);
    filter.SetPageSize(50);

    const QString qs = imtbase::CFilterSerializer::ToQueryString(filter);
    QVERIFY(qs.contains(QStringLiteral("page=2")));
    QVERIFY(qs.contains(QStringLiteral("limit=50")));
}


void CFilterTest::ToQueryString_SortFields()
{
    imtbase::CFilter filter;
    filter.AddSortField(imtbase::CFilter::SortField("Name", imtbase::CFilter::SO_ASC));

    const QString qs = imtbase::CFilterSerializer::ToQueryString(filter);
    QVERIFY(qs.contains(QStringLiteral("sort=Name%3Aasc")) || qs.contains(QStringLiteral("sort=Name:asc")));
}


void CFilterTest::ToQueryString_FieldFilters()
{
    imtbase::CFilter filter;
    filter.AddFieldFilter(imtbase::CFilter::FieldFilter(
        "Status", QStringLiteral("active"), imtbase::CFilter::FO_EQUAL));

    const QString qs = imtbase::CFilterSerializer::ToQueryString(filter);
    QVERIFY(qs.contains(QStringLiteral("filter%5BStatus%5D%5Beq%5D=active")) ||
            qs.contains(QStringLiteral("filter[Status][eq]=active")));
}


void CFilterTest::FromQueryString_TextFilter()
{
    imtbase::CFilter filter;
    QVERIFY(imtbase::CFilterSerializer::FromQueryString(QStringLiteral("text=hello"), filter));
    QCOMPARE(filter.GetTextFilter(), QStringLiteral("hello"));
}


void CFilterTest::FromQueryString_SortFields()
{
    imtbase::CFilter filter;
    QVERIFY(imtbase::CFilterSerializer::FromQueryString(QStringLiteral("sort=Name:desc"), filter));

    QCOMPARE(filter.GetSortFields().size(), 1);
    QCOMPARE(filter.GetSortFields().first().fieldId, QByteArray("Name"));
    QCOMPARE(filter.GetSortFields().first().sortingOrder, imtbase::CFilter::SO_DESC);
}


void CFilterTest::FromQueryString_FieldFilters()
{
    imtbase::CFilter filter;
    QVERIFY(imtbase::CFilterSerializer::FromQueryString(QStringLiteral("filter[Status][eq]=active"), filter));

    const imtbase::CFilter::FilterExpression& expr = filter.GetFilterExpression();
    QCOMPARE(expr.fieldFilters.size(), 1);
    QCOMPARE(expr.fieldFilters.first().fieldId, QByteArray("Status"));
    QCOMPARE(expr.fieldFilters.first().operation, imtbase::CFilter::FO_EQUAL);
    QCOMPARE(expr.fieldFilters.first().value.toString(), QStringLiteral("active"));
}


void CFilterTest::FromQueryString_LeadingQuestionMark()
{
    imtbase::CFilter filter;
    QVERIFY(imtbase::CFilterSerializer::FromQueryString(QStringLiteral("?text=world"), filter));
    QCOMPARE(filter.GetTextFilter(), QStringLiteral("world"));
}


void CFilterTest::ToJson_TextFilter()
{
    imtbase::CFilter filter;
    filter.SetTextFilter(QStringLiteral("hello"));

    const QJsonObject json = imtbase::CFilterSerializer::ToJson(filter);
    QCOMPARE(json[QStringLiteral("text")].toString(), QStringLiteral("hello"));
}


void CFilterTest::ToJson_SortFields()
{
    imtbase::CFilter filter;
    filter.AddSortField(imtbase::CFilter::SortField("CreatedAt", imtbase::CFilter::SO_DESC));

    const QJsonObject json = imtbase::CFilterSerializer::ToJson(filter);
    const QJsonArray sortArray = json[QStringLiteral("sort")].toArray();
    QCOMPARE(sortArray.size(), 1);
    QCOMPARE(sortArray[0].toObject()[QStringLiteral("field")].toString(), QStringLiteral("CreatedAt"));
    QCOMPARE(sortArray[0].toObject()[QStringLiteral("order")].toString(), QStringLiteral("desc"));
}


void CFilterTest::ToJson_FieldFilters()
{
    imtbase::CFilter filter;
    filter.AddFieldFilter(imtbase::CFilter::FieldFilter(
        "Status", QStringLiteral("active"), imtbase::CFilter::FO_EQUAL));

    const QJsonObject json = imtbase::CFilterSerializer::ToJson(filter);
    QVERIFY(json.contains(QStringLiteral("filter")));
    QVERIFY(json[QStringLiteral("filter")].toObject().contains(QStringLiteral("fields")));
}


void CFilterTest::FromJson_TextFilter()
{
    QJsonObject json;
    json[QStringLiteral("text")] = QStringLiteral("hello");

    imtbase::CFilter filter;
    QVERIFY(imtbase::CFilterSerializer::FromJson(json, filter));
    QCOMPARE(filter.GetTextFilter(), QStringLiteral("hello"));
}


void CFilterTest::FromJson_SortFields()
{
    QJsonArray sortArray;
    QJsonObject sortItem;
    sortItem[QStringLiteral("field")] = QStringLiteral("Name");
    sortItem[QStringLiteral("order")] = QStringLiteral("asc");
    sortArray << sortItem;

    QJsonObject json;
    json[QStringLiteral("sort")] = sortArray;

    imtbase::CFilter filter;
    QVERIFY(imtbase::CFilterSerializer::FromJson(json, filter));
    QCOMPARE(filter.GetSortFields().size(), 1);
    QCOMPARE(filter.GetSortFields().first().fieldId, QByteArray("Name"));
    QCOMPARE(filter.GetSortFields().first().sortingOrder, imtbase::CFilter::SO_ASC);
}


void CFilterTest::FromJson_FieldFilters()
{
    QJsonArray fields;
    QJsonObject field;
    field[QStringLiteral("field")] = QStringLiteral("Status");
    field[QStringLiteral("op")] = QStringLiteral("eq");
    field[QStringLiteral("value")] = QStringLiteral("active");
    fields << field;

    QJsonObject expression;
    expression[QStringLiteral("op")] = QStringLiteral("and");
    expression[QStringLiteral("fields")] = fields;

    QJsonObject json;
    json[QStringLiteral("filter")] = expression;

    imtbase::CFilter filter;
    QVERIFY(imtbase::CFilterSerializer::FromJson(json, filter));
    QCOMPARE(filter.GetFilterExpression().fieldFilters.size(), 1);
    QCOMPARE(filter.GetFilterExpression().fieldFilters.first().fieldId, QByteArray("Status"));
    QCOMPARE(filter.GetFilterExpression().fieldFilters.first().value.toString(), QStringLiteral("active"));
}


void CFilterTest::FromJson_Empty()
{
    imtbase::CFilter filter;
    QVERIFY(imtbase::CFilterSerializer::FromJson(QJsonObject(), filter));
    QVERIFY(filter.GetTextFilter().isEmpty());
    QVERIFY(filter.GetTextFieldIds().isEmpty());
    QVERIFY(filter.GetSortFields().isEmpty());
}


I_ADD_TEST(CFilterTest);


