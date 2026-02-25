// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CUserSerializableCollectionControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iser/CMemoryWriteArchive.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtgql::CSerializableObjectCollectionControllerComp)

void CUserSerializableCollectionControllerComp::CustomProcessObject(
			const imtgql::CGqlRequest& gqlRequest,
			iser::ISerializable& object) const
{
	QByteArray productId;

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr != nullptr){
		productId = inputParamPtr->GetParamArgumentValue("productId").toByteArray();
	}

	if (productId.isEmpty()){
		productId = gqlRequest.GetHeader("productId");
	}

	auto objectInfoPtr = dynamic_cast<imtauth::IUserInfo*>(&object);
	if (objectInfoPtr != nullptr){
		if (!objectInfoPtr->IsAdmin()){
			QByteArrayList products = objectInfoPtr->GetProducts();
			for (const QByteArray& currentProduct : products){
				objectInfoPtr->SetLocalPermissions(currentProduct, objectInfoPtr->GetPermissions(currentProduct));
			}
		}
	}
}


} // namespace imtauth


