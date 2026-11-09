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


bool CClientProductCatalogComp::AddClientProduct(const QByteArray& productId, const QString& /*name*/, QString& errorMessage, bool* isAddedPtr)
{
	if (isAddedPtr != nullptr){
		*isAddedPtr = false;
	}

	if (IsClientProduct(productId)){
		return true;
	}

	errorMessage = QStringLiteral("The clients of this server are set in its configuration, '%1' is not one of them").arg(QString::fromUtf8(productId));

	return false;
}


} // namespace imtauth


