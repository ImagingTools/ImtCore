// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtserverapp/IGuiElementContainer.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Commands.h>


namespace imtservergql
{


class CCommandsControllerComp: public sdl::imtbase::Commands::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtbase::Commands::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCommandsControllerComp);
		I_ASSIGN(m_guiElementContainerCompPtr, "GuiElementContainer", "GUI element  container", true, "GuiElementContainer");
		I_ASSIGN(m_typeIdAttrPtr, "TypeId", "Type-ID", true, "");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtbase::Commands::CGraphQlHandlerCompBase)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual sdl::imtbase::Commands::CGuiElementContainer OnGetCommands(
				const sdl::imtbase::Commands::CGetCommandsGqlRequest& getCommandsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	bool GetRepresentationFromGuiElementContainer(
			const imtserverapp::IGuiElementContainer& guiElementContainer,
			sdl::imtbase::Commands::CGuiElementContainer::V1_0& representation,
			const QByteArray& languageId,
			const imtauth::IUserInfo* userInfoPtr) const;
	bool GetRepresentationFromGuiElement(
	const imtserverapp::IGuiElementModel& guiElementModel,
	sdl::imtbase::Commands::CGuiElementModel::V1_0& representation,
	const QByteArray& languageId) const;
	QString TranslateName(const QString& name, const QString& context, const QByteArray& languageId) const;
	
protected:
	I_REF(imtserverapp::IGuiElementContainer, m_guiElementContainerCompPtr);
	I_ATTR(QByteArray, m_typeIdAttrPtr);
};


} // namespace imtservergql


