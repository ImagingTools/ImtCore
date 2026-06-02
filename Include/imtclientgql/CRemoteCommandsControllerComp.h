// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Commands.h>


namespace imtclientgql
{


class CRemoteCommandsControllerComp:
			public TClientRequestManagerCompWrap<
						sdl::V1_0::imtbase::CCommandsGqlHandlerCompBase>
{
public:
	typedef TClientRequestManagerCompWrap<
				sdl::V1_0::imtbase::CCommandsGqlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteCommandsControllerComp);
		I_ASSIGN_MULTI_0(m_typeIdsAttrPtr, "TypeIds", "Type ID-s", true);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtbase::CCommandsGqlHandlerCompBase)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual sdl::V1_0::imtbase::CGuiElementContainer OnGetCommands(
				const sdl::V1_0::imtbase::CGetCommandsGqlRequest& getCommandsRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

protected:
	I_MULTIATTR(QByteArray, m_typeIdsAttrPtr);
};


} // namespace imtclientgql


