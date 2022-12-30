#include <imtlicgql/CProductPermissionsControllerComp.h>


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtlicgql
{


// public methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CProductPermissionsControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	const QList<imtgql::CGqlObject>* params = gqlRequest.GetParams();

	QByteArray productId;
	if (!params->empty()){
		productId = params->at(0).GetFieldArgumentValue("ProductId").toByteArray();
	}

	if (productId.isEmpty()){
		return nullptr;
	}

	if (m_productProviderCompPtr.IsValid()){
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
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtlicgql


