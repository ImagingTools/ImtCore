// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtlic/IProductInfo.h>
#include <imtlic/CFeatureInfo.h>
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtlicgql
{


class CPermissionsProviderComp: public imtservergql::CGqlRequestHandlerCompBase
{
public:
	typedef imtservergql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CPermissionsProviderComp)
		I_ASSIGN(m_productInfoCompPtr, "FeatureContainer", "Feature container", true, "FeatureContainer");
	I_END_COMPONENT

protected:
	bool CreateRepresentationModelFromFeatureInfo(
				const imtlic::CFeatureInfo& featureInfo,
				imtbase::CTreeItemModel& representationModel,
				const QByteArray& languageId,
				QString& errorMessage) const;

	// reimplemented (imtservergql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtlic::IProductInfo, m_productInfoCompPtr);
};


} // namespace imtlicgql


