// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantAwareDelegateCompBase.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtdb/imtdb.h>


namespace imtdb
{


QString CTenantAwareDelegateCompBase::CreateAdditionalFiltersQuery(
            const iprm::IParamsSet& filterParams) const
{
    if (m_tenantFilterStrategyCompPtr == nullptr){
        return QString();
    }

    QByteArray paramKey = *m_tenantParamKeyAttrPtr;
    if (paramKey.isEmpty()){
        paramKey = "TenantId";
    }

    iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();

    if (!paramIds.contains(paramKey)){
        // No TenantId in context — apply global scope filter
        return m_tenantFilterStrategyCompPtr->CreateGlobalScopeFilterSql(QStringLiteral("root"));
    }

    iprm::TParamsPtr<iprm::IIdParam> tenantIdParamPtr(&filterParams, paramKey);
    if (!tenantIdParamPtr.IsValid()){
        return m_tenantFilterStrategyCompPtr->CreateGlobalScopeFilterSql(QStringLiteral("root"));
    }

    QByteArray tenantId = tenantIdParamPtr->GetId();
    if (tenantId.isEmpty()){
        return m_tenantFilterStrategyCompPtr->CreateGlobalScopeFilterSql(QStringLiteral("root"));
    }

    QString tenantIdStr = QString::fromUtf8(tenantId);
    return m_tenantFilterStrategyCompPtr->CreateTenantFilterSql(tenantIdStr, QStringLiteral("root"));
}


} // namespace imtdb


