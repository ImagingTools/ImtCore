// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtservergql/IObjectParamsFiller.h>


namespace imtauthgql
{


/**
	Provides additional parameters (partner tenant, role, status)
	for tenant relationship items displayed in FilterableSelect lists.

	\ingroup FilterableSelect
*/
class CTenantRelationshipItemInfoProviderComp:
			public ilog::CLoggerComponentBase, 
			public imtservergql::IObjectParamsFiller
{
public:
	I_BEGIN_COMPONENT(CTenantRelationshipItemInfoProviderComp);
		I_REGISTER_INTERFACE(imtservergql::IObjectParamsFiller);
		I_ASSIGN(m_relationshipCollectionCompPtr, "RelationshipCollection", "Tenant relationship collection", true, "RelationshipCollection");
		I_ASSIGN(m_tenantCollectionCompPtr, "TenantCollection", "Tenant collection for name resolution", false, "TenantCollection");
	I_END_COMPONENT;

	// reimplemented (IObjectParamsFiller)
	virtual bool FillParams(
				const QByteArray& objectId,
				iprm::IParamsSet& paramsSet,
				const QByteArray& contextTenantId = QByteArray()) const override;

private:
	I_REF(imtbase::IObjectCollection, m_relationshipCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_tenantCollectionCompPtr);
};


} // namespace imtauthgql
