// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtguigql/CGetStyleDataControllerComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>

// ACF includes
#include <iprm/IOptionsList.h>


namespace imtguigql
{


// protected methods

// reimplemented (imtservergql::CGqlRepresentationControllerCompBase)

QJsonObject CGetStyleDataControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const imtgql::CGqlParamObject* gqlInputParamPtr = gqlRequest.GetParamObject("input");
	if (gqlInputParamPtr == nullptr){
		errorMessage = QStringLiteral("Unable to get style. GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "CGetStyleDataControllerComp");

		return QJsonObject();
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
		errorMessage = QStringLiteral("Unable to get style. Theme is empty.");
		SendErrorMessage(0, errorMessage, "CGetStyleDataControllerComp");

		return QJsonObject();
	}

	QString prefix;
	if (m_prefixFileNameAttrPtr.IsValid() && !(*m_prefixFileNameAttrPtr).isEmpty()){
		prefix = *m_prefixFileNameAttrPtr + '_';
	}

	QString pathToTheme = prefix + theme.toLower() + ".theme";
	QFile resource(":/Style/" + pathToTheme);
	if (!resource.open(QIODevice::ReadOnly)){
		errorMessage = QStringLiteral("Unable to open the style file. Error: '%1'.").arg(resource.errorString());
		SendErrorMessage(0, errorMessage, "CGetStyleDataControllerComp");

		return QJsonObject();
	}

	QByteArray resources = resource.readAll();
	resource.close();

	QJsonObject sourceObj;
	QJsonParseError parseError;
	QJsonDocument sourceDoc = QJsonDocument::fromJson(resources, &parseError);
	if (parseError.error != QJsonParseError::NoError){
		SendWarningMessage(0, QStringLiteral("Unable to create style model from file: '%1'. Error: %2").arg(pathToTheme, parseError.errorString()), "CGetStyleDataControllerComp");
	}
	else{
		sourceObj = sourceDoc.object();
	}

	QJsonObject dataObj;
	dataObj.insert("theme", theme);
	dataObj.insert("source", sourceObj);

	QJsonObject rootObj;
	rootObj.insert("data", dataObj);

	return rootObj;
}


} // namespace imtgql


