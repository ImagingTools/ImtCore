// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <imtclientgql/CGqlRemoteRepresentationControllerCompBase.h>


namespace imtclientgql
{


class CGqlRemoteRepresentationControllerComp:
			public CGqlRemoteRepresentationControllerCompBase
{
public:
	typedef CGqlRemoteRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlRemoteRepresentationControllerComp)
		I_ASSIGN(m_productIdAttrPtr, "ProductId", "Product ID", false, "");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_ATTR(QByteArray, m_productIdAttrPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
};


} // namespace imtclientgql


