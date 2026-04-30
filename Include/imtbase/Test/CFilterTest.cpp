// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CFilterTest.h"


// Qt includes
#include <QtCore/QJsonObject>


// ============================================================
// CCollectionFilterAdapter tests
// ============================================================

void CFilterTest::ToComplexFilter_TextFilter()
{
    imtbase::CCollectionFilter legacy;
    legacy.SetTextFilter(QStringLiteral("hello"));
    legacy.SetFilteringInfoIds({ "Name", "Email" });

    imtbase::CComplexCollectionFilter complex =
        imtbase::CCollectionFilterAdapter::ToComplexFilter(legacy);

    QCOMPARE(complex.GetTextFilter(), QStringLiteral("hello"));

    QByteArrayList textFieldIds;
    for (const imtbase::IComplexCollectionFilter::FieldInfo& info : complex.GetFields()){
        if (info.metaInfo.flags & imtbase::IComplexCollectionFilter::SO_TEXT_FILTER){
            textFieldIds << info.id;
        }
    }
    QCOMPARE(textFieldIds.count(), 2);
    QVERIFY(textFieldIds.contains("Name"));
    QVERIFY(textFieldIds.contains("Email"));
}


void CFilterTest::ToComplexFilter_SortInfo()
{
    imtbase::CCollectionFilter legacy;
    legacy.SetSortingInfoIds({ "CreatedAt" });
    legacy.SetSortingOrder(imtbase::ICollectionFilter::SO_DESC);

    imtbase::CComplexCollectionFilter complex =
        imtbase::CCollectionFilterAdapter::ToComplexFilter(legacy);

    const imtbase::IComplexCollectionFilter::Fields& fields = complex.GetFields();
    bool foundSort = false;
    for (const imtbase::IComplexCollectionFilter::FieldInfo& info : fields){
        if (info.id == "CreatedAt" && (info.metaInfo.flags & imtbase::IComplexCollectionFilter::SO_SORT)){
            QCOMPARE(info.metaInfo.sortingOrder, imtbase::IComplexCollectionFilter::SO_DESC);
            foundSort = true;
        }
    }
    QVERIFY(foundSort);
}


void CFilterTest::ToLegacyFilter_TextFilter()
{
    imtbase::CComplexCollectionFilter complex;
    complex.SetTextFilter(QStringLiteral("world"));

    imtbase::IComplexCollectionFilter::Fields fields;
    imtbase::IComplexCollectionFilter::FieldInfo info("Status");
    info.metaInfo.flags = imtbase::IComplexCollectionFilter::SO_TEXT_FILTER;
    fields << info;
    complex.SetFields(fields);

    imtbase::CCollectionFilter legacy;
    imtbase::CCollectionFilterAdapter::ToLegacyFilter(complex, legacy);

    QCOMPARE(legacy.GetTextFilter(), QStringLiteral("world"));
    QVERIFY(legacy.GetFilteringInfoIds().contains("Status"));
}


void CFilterTest::ToLegacyFilter_SortInfo()
{
    imtbase::CComplexCollectionFilter complex;

    imtbase::IComplexCollectionFilter::Fields fields;
    imtbase::IComplexCollectionFilter::FieldInfo info("Name");
    info.metaInfo.flags = imtbase::IComplexCollectionFilter::SO_SORT;
    info.metaInfo.sortingOrder = imtbase::IComplexCollectionFilter::SO_ASC;
    fields << info;
    complex.SetFields(fields);

    imtbase::CCollectionFilter legacy;
    imtbase::CCollectionFilterAdapter::ToLegacyFilter(complex, legacy);

    QCOMPARE(legacy.GetSortingOrder(), imtbase::ICollectionFilter::SO_ASC);
    QVERIFY(legacy.GetSortingInfoIds().contains("Name"));
}


// ============================================================
// CPaginatedFilter tests
// ============================================================

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
    QCOMPARE(pager.GetPage(), 1); // Clamped
}


void CFilterTest::CPaginatedFilter_SetFilter()
{
    imtbase::CComplexCollectionFilter filter;
    filter.SetTextFilter(QStringLiteral("test"));

    imtbase::CPaginatedFilter pager;
    pager.SetFilter(filter);

    QCOMPARE(pager.GetFilter().GetTextFilter(), QStringLiteral("test"));
}


void CFilterTest::CPaginatedFilter_Offset()
{
    imtbase::CPaginatedFilter pager;
    pager.SetPage(3);
    pager.SetPageSize(25);
    QCOMPARE(pager.GetOffset(), 50); // (3-1) * 25
}


void CFilterTest::CPaginatedFilter_TotalPages()
{
    imtbase::CPaginatedFilter pager;
    pager.SetPageSize(50);
    pager.SetTotalCount(312);
    QCOMPARE(pager.GetTotalPages(), 7); // ceil(312/50) = 7
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


// ============================================================
// CFilterSerializer — URL query-string
// ============================================================

void CFilterTest::ToQueryString_TextFilter()
{
    imtbase::CComplexCollectionFilter filter;
    filter.SetTextFilter(QStringLiteral("hello"));

    const QString qs = imtbase::CFilterSerializer::ToQueryString(filter);
    QVERIFY(qs.contains(QStringLiteral("text=")));
    QVERIFY(qs.contains(QStringLiteral("hello")));
}


void CFilterTest::ToQueryString_WithPagination()
{
    imtbase::CComplexCollectionFilter filter;
    const QString qs = imtbase::CFilterSerializer::ToQueryString(filter, 2, 50);
    QVERIFY(qs.contains(QStringLiteral("page=2")));
    QVERIFY(qs.contains(QStringLiteral("limit=50")));
}


void CFilterTest::ToQueryString_SortFields()
{
    imtbase::CComplexCollectionFilter filter;
    imtbase::IComplexCollectionFilter::Fields fields;
    imtbase::IComplexCollectionFilter::FieldInfo info("Name");
    info.metaInfo.flags = imtbase::IComplexCollectionFilter::SO_SORT;
    info.metaInfo.sortingOrder = imtbase::IComplexCollectionFilter::SO_ASC;
    fields << info;
    filter.SetFields(fields);

    const QString qs = imtbase::CFilterSerializer::ToQueryString(filter);
    QVERIFY(qs.contains(QStringLiteral("sort=")));
    QVERIFY(qs.contains(QStringLiteral("Name")));
    QVERIFY(qs.contains(QStringLiteral("asc")));
}


void CFilterTest::ToQueryString_FieldFilters()
{
    imtbase::CComplexCollectionFilter filter;
    imtbase::IComplexCollectionFilter::FilterExpression expr;
    expr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter(
        "Status", QString("active"), imtbase::IComplexCollectionFilter::FO_EQUAL);
    filter.SetFilterExpression(expr);

    const QString qs = imtbase::CFilterSerializer::ToQueryString(filter);
    QVERIFY(qs.contains(QStringLiteral("filter%5BStatus%5D%5Beq%5D=active")) ||
            qs.contains(QStringLiteral("filter[Status][eq]=active")));
}


void CFilterTest::FromQueryString_TextFilter()
{
    imtbase::CComplexCollectionFilter filter;
    const bool ok = imtbase::CFilterSerializer::FromQueryString(
        QStringLiteral("text=hello"), filter);
    QVERIFY(ok);
    QCOMPARE(filter.GetTextFilter(), QStringLiteral("hello"));
}


void CFilterTest::FromQueryString_SortFields()
{
    imtbase::CComplexCollectionFilter filter;
    const bool ok = imtbase::CFilterSerializer::FromQueryString(
        QStringLiteral("sort=Name:desc"), filter);
    QVERIFY(ok);

    bool found = false;
    for (const imtbase::IComplexCollectionFilter::FieldInfo& info : filter.GetFields()){
        if (info.id == "Name" && (info.metaInfo.flags & imtbase::IComplexCollectionFilter::SO_SORT)){
            QCOMPARE(info.metaInfo.sortingOrder, imtbase::IComplexCollectionFilter::SO_DESC);
            found = true;
        }
    }
    QVERIFY(found);
}


void CFilterTest::FromQueryString_FieldFilters()
{
    imtbase::CComplexCollectionFilter filter;
    const bool ok = imtbase::CFilterSerializer::FromQueryString(
        QStringLiteral("filter[Status][eq]=active"), filter);
    QVERIFY(ok);

    const imtbase::IComplexCollectionFilter::FilterExpression& expr = filter.GetFilterExpression();
    QCOMPARE(expr.fieldFilters.size(), 1);
    QCOMPARE(expr.fieldFilters.first().fieldId, QByteArray("Status"));
    QCOMPARE(expr.fieldFilters.first().filterOperation,
             imtbase::IComplexCollectionFilter::FO_EQUAL);
    QCOMPARE(expr.fieldFilters.first().filterValue.toString(), QStringLiteral("active"));
}


void CFilterTest::FromQueryString_LeadingQuestionMark()
{
    imtbase::CComplexCollectionFilter filter;
    const bool ok = imtbase::CFilterSerializer::FromQueryString(
        QStringLiteral("?text=world"), filter);
    QVERIFY(ok);
    QCOMPARE(filter.GetTextFilter(), QStringLiteral("world"));
}


// ============================================================
// CFilterSerializer — JSON
// ============================================================

void CFilterTest::ToJson_TextFilter()
{
    imtbase::CComplexCollectionFilter filter;
    filter.SetTextFilter(QStringLiteral("hello"));

    const QJsonObject obj = imtbase::CFilterSerializer::ToJson(filter);
    QVERIFY(obj.contains(QStringLiteral("text")));
    QCOMPARE(obj[QStringLiteral("text")].toString(), QStringLiteral("hello"));
}


void CFilterTest::ToJson_SortFields()
{
    imtbase::CComplexCollectionFilter filter;
    imtbase::IComplexCollectionFilter::Fields fields;
    imtbase::IComplexCollectionFilter::FieldInfo info("CreatedAt");
    info.metaInfo.flags = imtbase::IComplexCollectionFilter::SO_SORT;
    info.metaInfo.sortingOrder = imtbase::IComplexCollectionFilter::SO_DESC;
    fields << info;
    filter.SetFields(fields);

    const QJsonObject obj = imtbase::CFilterSerializer::ToJson(filter);
    QVERIFY(obj.contains(QStringLiteral("sort")));
    const QJsonArray sortArr = obj[QStringLiteral("sort")].toArray();
    QCOMPARE(sortArr.size(), 1);
    QCOMPARE(sortArr[0].toObject()[QStringLiteral("field")].toString(), QStringLiteral("CreatedAt"));
    QCOMPARE(sortArr[0].toObject()[QStringLiteral("order")].toString(), QStringLiteral("desc"));
}


void CFilterTest::ToJson_FieldFilters()
{
    imtbase::CComplexCollectionFilter filter;
    imtbase::IComplexCollectionFilter::FilterExpression expr;
    expr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter(
        "Status", QString("active"), imtbase::IComplexCollectionFilter::FO_EQUAL);
    filter.SetFilterExpression(expr);

    const QJsonObject obj = imtbase::CFilterSerializer::ToJson(filter);
    QVERIFY(obj.contains(QStringLiteral("filter")));
    const QJsonObject filterObj = obj[QStringLiteral("filter")].toObject();
    QVERIFY(filterObj.contains(QStringLiteral("fields")));
}


void CFilterTest::FromJson_TextFilter()
{
    QJsonObject json;
    json[QStringLiteral("text")] = QStringLiteral("hello");

    imtbase::CComplexCollectionFilter filter;
    const bool ok = imtbase::CFilterSerializer::FromJson(json, filter);
    QVERIFY(ok);
    QCOMPARE(filter.GetTextFilter(), QStringLiteral("hello"));
}


void CFilterTest::FromJson_SortFields()
{
    QJsonArray sortArr;
    QJsonObject sortItem;
    sortItem[QStringLiteral("field")] = QStringLiteral("Name");
    sortItem[QStringLiteral("order")] = QStringLiteral("asc");
    sortArr << sortItem;

    QJsonObject json;
    json[QStringLiteral("sort")] = sortArr;

    imtbase::CComplexCollectionFilter filter;
    const bool ok = imtbase::CFilterSerializer::FromJson(json, filter);
    QVERIFY(ok);

    bool found = false;
    for (const imtbase::IComplexCollectionFilter::FieldInfo& info : filter.GetFields()){
        if (info.id == "Name" && (info.metaInfo.flags & imtbase::IComplexCollectionFilter::SO_SORT)){
            QCOMPARE(info.metaInfo.sortingOrder, imtbase::IComplexCollectionFilter::SO_ASC);
            found = true;
        }
    }
    QVERIFY(found);
}


void CFilterTest::FromJson_FieldFilters()
{
    QJsonArray fieldsArr;
    QJsonObject ffObj;
    ffObj[QStringLiteral("field")] = QStringLiteral("Status");
    ffObj[QStringLiteral("op")]    = QStringLiteral("eq");
    ffObj[QStringLiteral("value")] = QStringLiteral("active");
    fieldsArr << ffObj;

    QJsonObject filterObj;
    filterObj[QStringLiteral("op")]     = QStringLiteral("and");
    filterObj[QStringLiteral("fields")] = fieldsArr;

    QJsonObject json;
    json[QStringLiteral("filter")] = filterObj;

    imtbase::CComplexCollectionFilter filter;
    const bool ok = imtbase::CFilterSerializer::FromJson(json, filter);
    QVERIFY(ok);

    const imtbase::IComplexCollectionFilter::FilterExpression& expr = filter.GetFilterExpression();
    QCOMPARE(expr.fieldFilters.size(), 1);
    QCOMPARE(expr.fieldFilters.first().fieldId, QByteArray("Status"));
    QCOMPARE(expr.fieldFilters.first().filterValue.toString(), QStringLiteral("active"));
}


void CFilterTest::FromJson_Empty()
{
    imtbase::CComplexCollectionFilter filter;
    const bool ok = imtbase::CFilterSerializer::FromJson(QJsonObject(), filter);
    QVERIFY(ok);
    QVERIFY(filter.GetTextFilter().isEmpty());
}


I_ADD_TEST(CFilterTest);


