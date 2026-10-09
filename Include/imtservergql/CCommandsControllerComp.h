// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtserverapp/IGuiElementContainer.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Commands_fwd.h>


namespace imtservergql
{


class CCommandsControllerComp: public sdl::V1_0::imtbase::CCommandsGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtbase::CCommandsGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCommandsControllerComp);
		I_ASSIGN(m_guiElementContainerCompPtr, "GuiElementContainer", "GUI element  container", true, "GuiElementContainer");
		I_ASSIGN(m_typeIdAttrPtr, "TypeId", "Type-ID", true, "");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtbase::CCommandsGqlHandlerCompBase)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual sdl::V1_0::imtbase::CGuiElementContainer OnGetCommands(
				const sdl::V1_0::imtbase::CGetCommandsGqlRequest& getCommandsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	bool GetRepresentationFromGuiElementContainer(
			const imtserverapp::IGuiElementContainer& guiElementContainer,
			sdl::V1_0::imtbase::CGuiElementContainer& representation,
			const QByteArray& languageId,
			const imtauth::IUserInfo::FeatureIds& userPermissions,
			bool isAdmin,
			const QByteArray& permissionPath) const;
	bool GetRepresentationFromGuiElement(
	const imtserverapp::IGuiElementModel& guiElementModel,
	sdl::V1_0::imtbase::CGuiElementModel& representation,
	const QByteArray& languageId) const;
	QString TranslateName(const QString& name, const QString& context, const QByteArray& languageId) const;
	
protected:
	I_REF(imtserverapp::IGuiElementContainer, m_guiElementContainerCompPtr);
	I_ATTR(QByteArray, m_typeIdAttrPtr);
};


} // namespace imtservergql


