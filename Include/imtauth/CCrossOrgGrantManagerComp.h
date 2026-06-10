// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// Qt includes
#include <functional>

// ImtCore includes
#include <imtauth/ICrossOrgGrant.h>
#include <imtauth/ICrossOrgGrantData.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


class CCrossOrgGrantManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::ICrossOrgGrant
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCrossOrgGrantManagerComp);
		I_REGISTER_INTERFACE(imtauth::ICrossOrgGrant);
		I_ASSIGN(m_grantCollectionCompPtr, "GrantCollection", "Cross-org grant collection", false, "GrantCollection");
		I_ASSIGN(m_grantFactoryCompPtr, "GrantFactory", "Cross-org grant factory", false, "CrossOrgGrantInfo");
	I_END_COMPONENT;

	virtual QByteArray CreateGrant(
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId,
				const QByteArrayList& roleIds,
				const QString& description = QString(),
				const QString& expiresAt = QString()) override;
	virtual bool RevokeGrant(const QByteArray& grantId) override;
	virtual bool RemoveGrants(const QByteArrayList& grantIds) override;
	virtual CrossOrgGrantInfo GetGrant(const QByteArray& grantId) const override;
	virtual CrossOrgGrants GetGrantsBySourceTenant(const QByteArray& sourceTenantId) const override;
	virtual CrossOrgGrants GetGrantsByTargetTenant(const QByteArray& targetTenantId) const override;
	virtual bool HasAccess(
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId,
				const QByteArray& roleId) const override;

private:
	CrossOrgGrants CollectGrants(const std::function<bool(const CrossOrgGrantInfo&)>& predicate) const;
	bool IsGrantEffective(const CrossOrgGrantInfo& grantInfo) const;

private:
	I_REF(imtbase::IObjectCollection, m_grantCollectionCompPtr);
	I_FACT(imtauth::ICrossOrgGrantData, m_grantFactoryCompPtr);
};


} // namespace imtauth
