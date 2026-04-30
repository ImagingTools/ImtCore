// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CFilterTest: public QObject
{
    Q_OBJECT

private slots:
    void CFilter_TextAndFields();
    void CFilter_FieldExpression();
    void CFilter_SortFields();
    void CFilter_Pagination();
    void CFilter_Clear();

    void CPaginatedFilter_DefaultValues();
    void CPaginatedFilter_SetPage();
    void CPaginatedFilter_SetFilter();
    void CPaginatedFilter_Offset();
    void CPaginatedFilter_TotalPages();
    void CPaginatedFilter_HasNextPage();
    void CPaginatedFilter_HasPreviousPage();

    void ToQueryString_TextFilter();
    void ToQueryString_WithPagination();
    void ToQueryString_SortFields();
    void ToQueryString_FieldFilters();
    void FromQueryString_TextFilter();
    void FromQueryString_SortFields();
    void FromQueryString_FieldFilters();
    void FromQueryString_LeadingQuestionMark();

    void ToJson_TextFilter();
    void ToJson_SortFields();
    void ToJson_FieldFilters();
    void FromJson_TextFilter();
    void FromJson_SortFields();
    void FromJson_FieldFilters();
    void FromJson_Empty();
};


