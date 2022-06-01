#include <imtguigql/CGetStyleDataControllerComp.h>

// ACF includes
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>

// Qt includes
#include <QtCore/QObject>

// ImtCore includes
#include <imtqml/CPageDataEnumProviderComp.h>


namespace imtguigql
{


// public methods

// reimplemented (imtgql::IGqlRepresentationDataController)

imtbase::CTreeItemModel* CGetStyleDataControllerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_modelIdsCompPtr.FindValue(gqlRequest.GetCommandId()) == -1){
		return nullptr;
	}

	const QList<imtgql::CGqlObject>* fieldList = gqlRequest.GetFields();
	const QList<imtgql::CGqlObject>* paramList = gqlRequest.GetParams();

	int count = fieldList->count();

	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModelPtr = nullptr;
	imtbase::CTreeItemModel* sourceModelPtr = nullptr;
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
		imtbase::CTreeItemModel* errorsItemModelPtr = rootModelPtr->AddTreeModel("errors");
		Q_ASSERT(errorsItemModelPtr != nullptr);

		errorsItemModelPtr->SetData("message", errorMessage);
	}
	else{
		dataModelPtr = new imtbase::CTreeItemModel();
		QString pathToTheme;
		if (theme == ""){
			if (m_parameterCompPtr.IsValid()){
				iprm::ISelectionParam* selectionParam = dynamic_cast<iprm::ISelectionParam*>(m_parameterCompPtr.GetPtr());
				if (selectionParam != nullptr){
					const iprm::IOptionsList* optionList = selectionParam->GetSelectionConstraints();

					if (optionList != nullptr){
						int index = selectionParam->GetSelectedOptionIndex();

						if (index >= 0){
							theme = optionList->GetOptionName(index).toUtf8();
						}
					}
				}
			}
		}

		dataModelPtr->SetData("theme", theme);
		QString prefix;
		if (m_prefixFileNameAttrPtr.IsValid() && !(*m_prefixFileNameAttrPtr).isEmpty()){
			prefix = *m_prefixFileNameAttrPtr + '_';
		}
		if (theme == "Dark"){
			pathToTheme = ":/Style/" + prefix + "dark.theme";
		}
		else if (theme == "Light"){
			pathToTheme = ":/Style/" + prefix + "light.theme";
		}

		QFile resource(pathToTheme);
		if (resource.open(QIODevice::ReadOnly)){
			QByteArray resources = resource.readAll();
			sourceModelPtr = new imtbase::CTreeItemModel();
			sourceModelPtr->Parse(resources);
			dataModelPtr->SetExternTreeModel("source", sourceModelPtr);
		}

		QFile decorators(":/Decorators/" + prefix + "decorators.theme");
		if (decorators.open(QIODevice::ReadOnly)){
			imtbase::CTreeItemModel* decoratorsModelPtr = new imtbase::CTreeItemModel();
			if (decoratorsModelPtr->Parse(decorators.readAll())){
				dataModelPtr->SetExternTreeModel("decorators", decoratorsModelPtr);
			}
		}
	}

	rootModelPtr->SetExternTreeModel("data", dataModelPtr);

	return rootModelPtr;
}


} // namespace imtgql


