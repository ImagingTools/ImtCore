#include <imtauthgql/CCollectionUserInfoProviderComp.h>


// ACF includes
#include <iser/CJsonMemWriteArchive.h>


namespace imtauthgql
{


// reimplemented (imtgql::CCGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* CCollectionUserInfoProviderComp::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_userCollectionCompPtr.IsValid()){
		errorMessage = QString("m_userCollectionCompPtr invalid").toUtf8();
		SendErrorMessage(0, errorMessage, "CCollectionUserInfoProviderComp");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	QByteArray objectId;
	QByteArray productId;

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr != nullptr){
		objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
		productId = inputParamPtr->GetFieldArgumentValue("ProductId").toByteArray();
	}

	if (productId.isEmpty()){
		productId = gqlRequest.GetHeader("ProductId");
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		imtauth::IUserInfo* objectInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
		if (objectInfoPtr == nullptr){
			errorMessage = QString("Object conversing error").toUtf8();
			SendErrorMessage(0, errorMessage, "CCollectionUserInfoProviderComp");

			return nullptr;
		}

		if (!objectInfoPtr->IsAdmin()){
			QByteArrayList productPermissions = objectInfoPtr->GetPermissions(productId);

			QByteArrayList products = objectInfoPtr->GetProducts();
			for (const QByteArray& currentProduct : products){
				if (currentProduct != productId){
					objectInfoPtr->RemoveProduct(currentProduct);
				}
			}

			objectInfoPtr->SetLocalPermissions(productId, productPermissions);
		}

		QByteArray json;
		{
			iser::CJsonMemWriteArchive archive(m_versionInfoCompPtr.GetPtr());
			if (!objectInfoPtr->Serialize(archive)){
				errorMessage = QString("Error when serializing an object").toUtf8();
				SendErrorMessage(0, errorMessage, "CCollectionUserInfoProviderComp");

				return nullptr;
			}

			json = archive.GetData();
		}

		if (!dataModelPtr->CreateFromJson(json)){
			errorMessage = QString("Error when creating a tree model from json").toUtf8();
			SendErrorMessage(0, errorMessage);

			return nullptr;
		}
	}
	else{
		errorMessage = QString("Object with ID %1 not exists").arg(qPrintable(objectId)).toUtf8();
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtauthgql


