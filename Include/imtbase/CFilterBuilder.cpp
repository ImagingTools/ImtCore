// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CFilterBuilder.h>


// Qt includes
#include <QtCore/QDebug>

// STD includes
#include <limits>


namespace imtbase
{


CFilterBuilder& CFilterBuilder::searchText(const QString& text, const QByteArrayList& scopes)
{
    return search(text, scopes);
}


CFilterBuilder& CFilterBuilder::search(const QString& text, const QByteArrayList& scopes)
{
    m_filter.SetSearch(text, scopes);
    return *this;
}


CFilterBuilder& CFilterBuilder::where(const QByteArray& path, const QString& predicate, const QVariant& argument)
{
    CurrentRuleSet().rules << CFilter::Rule(path, predicate, argument);
    return *this;
}


CFilterBuilder& CFilterBuilder::beginGroup(CFilter::RuleSet::Join join)
{
    CFilter::RuleSet& current = CurrentRuleSet();
    current.children << CFilter::RuleSet(join);
    m_groupPath << (current.children.size() - 1);
    return *this;
}


CFilterBuilder& CFilterBuilder::beginAnd()
{
    return beginGroup(CFilter::RuleSet::All);
}


CFilterBuilder& CFilterBuilder::beginOr()
{
    return beginGroup(CFilter::RuleSet::Any);
}


CFilterBuilder& CFilterBuilder::endGroup()
{
    if (!m_groupPath.isEmpty()){
        m_groupPath.removeLast();
    }
    return *this;
}


CFilterBuilder& CFilterBuilder::all(const CFilter::RuleSet& rules)
{
    CFilter::RuleSet allRules = rules;
    allRules.join = CFilter::RuleSet::All;
    CurrentRuleSet().children << allRules;
    return *this;
}


CFilterBuilder& CFilterBuilder::any(const CFilter::RuleSet& rules)
{
    CFilter::RuleSet anyRules = rules;
    anyRules.join = CFilter::RuleSet::Any;
    CurrentRuleSet().children << anyRules;
    return *this;
}


CFilterBuilder& CFilterBuilder::orderBy(const QByteArray& path, bool descending)
{
    m_filter.AddOrder(CFilter::Order(path, descending));
    return *this;
}


CFilterBuilder& CFilterBuilder::page(int pageNumber, int pageSize)
{
    if (pageNumber <= 0 || pageSize <= 0){
        m_filter.ClearWindow();
        return *this;
    }

    const int zeroBasedPage = pageNumber - 1;
    // Guard the offset multiplication below from undefined signed integer overflow.
    if (zeroBasedPage > std::numeric_limits<int>::max() / pageSize){
        m_filter.ClearWindow();
        return *this;
    }

    m_filter.SetWindow(zeroBasedPage * pageSize, pageSize);
    return *this;
}


CFilterBuilder& CFilterBuilder::windowed(int first, int count)
{
    return window(first, count);
}


CFilterBuilder& CFilterBuilder::window(int first, int count)
{
    m_filter.SetWindow(first, count);
    return *this;
}


CFilter CFilterBuilder::build() const
{
    CFilter filter = m_filter;
    filter.SetRules(m_rules);
    return filter;
}


CFilter::RuleSet& CFilterBuilder::CurrentRuleSet()
{
    CFilter::RuleSet* current = &m_rules;
    for (const int index : m_groupPath){
        if (index < 0 || index >= current->children.size()){
            qWarning() << "Invalid CFilterBuilder group stack while adding to the current group; use beginGroup()/beginAnd()/beginOr() and endGroup() in balanced order";
            m_groupPath.clear();
            return m_rules;
        }
        current = &current->children[index];
    }
    return *current;
}


} // namespace imtbase
