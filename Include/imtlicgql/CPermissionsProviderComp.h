// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtlic/IProductInfo.h>
#include <imtlic/CFeatureInfo.h>
#include <imtauth/ITenantManager.h>
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtlicgql
{


class CPermissionsProviderComp: public imtservergql::CGqlRequestHandlerCompBase
{
public:
	typedef imtservergql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CPermissionsProviderComp)
		I_ASSIGN(m_productInfoCompPtr, "FeatureContainer", "Feature container", true, "FeatureContainer");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager for filtering permissions by tenant scope", false, "TenantManager");
	I_END_COMPONENT

protected:
	bool CreateRepresentationModelFromFeatureInfo(
				const imtlic::CFeatureInfo& featureInfo,
				imtbase::CTreeItemModel& representationModel,
				const QByteArray& languageId,
				const QSet<QByteArray>* allowedPermissionsPtr,
				QString& errorMessage) const;

	// reimplemented (imtservergql::CGqlRequestHandlerCompBase)
	virtual QJsonObject CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtlic::IProductInfo, m_productInfoCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
};


} // namespace imtlicgql


