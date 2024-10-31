#include <imtauthgql/CUserSerializableCollectionControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CParamsSet.h>
#include <iser/CMemoryWriteArchive.h>
#include <iser/CMemoryReadArchive.h>

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

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr != nullptr){
		productId = inputParamPtr->GetFieldArgumentValue("ProductId").toByteArray();
	}

	if (productId.isEmpty()){
		productId = gqlRequest.GetHeader("ProductId");
	}

	imtauth::IUserInfo* objectInfoPtr = dynamic_cast<imtauth::IUserInfo*>(&object);
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


