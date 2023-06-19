#include <imtlicgql/CProductPermissionsControllerComp.h>


namespace imtlicgql
{


// public methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CProductPermissionsControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_productProviderCompPtr.IsValid()){
		return nullptr;
	}

	const QList<imtgql::CGqlObject> paramsPtr = gqlRequest.GetParams();

	QByteArray productId;
	if (!paramsPtr.empty()){
		productId = paramsPtr.at(0).GetFieldArgumentValue("ProductId").toByteArray();
	}

	if (productId.isEmpty()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* productsModelPtr = m_productProviderCompPtr->CreateResponse(gqlRequest, errorMessage);

	QString json = productsModelPtr->toJSON();
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


