#include <imtgql/CCollectionObjectProviderComp.h>


// ACF includes
#include <iser/CJsonMemWriteArchive.h>


namespace imtgql
{


// reimplemented (imtgql::CCGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* CCollectionObjectProviderComp::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	QByteArray objectId;

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr != nullptr){
		objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		iser::IObject* objectInfoPtr = dynamic_cast<iser::IObject*>(dataPtr.GetPtr());
		Q_ASSERT_X(objectInfoPtr != nullptr, __FILE__, "iser::IObject interface not supported");
		if (objectInfoPtr == nullptr){
			errorMessage = QObject::tr("INTERNAL: Object implementation doesn't support iser::IObject interface").toUtf8();

			SendErrorMessage(0, errorMessage);

			return nullptr;
		}

		QByteArray json;
		{
			iser::CJsonMemWriteArchive archive(json);
			if (!objectInfoPtr->Serialize(archive)){
				errorMessage = QObject::tr("Object serialization failed").toUtf8();

				SendErrorMessage(0, errorMessage);

				return nullptr;
			}
		}

		if (!dataModelPtr->CreateFromJson(json)){
			errorMessage = QObject::tr("Tree item model creation from JSON failed").toUtf8();

			SendErrorMessage(0, errorMessage);

			return nullptr;
		}
	}
	else{
		errorMessage = QObject::tr("Object with the ID '%1' doesn't exist").arg(qPrintable(objectId)).toUtf8();

		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtgql


