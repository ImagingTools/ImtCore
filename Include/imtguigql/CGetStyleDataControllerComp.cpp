// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtguigql/CGetStyleDataControllerComp.h>


// Qt includes
#include <QtCore/QFile>

// ACF includes
#include <iprm/IOptionsList.h>


namespace imtguigql
{


// protected methods

// reimplemented (imtservergql::CGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* CGetStyleDataControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const imtgql::CGqlParamObject* gqlInputParamPtr = gqlRequest.GetParamObject("input");
	if (gqlInputParamPtr == nullptr){
		errorMessage = QString("Unable to get style. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "CGetStyleDataControllerComp");

		return nullptr;
	}

	QString theme = gqlInputParamPtr->GetParamArgumentValue("theme").toString();
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
		errorMessage = QString("Unable to get style. Theme is empty.");
		SendErrorMessage(0, errorMessage, "CGetStyleDataControllerComp");

		return nullptr;
	}

	QString prefix;
	if (m_prefixFileNameAttrPtr.IsValid() && !(*m_prefixFileNameAttrPtr).isEmpty()){
		prefix = *m_prefixFileNameAttrPtr + '_';
	}

	QString pathToTheme = prefix + theme.toLower() + ".theme";
	QFile resource(":/Style/" + pathToTheme);
	if (!resource.open(QIODevice::ReadOnly)){
		errorMessage = QString("Unable to open the style file. Error: '%1'.").arg(resource.errorString());
		SendErrorMessage(0, errorMessage, "CGetStyleDataControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	imtbase::CTreeItemModel* sourceModelPtr = dataModelPtr->AddTreeModel("source");

	dataModelPtr->SetData("theme", theme);

	QByteArray resources = resource.readAll();
	if (!sourceModelPtr->CreateFromJson(resources)){
		SendWarningMessage(0, QString("Unable to create style model from file: '%1'").arg(pathToTheme), "CGetStyleDataControllerComp");
	}

	resource.close();

	return rootModelPtr.PopPtr();
}


} // namespace imtgql


