#include <imtlicgql/CProductPermissionsControllerComp.h>


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtlicgql
{


// public methods

// reimplemented (imtgql::IGqlRepresentationDataController)

imtbase::CHierarchicalItemModelPtr CProductPermissionsControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	imtbase::CHierarchicalItemModelPtr rootModelPtr(new imtbase::CTreeItemModel());
//	imtbase::CTreeItemModel* permissionsModel = rootModelPtr->AddTreeModel("data");

	const QList<imtgql::CGqlObject>* params = gqlRequest.GetParams();

	QByteArray productId;
	if (!params->empty()){
		productId = params->at(0).GetFieldArgumentValue("ProductId").toByteArray();
	}

	if (productId.isEmpty()){
		return imtbase::CHierarchicalItemModelPtr();
	}

	if (m_productProviderCompPtr.IsValid()){
		imtbase::CTreeItemModel* productsModelPtr = m_productProviderCompPtr->GetRepresentation(*params, QByteArrayList());
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
	}

	return rootModelPtr;
}


} // namespace imtlicgql


