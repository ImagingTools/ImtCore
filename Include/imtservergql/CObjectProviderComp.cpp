#include <imtservergql/CObjectProviderComp.h>


// ACF includes
#include <iser/CJsonMemWriteArchive.h>


namespace imtservergql
{


// reimplemented (imtgql::CCGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* CObjectProviderComp::CreateInternalResponse(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	if (!m_objectCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'Object' was not set", "CObjectProviderComp");
		return nullptr;
	}

	QByteArray json;
	{
		iser::CJsonMemWriteArchive archive;
		if (!m_objectCompPtr->Serialize(archive)){
			errorMessage = QString("Failed to perform serialization to the archive from the object").toUtf8();

			return nullptr;
		}

		json = archive.GetData();
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	if (!dataModelPtr->CreateFromJson(json)){
		errorMessage = QString("Failed to convert to a tree model from json").toUtf8();

		return nullptr;
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtservergql


