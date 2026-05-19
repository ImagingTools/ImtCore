// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/CFilter.h>


namespace imtbase
{


/**
 * @brief Fluent builder for standalone CFilter requests.
 */
class CFilterBuilder
{
public:
    CFilterBuilder& search(const QString& text, const QByteArrayList& scopes = QByteArrayList());
    CFilterBuilder& searchText(const QString& text, const QByteArrayList& scopes = QByteArrayList());
    CFilterBuilder& where(const QByteArray& path, const QString& predicate, const QVariant& argument = QVariant());
    CFilterBuilder& beginGroup(CFilter::RuleSet::Join join);
    CFilterBuilder& beginAnd();
    CFilterBuilder& beginOr();
    CFilterBuilder& endGroup();
    CFilterBuilder& all(const CFilter::RuleSet& rules);
    CFilterBuilder& any(const CFilter::RuleSet& rules);
    CFilterBuilder& orderBy(const QByteArray& path, bool descending = false);
    CFilterBuilder& page(int pageNumber, int pageSize);
    CFilterBuilder& window(int first, int count);
    CFilterBuilder& windowed(int first, int count);
    CFilter build() const;

private:
    CFilter::RuleSet& CurrentRuleSet();

    CFilter m_filter;
    CFilter::RuleSet m_rules;
    QVector<int> m_groupPath;
};


} // namespace imtbase
