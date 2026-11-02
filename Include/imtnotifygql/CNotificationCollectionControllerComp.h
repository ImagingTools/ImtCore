// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtnotify/INotificationInboxController.h>
#include <GeneratedFiles/imtnotifysdl/SDL/1.0/CPP/Notifications_fwd.h>


namespace imtnotifygql
{


/**
	The inbox of the caller as the collection "Notifications" (command UserNotificationsList).
	The inbox is taken from the request context; an anonymous request is refused.
	Only the list, the headers and RemoveElements are served; RemoveElements removes through the inbox controller, so the sessions learn it.
*/
class CNotificationCollectionControllerComp:
			public sdl::V1_0::imtnotify::CNotificationCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::imtnotify::CNotificationCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CNotificationCollectionControllerComp);
		I_ASSIGN(m_inboxControllerCompPtr, "InboxController", "Controller removing the notifications of an inbox", true, "InboxController");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtnotify::CNotificationCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::V1_0::imtnotify::CUserNotificationsListGqlRequest& listRequest,
				sdl::V1_0::imtnotify::CUserNotification& representationObject,
				QString& errorMessage) const override;
	virtual void SetAdditionalFilters(
				const imtgql::CGqlRequest& gqlRequest,
				const imtgql::CGqlParamObject& viewParamsGql,
				iprm::CParamsSet* filterParamsPtr) const override;

	// reimplemented (imtservergql::CObjectCollectionControllerCompBase)
	virtual sdl::V1_0::imtbase::CRemoveElementsPayload OnRemoveElements(
				const sdl::V1_0::imtbase::CRemoveElementsGqlRequest& removeElementsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtnotify::INotificationInboxController, m_inboxControllerCompPtr);
};


} // namespace imtnotifygql


