#include <imtguigql/CGetStyleDataControllerComp.h>


// ACF includes
#include <iprm/IOptionsList.h>

// Qt includes
#include <QtCore/QFile>


namespace imtguigql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* CGetStyleDataControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QString theme;

	const QList<imtgql::CGqlObject>* paramsPtr = gqlRequest.GetParams();
	if (!paramsPtr->isEmpty()){
		theme = paramsPtr->at(0).GetFieldArgumentValue("theme").toString();
	}

	if(theme.isEmpty()){
		if (m_selectionParamCompPtr.IsValid()){
			const iprm::IOptionsList* optionList = m_selectionParamCompPtr->GetSelectionConstraints();
			if (optionList != nullptr){
				int index = m_selectionParamCompPtr->GetSelectedOptionIndex();
				if (index >= 0){
					theme = optionList->GetOptionName(index);
				}
			}
		}
	}

	if(!theme.isEmpty()){
		istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
		imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

		dataModelPtr->SetData("theme", theme);

		QString prefix;
		QString pathToTheme;

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
			istd::TDelPtr<imtbase::CTreeItemModel> sourceModelPtr(new imtbase::CTreeItemModel());

			QByteArray resources = resource.readAll();
			if (sourceModelPtr->CreateFromJson(resources)){
				dataModelPtr->SetExternTreeModel("source", sourceModelPtr.GetPtr());
			}
		}

		QFile decorators(":/Decorators/" + prefix + "decorators.theme");
		if (decorators.open(QIODevice::ReadOnly)){
			istd::TDelPtr<imtbase::CTreeItemModel> decoratorsModelPtr(new imtbase::CTreeItemModel());

			QByteArray decoratorsData = decorators.readAll();
			if (decoratorsModelPtr->CreateFromJson(decoratorsData)){
				dataModelPtr->SetExternTreeModel("decorators", decoratorsModelPtr.GetPtr());
			}
		}

		return rootModelPtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtgql


