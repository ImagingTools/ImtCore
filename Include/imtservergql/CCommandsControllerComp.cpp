// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CCommandsControllerComp.h>


// ACF includes
#include <iqt/iqt.h>

// ImtCore includes
#include <imtserverapp/IGuiElementModel.h>


namespace imtservergql
{


// protected methods

// reimplemented (sdl::imtbase::Commands::CGraphQlHandlerCompBase)

bool CCommandsControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
	if (!isSupported){
		return false;
	}

	namespace Commands = ::sdl::imtbase::Commands;

	Commands::CGetCommandsGqlRequest getCommandsGqlRequest(gqlRequest, false);
	const Commands::GetCommandsRequestArguments& arguments = getCommandsGqlRequest.GetRequestedArguments();
	if (!getCommandsGqlRequest.IsValid() || !arguments.input.Version_1_0.has_value()){
		return false;
	}
	
	const QByteArray& typeId = arguments.input.Version_1_0->typeId.value_or(QByteArray());
	if (!typeId.isEmpty()){
		return *m_typeIdAttrPtr == typeId;
	}

	return false;
}


sdl::imtbase::Commands::CGuiElementContainer CCommandsControllerComp::OnGetCommands(
			const sdl::imtbase::Commands::CGetCommandsGqlRequest& getCommandsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtbase::Commands::CGuiElementContainer::V1_0 response;
	
	if (!m_guiElementContainerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'GuiElementContainer' was not set", "CCommandsControllerComp");
		return sdl::imtbase::Commands::CGuiElementContainer();
	}
	
	sdl::imtbase::Commands::GetCommandsRequestArguments arguments = getCommandsRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		Q_ASSERT(false);
		return sdl::imtbase::Commands::CGuiElementContainer();
	}
	
	QByteArray typeId;
	if (arguments.input.Version_1_0->typeId){
		typeId = *arguments.input.Version_1_0->typeId;
	}
	
	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = getCommandsRequest.GetRequestContext();
	if (gqlContextPtr !=  nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}
	
	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();

	if (!GetRepresentationFromGuiElementContainer(*m_guiElementContainerCompPtr, response, languageId, userInfoPtr)){
		errorMessage = QString("Unable to get commands for type-ID '%1'. Error: Get representation failed").arg(qPrintable(typeId));
		SendErrorMessage(0, errorMessage, "CCommandsControllerComp");
		return sdl::imtbase::Commands::CGuiElementContainer();
	}
	
	sdl::imtbase::Commands::CGuiElementContainer retVal;
	retVal.Version_1_0 = std::move(response);
	
	return retVal;
}


bool CCommandsControllerComp::CheckPermissions(const imtgql::CGqlRequest& /*gqlRequest*/, QString& /*errorMessage*/) const
{
	return true;
}


// private methods

bool CCommandsControllerComp::GetRepresentationFromGuiElementContainer(
			const imtserverapp::IGuiElementContainer& guiElementContainer,
			sdl::imtbase::Commands::CGuiElementContainer::V1_0& representation,
			const QByteArray& languageId,
			const imtauth::IUserInfo* userInfoPtr) const
{
	imtauth::IUserInfo::FeatureIds userPermissions;
	bool isAdmin = false;
	if (userInfoPtr != nullptr){
		userPermissions = userInfoPtr->GetPermissions();
		isAdmin = userInfoPtr->IsAdmin();
	}
	
	QByteArrayList elementIds = guiElementContainer.GetElementIds();
	
	imtsdl::TElementList<sdl::imtbase::Commands::CGuiElementModel::V1_0> elementList;
	
	for (const QByteArray& elementId : elementIds){
		const imtserverapp::IGuiElementModel* guiElementPtr = guiElementContainer.GetGuiElementModel(elementId);
		if (guiElementPtr != nullptr){
			if (!isAdmin){
				if (m_commandPermissionsCompPtr.IsValid()){
					QByteArrayList elementPermissions = m_commandPermissionsCompPtr->GetCommandPermissions(elementId);
					if (m_checkPermissionCompPtr.IsValid() && !elementPermissions.isEmpty()){
						bool result = m_checkPermissionCompPtr->CheckPermission(userPermissions, elementPermissions);
						if (!result){
							continue;
						}
					}
				}
			}
			
			sdl::imtbase::Commands::CGuiElementModel::V1_0 element;
			if (!GetRepresentationFromGuiElement(*guiElementPtr, element, languageId)){
				return false;
			}
			
			const imtserverapp::IGuiElementContainer* subElementContainerPtr = guiElementPtr->GetSubElements();
			if (subElementContainerPtr != nullptr){
				sdl::imtbase::Commands::CGuiElementContainer::V1_0 subElements;
				if (!GetRepresentationFromGuiElementContainer(*subElementContainerPtr, subElements, languageId, userInfoPtr)){
					return false;
				}
				
				imtsdl::TElementList<sdl::imtbase::Commands::CGuiElementModel::V1_0> subElementList;
				for (const istd::TSharedNullable<sdl::imtbase::Commands::CGuiElementModel::V1_0>& subElement : *subElements.elements){
					subElementList << *subElement;
				}

				element.subElements = subElementList;
			}
			
			elementList << element;
		}
	}
	
	representation.elements = elementList;
	
	return true;
}


bool CCommandsControllerComp::GetRepresentationFromGuiElement(
	const imtserverapp::IGuiElementModel& guiElementModel,
	sdl::imtbase::Commands::CGuiElementModel::V1_0& representation,
	const QByteArray& languageId) const
{
	representation.elementId = guiElementModel.GetElementId();
	
	QString elementName = guiElementModel.GetElementName();
	elementName = TranslateName(elementName, "Attribute", languageId);
	
	representation.elementName = elementName;
	representation.elementDescription = guiElementModel.GetElementDescription();
	representation.elementItemPath = guiElementModel.GetElementItemPath();
	representation.elementStatus = guiElementModel.GetElementStatus();
	representation.enabled = guiElementModel.IsEnabled();
	representation.visible = guiElementModel.IsVisible();
	representation.priority = guiElementModel.GetPriority();
	representation.alignment = guiElementModel.GetAlignment();
	
	return true;
}


QString CCommandsControllerComp::TranslateName(const QString& name, const QString& context, const QByteArray& languageId) const
{
	if (!m_translationManagerCompPtr.IsValid()){
		return name;
	}
	
	return iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), name.toUtf8(), languageId, context.toUtf8());
}


} // namespace imtservergql


