// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/imtserverapp.h>


namespace imtserverapp
{


void SetupPageGroupRepresentation(
			const IPageGuiElementModel& page,
			const IRepresentationController& groupRepresentationController,
			QJsonObject& representation,
			const iprm::IParamsSet* paramsPtr)
{
	QJsonObject groupRepresentation;
	const IGuiElementModel* groupPtr = page.GetGroup();
	if (groupPtr != nullptr){
		groupRepresentationController.GetRepresentationFromDataModel(*groupPtr, groupRepresentation, paramsPtr);
	}

	representation.insert(QStringLiteral("groupId"), groupRepresentation.value(QStringLiteral("id")).toString());
	representation.insert(QStringLiteral("groupName"), groupRepresentation.value(QStringLiteral("name")).toString());
	representation.insert(QStringLiteral("groupIcon"), groupRepresentation.value(QStringLiteral("icon")).toString());
}


} // namespace imtserverapp


