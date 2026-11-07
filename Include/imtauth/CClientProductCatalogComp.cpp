// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CClientProductCatalogComp.h>


namespace imtauth
{


// public methods

// reimplemented (IClientProductCatalog)

bool CClientProductCatalogComp::IsClientProduct(const QByteArray& productId, bool* isReadPtr) const
{
	if (isReadPtr != nullptr){
		*isReadPtr = true;
	}

	if (productId.isEmpty()){
		return false;
	}

	for (int i = 0; i < m_productIdsAttrPtr.GetCount(); i++){
		if (m_productIdsAttrPtr[i] == productId){
			return true;
		}
	}

	return false;
}


} // namespace imtauth


