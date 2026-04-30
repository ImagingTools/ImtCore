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
    void CFilter_Search();
    void CFilter_Rules();
    void CFilter_RuleSets();
    void CFilter_FluentApi();
    void CFilter_Orders();
    void CFilter_Window();
    void CFilter_Clear();

    void CPaginatedFilter_DefaultValues();
    void CPaginatedFilter_SetPage();
    void CPaginatedFilter_SetFilter();
    void CPaginatedFilter_Offset();
    void CPaginatedFilter_TotalPages();
    void CPaginatedFilter_HasNextPage();
    void CPaginatedFilter_HasPreviousPage();

    void ToQueryString_Search();
    void ToQueryString_Window();
    void ToQueryString_Orders();
    void ToQueryString_Rules();
    void ToQueryString_GroupRules();
    void FromQueryString_Search();
    void FromQueryString_Orders();
    void FromQueryString_Rules();
    void FromQueryString_GroupRules();
    void FromQueryString_LeadingQuestionMark();

    void ToJson_Search();
    void ToJson_Orders();
    void ToJson_Rules();
    void FromJson_Search();
    void FromJson_Orders();
    void FromJson_Rules();
    void FromJson_Empty();

    void SdlConverter_FromQmlModel();
};


