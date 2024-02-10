#include <imtguigql/CGetStyleDataControllerComp.h>


// Qt includes
#include <QtCore/QFile>

// ACF includes
#include <iprm/IOptionsList.h>


namespace imtguigql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* CGetStyleDataControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& /*errorMessage*/) const
{
	QString theme;
	const QList<imtgql::CGqlObject> paramsPtr = gqlRequest.GetParams();
	if (!paramsPtr.isEmpty()){
		theme = paramsPtr.at(0).GetFieldArgumentValue("theme").toString();
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

	if(theme.isEmpty()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	dataModelPtr->SetData("theme", theme);

	QString prefix;
	if (m_prefixFileNameAttrPtr.IsValid() && !(*m_prefixFileNameAttrPtr).isEmpty()){
		prefix = *m_prefixFileNameAttrPtr + '_';
	}

	QString pathToTheme = prefix + theme.toLower() + ".theme";
	QFile resource(":/Style/" + pathToTheme);
	if (resource.open(QIODevice::ReadOnly)){
		imtbase::CTreeItemModel* sourceModelPtr = dataModelPtr->AddTreeModel("source");

		QByteArray resources = resource.readAll();
		sourceModelPtr->CreateFromJson(resources);
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtgql


