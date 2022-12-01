#include <imtlicgql/CProductPermissionsControllerComp.h>


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtlicgql
{


// public methods

// reimplemented (imtgql::IGqlRepresentationDataController)

imtbase::CTreeItemModel* CProductPermissionsControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* permissionsModel = rootModelPtr->AddTreeModel("data");

	const QList<imtgql::CGqlObject>* params = gqlRequest.GetParams();

	QByteArray productId;
	if (!params->empty()){
		productId = params->at(0).GetFieldArgumentValue("ProductId").toByteArray();
	}

	if (productId.isEmpty()){
		return nullptr;
	}

	if (m_productProviderCompPtr.IsValid()){
		imtbase::CTreeItemModel* productsModelPtr = m_productProviderCompPtr->GetTreeItemModel(*params, QByteArrayList());
		if (productsModelPtr != nullptr){
			for (int i = 0; i < productsModelPtr->GetItemsCount(); i++){
				QByteArray currentProductId = productsModelPtr->GetData("Id", i).toByteArray();
				if (currentProductId == productId){
					imtbase::CTreeItemModel* productPermissionsModelPtr = productsModelPtr->GetTreeItemModel("Permissions", i);
					if (productPermissionsModelPtr != nullptr){
						for (int j = 0; j < productPermissionsModelPtr->GetItemsCount(); j++){
							QByteArray featureId;
							if (productPermissionsModelPtr->ContainsKey("Id", j)){
								featureId = productPermissionsModelPtr->GetData("Id", j).toByteArray();
							}

							QString featureName;
							if (productPermissionsModelPtr->ContainsKey("Name", j)){
								featureName = productPermissionsModelPtr->GetData("Name", j).toString();
							}

							int index = permissionsModel->InsertNewItem();

							permissionsModel->SetData("Id", featureId, index);
							permissionsModel->SetData("Name", featureName, index);
						}
					}
				}
			}
		}
	}

	return rootModelPtr;
}


} // namespace imtlicgql


