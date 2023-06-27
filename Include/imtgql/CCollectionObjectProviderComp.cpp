#include <imtgql/CCollectionObjectProviderComp.h>


// ACF includes
#include <iser/CJsonMemWriteArchive.h>


namespace imtgql
{


imtbase::CTreeItemModel* CCollectionObjectProviderComp::GetObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();

	QByteArray objectId;

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr != nullptr){
		objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		iser::IObject* objectInfoPtr = dynamic_cast<iser::IObject*>(dataPtr.GetPtr());
		if (objectInfoPtr != nullptr){
			QByteArray json;
			{
				iser::CJsonMemWriteArchive archive(json);
				if (!objectInfoPtr->Serialize(archive)){
					return nullptr;
				}
			}

			if (!dataModel->CreateFromJson(json)){
				return nullptr;
			}
		}
	}

	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


} // namespace imtgql


