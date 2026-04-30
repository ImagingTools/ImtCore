// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QByteArrayList>
#include <QtCore/QVariant>
#include <QtCore/QVector>
#include <QtCore/QString>


namespace imtbase
{


/**
 * @brief Independent filter request model.
 *
 * CFilter describes a query request as search text, predicate rules, ordering,
 * and a result window.  Predicate names are stored as strings on purpose so the
 * model can grow without mirroring legacy enum-based filter interfaces.
 */
class CFilter
{
public:
    struct Search
    {
        Search(const QString& text = QString(), const QByteArrayList& scopes = QByteArrayList());

        bool IsActive() const;
        bool operator==(const Search& other) const;
        bool operator!=(const Search& other) const;

        QString text;
        QByteArrayList scopes;
    };

    struct Rule
    {
        Rule(
            const QByteArray& path = QByteArray(),
            const QString& predicate = QString(),
            const QVariant& argument = QVariant());

        bool IsValid() const;
        bool operator==(const Rule& other) const;
        bool operator!=(const Rule& other) const;

        QByteArray path;
        QString predicate;
        QVariant argument;
    };

    struct RuleSet
    {
        enum Join
        {
            All,
            Any
        };

        explicit RuleSet(Join join = All);

        bool IsEmpty() const;
        bool operator==(const RuleSet& other) const;
        bool operator!=(const RuleSet& other) const;

        Join join = All;
        QVector<Rule> rules;
        QVector<RuleSet> children;
    };

    struct Order
    {
        Order(const QByteArray& path = QByteArray(), bool descending = false);

        bool IsValid() const;
        bool operator==(const Order& other) const;
        bool operator!=(const Order& other) const;

        QByteArray path;
        bool descending = false;
    };

    struct Window
    {
        Window(int first = -1, int count = -1);

        bool IsActive() const;
        bool operator==(const Window& other) const;
        bool operator!=(const Window& other) const;

        int first = -1;
        int count = -1;
    };

    const Search& GetSearch() const;
    void SetSearch(const Search& search);
    void SetSearch(const QString& text, const QByteArrayList& scopes = QByteArrayList());

    const RuleSet& GetRules() const;
    void SetRules(const RuleSet& rules);
    void AddRule(const Rule& rule);
    void AddRuleSet(const RuleSet& rules);

    QVector<Order> GetOrders() const;
    void SetOrders(const QVector<Order>& orders);
    void AddOrder(const Order& order);

    Window GetWindow() const;
    void SetWindow(const Window& window);
    void SetWindow(int first, int count);
    void ClearWindow();

    int GetOffset() const;
    int GetLimit() const;
    bool HasWindow() const;

    void Clear();

private:
    Search m_search;
    RuleSet m_rules;
    QVector<Order> m_orders;
    Window m_window;
};


} // namespace imtbase


