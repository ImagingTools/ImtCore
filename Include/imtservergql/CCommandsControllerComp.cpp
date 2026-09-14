// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CCommandsControllerComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Commands.h>


// ACF includes
#include <iqt/iqt.h>

// ImtCore includes
#include <imtserverapp/imtserverapp.h>
#include <imtserverapp/IGuiElementModel.h>
#include <imtservergql/imtservergql.h>


namespace imtservergql
{


// protected methods

// reimplemented (sdl::V1_0::imtbase::CCommandsGqlHandlerCompBase)

bool CCommandsControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
	if (!isSupported){
		return false;
	}

	namespace Commands = ::sdl::V1_0::imtbase;

	Commands::CGetCommandsGqlRequest getCommandsGqlRequest(gqlRequest, false);
	const Commands::GetCommandsRequestArguments& arguments = getCommandsGqlRequest.GetRequestedArguments();
	if (!getCommandsGqlRequest.IsValid()){
		return false;
	}

	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return false;
	}
	
	const QByteArray& typeId = arguments.input->typeId.value_or(QByteArray());
	if (!typeId.isEmpty()){
		return *m_typeIdAttrPtr == typeId;
	}

	return false;
}


sdl::V1_0::imtbase::CGuiElementContainer CCommandsControllerComp::OnGetCommands(
			const sdl::V1_0::imtbase::CGetCommandsGqlRequest& getCommandsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CGuiElementContainer response;
	
	if (!m_guiElementContainerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'GuiElementContainer' was not set", "CCommandsControllerComp");
		return sdl::V1_0::imtbase::CGuiElementContainer();
	}
	
	sdl::V1_0::imtbase::GetCommandsRequestArguments arguments = getCommandsRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return sdl::V1_0::imtbase::CGuiElementContainer();;
	}
	QByteArray typeId;
	if (arguments.input->typeId){
		typeId = *arguments.input->typeId;
	}

	imtauth::IUserInfo::FeatureIds userPermissions;
	bool isAdmin = false;
	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = getCommandsRequest.GetRequestContext();
	if (gqlContextPtr !=  nullptr){
		languageId = gqlContextPtr->GetLanguageId();

		const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			isAdmin = userInfoPtr->IsAdmin();
		}

		if (!isAdmin){
			// Resolved once for the whole command tree: every element below is
			// checked against it, and what those permissions require is the
			// same for all of them.
			userPermissions = GetUserPermissions(*gqlContextPtr);
		}
	}

	if (!GetRepresentationFromGuiElementContainer(*m_guiElementContainerCompPtr, response, languageId, userPermissions, isAdmin, GetPermissionPath(gqlRequest))){
		errorMessage = QStringLiteral("Unable to get commands for type-ID '%1'. Error: Get representation failed").arg(typeId);
		SendErrorMessage(0, errorMessage, "CCommandsControllerComp");
		return sdl::V1_0::imtbase::CGuiElementContainer();
	}
	
	return response;
}


bool CCommandsControllerComp::CheckPermissions(const imtgql::CGqlRequest& /*gqlRequest*/, QString& /*errorMessage*/) const
{
	return true;
}


// private methods

bool CCommandsControllerComp::GetRepresentationFromGuiElementContainer(
			const imtserverapp::IGuiElementContainer& guiElementContainer,
			sdl::V1_0::imtbase::CGuiElementContainer& representation,
			const QByteArray& languageId,
			const imtauth::IUserInfo::FeatureIds& userPermissions,
			bool isAdmin,
			const QByteArray& permissionPath) const
{
	QByteArrayList elementIds = guiElementContainer.GetElementIds();
	
	imtsdl::TElementList<sdl::V1_0::imtbase::CGuiElementModel> elementList;
	
	for (const QByteArray& elementId : elementIds){
		const imtserverapp::IGuiElementModel* guiElementPtr = guiElementContainer.GetGuiElementModel(elementId);
		if (guiElementPtr != nullptr){
			if (!imtserverapp::IsElementAccessible(
					m_commandPermissionsCompPtr.GetPtr(),
					m_checkPermissionCompPtr.GetPtr(),
					elementId,
					userPermissions,
					isAdmin,
					permissionPath)){
				continue;
			}
			
			sdl::V1_0::imtbase::CGuiElementModel element;
			if (!GetRepresentationFromGuiElement(*guiElementPtr, element, languageId)){
				return false;
			}
			
			const imtserverapp::IGuiElementContainer* subElementContainerPtr = guiElementPtr->GetSubElements();
			if (subElementContainerPtr != nullptr){
				sdl::V1_0::imtbase::CGuiElementContainer subElements;
				if (!GetRepresentationFromGuiElementContainer(*subElementContainerPtr, subElements, languageId, userPermissions, isAdmin, permissionPath)){
					return false;
				}
				
				imtsdl::TElementList<sdl::V1_0::imtbase::CGuiElementModel> subElementList;
				for (const istd::TNullableValue<sdl::V1_0::imtbase::CGuiElementModel>& subElement : *subElements.elements){
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
	sdl::V1_0::imtbase::CGuiElementModel& representation,
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


