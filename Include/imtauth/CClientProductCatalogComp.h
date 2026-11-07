// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IClientProductCatalog.h>


namespace imtauth
{


/**
	Client products named in the configuration of the server.
*/
class CClientProductCatalogComp:
			public icomp::CComponentBase,
			virtual public IClientProductCatalog
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CClientProductCatalogComp)
		I_REGISTER_INTERFACE(IClientProductCatalog);
		I_ASSIGN_MULTI_0(m_productIdsAttrPtr, "ProductIds", "Products whose servers may be given a token of this server", true);
	I_END_COMPONENT

	// reimplemented (IClientProductCatalog)
	virtual bool IsClientProduct(const QByteArray& productId, bool* isReadPtr = nullptr) const override;

private:
	I_MULTIATTR(QByteArray, m_productIdsAttrPtr);
};


} // namespace imtauth


