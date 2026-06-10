// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/IDocumentNameProvider.h>


namespace imtauth
{


class CTenantRelationshipDocumentNameProviderComp:
			public icomp::CComponentBase,
			virtual public imtdoc::IDocumentNameProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantRelationshipDocumentNameProviderComp)
		I_ASSIGN(m_tenantCollectionCompPtr, "TenantCollection", "Tenant collection for tenant name resolution", true, "TenantCollection");
		I_REGISTER_INTERFACE(imtdoc::IDocumentNameProvider);
	I_END_COMPONENT

	// reimplemented (imtdoc::IDocumentNameProvider)
	virtual QString GetDefaultDocumentName(const QByteArray& objectId, const istd::IChangeable& document) const override;

private:
	I_REF(imtbase::IObjectCollection, m_tenantCollectionCompPtr);
};


} // namespace imtauth
