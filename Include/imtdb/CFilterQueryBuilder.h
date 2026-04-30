// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QVariantList>

// ImtCore includes
#include <imtbase/CFilter.h>


namespace imtdb
{


/**
 * @brief SQL query builder for standalone imtbase::CFilter.
 *
 * This builder is not integrated into database delegates.  It provides a clean
 * reusable translation layer from the standalone filter model to SQL clauses.
 */
class CFilterQueryBuilder
{
public:
    enum DatabaseDialect
    {
        DD_POSTGRESQL,
        DD_SQLITE
    };

    struct QueryResult
    {
        QString whereClause;
        QString orderByClause;
        QString limitClause;
        QVariantList bindValues;
    };

    explicit CFilterQueryBuilder(DatabaseDialect dialect = DD_POSTGRESQL);

    QueryResult Build(const imtbase::CFilter& filter) const;

    QString BuildTextFilter(
        const QString& text,
        const QByteArrayList& columnIds,
        QVariantList& bindValues) const;

    QString BuildFilterExpression(
        const imtbase::CFilter::FilterExpression& expression,
        QVariantList& bindValues) const;

    QString BuildSortClause(const QVector<imtbase::CFilter::SortField>& sortFields) const;
    QString BuildLimitClause(const imtbase::CFilter& filter) const;

    void SetJsonColumnName(const QString& columnName);
    void SetUseJsonExtract(bool useJsonExtract);

private:
    QString MakeLikeCondition(const QString& expression, const QString& placeholder) const;
    QString MakeJsonFieldAccess(const QString& fieldId) const;
    QString MakePlaceholder(int bindIndex) const;
    QString AddBindValue(const QVariant& value, QVariantList& bindValues) const;
    QString AddLikeBindValue(const QString& value, QVariantList& bindValues) const;
    QString MakeFieldAccess(const QByteArray& fieldId) const;
    QString EscapeLikePattern(const QString& text) const;
    static QString QuoteIdentifier(const QString& identifier);
    static QString EscapeJsonKey(const QString& key);

private:
    DatabaseDialect m_dialect;
    QString m_jsonColumnName;
    bool m_useJsonExtract = false;
};


} // namespace imtdb


