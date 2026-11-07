// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CConnectedClientProductCatalogComp.h>


// ImtCore includes
#include <imtauth/CConnectedProductDocumentValidatorComp.h>


namespace imtauth
{


// public methods

// reimplemented (IClientProductCatalog)

bool CConnectedClientProductCatalogComp::IsClientProduct(const QByteArray& productId, bool* isReadPtr) const
{
	if (!m_productCollectionCompPtr.IsValid()){
		if (isReadPtr != nullptr){
			*isReadPtr = false;
		}

		return false;
	}

	return CConnectedProductDocumentValidatorComp::HasProduct(*m_productCollectionCompPtr, productId, isReadPtr);
}


} // namespace imtauth


