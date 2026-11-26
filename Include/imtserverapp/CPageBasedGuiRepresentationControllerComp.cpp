// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CPageBasedGuiRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtserverapp/CCommandRepresentationController.h>
#include <imtserverapp/IPageGuiElementModel.h>
#include <imtserverapp/IGuiElementContainer.h>
#include <imtserverapp/imtserverapp.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtbase::CGuiElementRepresentationControllerComp)

bool CPageBasedGuiRepresentationControllerComp::SetupItemModel(
			const imtserverapp::IGuiElementModel& guiElementModel,
			QJsonObject& representation,
			const iprm::IParamsSet* paramsPtr) const
{
	BaseClass::SetupItemModel(guiElementModel, representation, paramsPtr);

	const imtserverapp::IPageGuiElementModel* pageGuiElementPtr = dynamic_cast<const imtserverapp::IPageGuiElementModel*>(&guiElementModel);
	if (pageGuiElementPtr == nullptr){
		return false;
	}

	QString pageQmlItemFilePath = pageGuiElementPtr->GetPageQmlItemFilePath();
	QString startSourceItem = pageGuiElementPtr->GetStartSourceItem();

	representation.insert(QStringLiteral("source"), pageQmlItemFilePath);
	representation.insert(QStringLiteral("startItem"), startSourceItem);
	// The group is shown by name and icon only: it carries no permissions of its own
	const CCommandRepresentationController groupRepresentationController(m_translationManagerCompPtr.GetPtr());
	SetupPageGroupRepresentation(*pageGuiElementPtr, groupRepresentationController, representation, paramsPtr);

	return true;
}


} // namespace imtserverapp

