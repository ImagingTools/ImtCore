#include <imtlicgql/CProductPermissionsControllerComp.h>


namespace imtlicgql
{


// public methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CProductPermissionsControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_productProviderCompPtr.IsValid()){
		SendErrorMessage(0, QString("Internal error."), "CProductPermissionsControllerComp");

		return nullptr;
	}

	const QList<imtgql::CGqlObject> paramsPtr = gqlRequest.GetParams();

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		SendErrorMessage(0, QString("Unable to create object. GQL input params is invalid."), "CProductPermissionsControllerComp");

		return nullptr;
	}

	QByteArray productId = inputParamPtr->GetFieldArgumentValue("ProductId").toByteArray();
	if (productId.isEmpty()){
		SendErrorMessage(0, QString("Unable to get permission for product with empty ID."), "CProductPermissionsControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* productsModelPtr = m_productProviderCompPtr->CreateResponse(gqlRequest, errorMessage);
	if (productsModelPtr != nullptr){
		for (int i = 0; i < productsModelPtr->GetItemsCount(); i++){
			QByteArray currentProductId = productsModelPtr->GetData("Id", i).toByteArray();
			if (currentProductId == productId){
				imtbase::CTreeItemModel* productPermissionsModelPtr = productsModelPtr->GetTreeItemModel("Permissions", i);
				if (productPermissionsModelPtr != nullptr){
					rootModelPtr->SetExternTreeModel("data", productPermissionsModelPtr);
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtlicgql


