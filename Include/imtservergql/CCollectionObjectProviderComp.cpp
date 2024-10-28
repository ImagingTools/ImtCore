#include <imtservergql/CCollectionObjectProviderComp.h>


// ACF includes
#include <iser/CJsonMemWriteArchive.h>


namespace imtservergql
{


// reimplemented (imtgql::CCGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* CCollectionObjectProviderComp::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Internal error").toUtf8();
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	QByteArray objectId;

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr != nullptr){
		objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		iser::IObject* objectInfoPtr = dynamic_cast<iser::IObject*>(dataPtr.GetPtr());
		Q_ASSERT_X(objectInfoPtr != nullptr, __FILE__, "iser::IObject interface not supported");
		if (objectInfoPtr == nullptr){
			errorMessage = QString("INTERNAL: Object implementation doesn't support iser::IObject interface").toUtf8();

			SendErrorMessage(0, errorMessage);

			return nullptr;
		}

		QByteArray json;
		{
			iser::CJsonMemWriteArchive archive(json);
			if (!objectInfoPtr->Serialize(archive)){
				errorMessage = QString("Object serialization failed").toUtf8();

				SendErrorMessage(0, errorMessage);

				return nullptr;
			}
		}

		if (!dataModelPtr->CreateFromJson(json)){
			errorMessage = QString("Tree item model creation from JSON failed").toUtf8();

			SendErrorMessage(0, errorMessage);

			return nullptr;
		}
	}
	else{
		errorMessage = QString("Object with the ID '%1' doesn't exist").arg(qPrintable(objectId)).toUtf8();

		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtservergql


