// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlic/CProductLicensingInfo.h>
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtlicgql
{


class CProductInfoProviderComp: public imtservergql::CGqlRequestHandlerCompBase
{
public:
	typedef imtservergql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductInfoProviderComp)
		I_ASSIGN_MULTI_0(m_productIdsAttrPtr, "ProductIds", "List of product-IDs for data provider", true);
		I_ASSIGN_MULTI_0(m_productNamesAttrPtr, "ProductNames", "List of product-Names for data provider", true);
		I_ASSIGN_MULTI_0(m_permissionsProviderCompPtr, "PermissionsProvider", "Permissins provideor", false);
	I_END_COMPONENT

protected:
	// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_MULTIATTR(QByteArray, m_productIdsAttrPtr);
	I_MULTIATTR(QString, m_productNamesAttrPtr);
	I_MULTIREF(imtgql::IGqlRequestHandler, m_permissionsProviderCompPtr);
};


} // namespace imtlicgql


