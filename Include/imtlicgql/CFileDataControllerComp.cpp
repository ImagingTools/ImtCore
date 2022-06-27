// ImtCore includes
#include <imtlicgql/CFileDataControllerComp.h>


namespace imtlicgql
{


// public methods

// reimplemented (imtgql::IGqlRepresentationDataController)

imtbase::CTreeItemModel* CFileDataControllerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_modelIdsCompPtr.FindValue(gqlRequest.GetCommandId()) == -1 || !m_fileProviderCompPtr.IsValid()){
		return nullptr;
	}

	const QList<imtgql::CGqlObject>* fieldList = gqlRequest.GetFields();
	const QList<imtgql::CGqlObject>* paramList = gqlRequest.GetParams();

	int count = fieldList->count();
	QByteArrayList fields;

	QByteArray fileName;

	for (int i = 0; i < count; i++){
		if (fieldList->at(i).GetId() == "file"){
			fields = fieldList->at(i).GetFieldIds();

			if (paramList != nullptr && !paramList->isEmpty()){
				fileName = paramList->at(0).GetFieldArgumentValue("name").toByteArray();
			}
		}
	}

	if (m_fileNameAttrPtr.IsValid()){
		fileName = (*m_fileNameAttrPtr).toUtf8();
	}

	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModelPtr = rootModelPtr->AddTreeModel("errors");
		Q_ASSERT(errorsItemModelPtr != nullptr);

		errorsItemModelPtr->SetData("message", errorMessage);
	}
	else{
		imtbase::CTreeItemModel* dataModelPtr = new imtbase::CTreeItemModel();
		QByteArray data;

		if (!m_fileProviderCompPtr->LoadData(data, fileName)){
			SendErrorMessage(0, "Error downloading file ", "Server file data controller");
		}

		dataModelPtr->SetData("FileData", data);
		rootModelPtr->SetExternTreeModel("data", dataModelPtr);
	}

	return rootModelPtr;
}


} // namespace imtlicgql


