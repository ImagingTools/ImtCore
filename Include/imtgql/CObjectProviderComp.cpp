#include <imtgql/CObjectProviderComp.h>


// ACF includes
#include <iser/CJsonMemWriteArchive.h>


namespace imtgql
{


// reimplemented (imtgql::CCGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* CObjectProviderComp::CreateInternalResponse(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	if (!m_objectCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error. Object is invalid").toUtf8();

		return nullptr;
	}

	imtbase::CTreeItemModel* dataModelPtr = new imtbase::CTreeItemModel();

	QByteArray json;
	{
		iser::CJsonMemWriteArchive archive(json);
		if (!m_objectCompPtr->Serialize(archive)){
			errorMessage = QObject::tr("Failed to perform serialization to the archive from the object").toUtf8();

			return nullptr;
		}
	}

	if (!dataModelPtr->CreateFromJson(json)){
		errorMessage = QObject::tr("Failed to convert to a tree model from json").toUtf8();

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	rootModelPtr->SetExternTreeModel("data", dataModelPtr);

	return rootModelPtr.PopPtr();
}


} // namespace imtgql


