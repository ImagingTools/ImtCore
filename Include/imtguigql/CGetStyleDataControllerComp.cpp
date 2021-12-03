#include "CGetStyleDataControllerComp.h"

// Qt includes
#include <QtCore/QObject>

// ImtCore includes
#include <imtqml/CPageDataEnumProviderComp.h>


namespace imtguigql
{


//// reimplemented (imtgql::IGqlRepresentationDataController)


imtbase::CTreeItemModel* CGetStyleDataControllerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{

	if (gqlRequest.GetCommandId() != *m_modelIdCompPtr){
		return nullptr;
	}

	const QList<imtgql::CGqlObject>* fieldList = gqlRequest.GetFields();
	const QList<imtgql::CGqlObject>* paramList = gqlRequest.GetParams();

	int count = fieldList->count();

	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* sourceModel = nullptr;
	bool isSetResponce = false;
	QByteArrayList fields;
	QByteArray theme;

	for (int i = 0; i < count; i++){
		if (fieldList->at(i).GetId() == "style"){
			fields = fieldList->at(i).GetFieldIds();
			if (paramList != nullptr && !paramList->isEmpty()){
				theme = paramList->at(0).GetFieldArgumentValue("theme").toByteArray();
			}
			isSetResponce = true;
		}
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{

		dataModel = new imtbase::CTreeItemModel();
		dataModel->SetData("theme",theme);
	//	dataModel->SetData("theme",theme);
		QString pathToTheme;
		if (theme == "Dark") {
			pathToTheme = ":/dark.theme";
		} else {
			pathToTheme = ":/light.theme";
		}
		QFile resource(pathToTheme);
		if (resource.open(QIODevice::ReadOnly)) {
			QByteArray resources = resource.readAll();
			sourceModel = new imtbase::CTreeItemModel();
			sourceModel->Parse(resources);
			dataModel->SetExternTreeModel("source", sourceModel);
		}

	}
	rootModel->SetExternTreeModel("data", dataModel);
	return rootModel;
}


} // namespace imtgql


