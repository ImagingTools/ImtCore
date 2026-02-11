// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtlicgql
{


class CProductPermissionsControllerComp: public imtservergql::CGqlRequestHandlerCompBase
{
public:
	typedef imtservergql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductPermissionsControllerComp);
		I_ASSIGN(m_productProviderCompPtr, "ProductProvider", "Product provider", true, "ProductProvider");
	I_END_COMPONENT;

	// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtgql::IGqlRequestHandler, m_productProviderCompPtr);
};


} // namespace imtlicgql


