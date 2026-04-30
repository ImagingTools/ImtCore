// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CFilter.h>


// Qt includes
#include <QtCore/QDebug>

// STD includes
#include <limits>


namespace imtbase
{


CFilter::Search::Search(const QString& text, const QByteArrayList& scopes)
    : text(text)
    , scopes(scopes)
{
}


bool CFilter::Search::IsActive() const
{
    return !text.isEmpty() && !scopes.isEmpty();
}


bool CFilter::Search::operator==(const Search& other) const
{
    return text == other.text && scopes == other.scopes;
}


bool CFilter::Search::operator!=(const Search& other) const
{
    return !(*this == other);
}


CFilter::Rule::Rule(const QByteArray& path, const QString& predicate, const QVariant& argument)
    : path(path)
    , predicate(predicate)
    , argument(argument)
{
}


bool CFilter::Rule::IsValid() const
{
    return !path.isEmpty() && !predicate.isEmpty();
}


bool CFilter::Rule::operator==(const Rule& other) const
{
    return path == other.path &&
        predicate == other.predicate &&
        argument == other.argument;
}


bool CFilter::Rule::operator!=(const Rule& other) const
{
    return !(*this == other);
}


CFilter::RuleSet::RuleSet(Join join)
    : join(join)
{
}


bool CFilter::RuleSet::IsEmpty() const
{
    return rules.isEmpty() && children.isEmpty();
}


bool CFilter::RuleSet::operator==(const RuleSet& other) const
{
    return join == other.join &&
        rules == other.rules &&
        children == other.children;
}


bool CFilter::RuleSet::operator!=(const RuleSet& other) const
{
    return !(*this == other);
}


CFilter::Order::Order(const QByteArray& path, bool descending)
    : path(path)
    , descending(descending)
{
}


bool CFilter::Order::IsValid() const
{
    return !path.isEmpty();
}


bool CFilter::Order::operator==(const Order& other) const
{
    return path == other.path && descending == other.descending;
}


bool CFilter::Order::operator!=(const Order& other) const
{
    return !(*this == other);
}


CFilter::Window::Window(int first, int count)
    : first(first >= 0 ? first : -1)
    , count(count > 0 ? count : -1)
{
}


bool CFilter::Window::IsActive() const
{
    return first >= 0 && count > 0;
}


bool CFilter::Window::operator==(const Window& other) const
{
    return first == other.first && count == other.count;
}


bool CFilter::Window::operator!=(const Window& other) const
{
    return !(*this == other);
}


CFilter::RuleSet CFilter::AllOf(const QVector<Rule>& rules, const QVector<RuleSet>& children)
{
    RuleSet ruleSet(RuleSet::All);
    ruleSet.rules = rules;
    ruleSet.children = children;
    return ruleSet;
}


CFilter::RuleSet CFilter::AnyOf(const QVector<Rule>& rules, const QVector<RuleSet>& children)
{
    RuleSet ruleSet(RuleSet::Any);
    ruleSet.rules = rules;
    ruleSet.children = children;
    return ruleSet;
}


CFilter& CFilter::search(const QString& text, const QByteArrayList& scopes)
{
    SetSearch(text, scopes);
    return *this;
}


CFilter& CFilter::searchText(const QString& text, const QByteArrayList& scopes)
{
    return search(text, scopes);
}


CFilter& CFilter::where(const QByteArray& path, const QString& predicate, const QVariant& argument)
{
    CurrentRuleSet().rules << Rule(path, predicate, argument);
    return *this;
}


CFilter& CFilter::beginGroup(RuleSet::Join join)
{
    RuleSet& current = CurrentRuleSet();
    current.children << RuleSet(join);
    m_groupPath << (current.children.size() - 1);
    return *this;
}


CFilter& CFilter::beginAnd()
{
    return beginGroup(RuleSet::All);
}


CFilter& CFilter::beginOr()
{
    return beginGroup(RuleSet::Any);
}


CFilter& CFilter::endGroup()
{
    if (!m_groupPath.isEmpty()){
        m_groupPath.removeLast();
    }
    return *this;
}


CFilter& CFilter::all(const RuleSet& rules)
{
    RuleSet allRules = rules;
    allRules.join = RuleSet::All;
    AddRuleSet(allRules);
    return *this;
}


CFilter& CFilter::any(const RuleSet& rules)
{
    RuleSet anyRules = rules;
    anyRules.join = RuleSet::Any;
    AddRuleSet(anyRules);
    return *this;
}


CFilter& CFilter::orderBy(const QByteArray& path, bool descending)
{
    AddOrder(Order(path, descending));
    return *this;
}


CFilter& CFilter::page(int pageNumber, int pageSize)
{
    if (pageNumber <= 0){
        ClearWindow();
        return *this;
    }
    if (pageSize <= 0){
        ClearWindow();
        return *this;
    }

    const int zeroBasedPage = pageNumber - 1;
    // Guard the offset multiplication below from undefined signed integer overflow.
    if (zeroBasedPage > std::numeric_limits<int>::max() / pageSize){
        ClearWindow();
        return *this;
    }

    SetWindow(zeroBasedPage * pageSize, pageSize);
    return *this;
}


CFilter& CFilter::window(int first, int count)
{
    SetWindow(first, count);
    return *this;
}


CFilter& CFilter::windowed(int first, int count)
{
    return window(first, count);
}


const CFilter& CFilter::build() const
{
    return *this;
}


const CFilter::Search& CFilter::GetSearch() const
{
    return m_search;
}


void CFilter::SetSearch(const Search& search)
{
    m_search = search;
}


void CFilter::SetSearch(const QString& text, const QByteArrayList& scopes)
{
    m_search = Search(text, scopes);
}


const CFilter::RuleSet& CFilter::GetRules() const
{
    return m_rules;
}


void CFilter::SetRules(const RuleSet& rules)
{
    m_rules = rules;
    m_groupPath.clear();
}


void CFilter::AddRule(const Rule& rule)
{
    m_rules.rules << rule;
}


void CFilter::AddRuleSet(const RuleSet& rules)
{
    m_rules.children << rules;
}


QVector<CFilter::Order> CFilter::GetOrders() const
{
    return m_orders;
}


void CFilter::SetOrders(const QVector<Order>& orders)
{
    m_orders = orders;
}


void CFilter::AddOrder(const Order& order)
{
    m_orders << order;
}


CFilter::Window CFilter::GetWindow() const
{
    return m_window;
}


void CFilter::SetWindow(const Window& window)
{
    m_window = window;
}


void CFilter::SetWindow(int first, int count)
{
    m_window = Window(first, count);
}


void CFilter::ClearWindow()
{
    m_window = Window();
}


int CFilter::GetOffset() const
{
    return m_window.IsActive() ? m_window.first : -1;
}


int CFilter::GetLimit() const
{
    return m_window.IsActive() ? m_window.count : -1;
}


bool CFilter::HasWindow() const
{
    return m_window.IsActive();
}


void CFilter::Clear()
{
    m_search = Search();
    m_rules = RuleSet();
    m_orders.clear();
    m_window = Window();
    m_groupPath.clear();
}


CFilter::RuleSet& CFilter::CurrentRuleSet()
{
    RuleSet* current = &m_rules;
    for (const int index : m_groupPath){
        if (index < 0 || index >= current->children.size()){
            qWarning() << "Invalid CFilter group path, resetting fluent group stack";
            m_groupPath.clear();
            return m_rules;
        }
        current = &current->children[index];
    }
    return *current;
}


} // namespace imtbase
