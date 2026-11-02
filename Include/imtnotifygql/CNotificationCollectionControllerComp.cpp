// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtnotifygql/CNotificationCollectionControllerComp.h>


// ACF includes
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>
#include <imtnotifygql/imtnotifygql.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>


namespace imtnotifygql
{


namespace
{


void SetIdParam(iprm::CParamsSet& params, const QByteArray& paramId, const QByteArray& id)
{
	istd::TDelPtr<iprm::CIdParam> idParamPtr(new iprm::CIdParam);
	idParamPtr->SetId(id);

	params.SetEditableParameter(paramId, idParamPtr.PopPtr(), true);
}


} // namespace


// protected methods

// reimplemented (sdl::V1_0::imtnotify::CNotificationCollectionControllerCompBase)

bool CNotificationCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::V1_0::imtnotify::CUserNotificationsListGqlRequest& /*listRequest*/,
			sdl::V1_0::imtnotify::CUserNotification& representationObject,
			QString& errorMessage) const
{
	imtbase::IObjectCollection::DataPtr dataPtr;
	const imtnotify::INotification* notificationPtr = nullptr;
	if (objectCollectionIterator.GetObjectData(dataPtr)){
		notificationPtr = dynamic_cast<const imtnotify::INotification*>(dataPtr.GetPtr());
	}

	if (notificationPtr == nullptr){
		errorMessage = QStringLiteral("Unable to read the notification '%1'").arg(QString::fromUtf8(objectCollectionIterator.GetObjectId()));
		SendErrorMessage(0, errorMessage, "CNotificationCollectionControllerComp");

		return false;
	}

	representationObject = GetSdlNotification(objectCollectionIterator.GetObjectId(), *notificationPtr);

	return true;
}


void CNotificationCollectionControllerComp::SetAdditionalFilters(
			const imtgql::CGqlRequest& gqlRequest,
			const imtgql::CGqlParamObject& /*viewParamsGql*/,
			iprm::CParamsSet* filterParamsPtr) const
{
	if (filterParamsPtr == nullptr){
		return;
	}

	// An empty user leaves the selection empty
	const imtnotify::NotificationScope scope = GetScope(gqlRequest.GetRequestContext());
	SetIdParam(*filterParamsPtr, imtnotify::s_inboxProductIdParamId, scope.productId);
	SetIdParam(*filterParamsPtr, imtnotify::s_inboxTenantIdParamId, scope.tenantId);
	SetIdParam(*filterParamsPtr, imtnotify::s_inboxUserIdParamId, scope.userId);
}


// reimplemented (imtservergql::CObjectCollectionControllerCompBase)

sdl::V1_0::imtbase::CRemoveElementsPayload CNotificationCollectionControllerComp::OnRemoveElements(
			const sdl::V1_0::imtbase::CRemoveElementsGqlRequest& removeElementsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CRemoveElementsPayload response;

	const imtnotify::NotificationScope scope = GetScope(gqlRequest.GetRequestContext());
	if (!scope.IsValid() || !m_inboxControllerCompPtr.IsValid()){
		errorMessage = QStringLiteral("The notifications are available only to a signed-in user");

		return response;
	}

	const sdl::V1_0::imtbase::RemoveElementsRequestArguments arguments = removeElementsRequest.GetRequestedArguments();
	QByteArrayList elementIds;
	if (arguments.input && arguments.input->elementIds){
		elementIds = arguments.input->elementIds->ToList();
	}

	// Ids of other inboxes are ignored by the inbox
	QByteArrayList removedIds;
	response.success = m_inboxControllerCompPtr->Remove(scope, elementIds, removedIds);
	if (!*response.success){
		errorMessage = QStringLiteral("The notifications could not be removed");
	}

	return response;
}


// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)

bool CNotificationCollectionControllerComp::CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!GetScope(gqlRequest.GetRequestContext()).IsValid()){
		errorMessage = QStringLiteral("The notifications are available only to a signed-in user");

		return false;
	}

	// The other collection commands would reach a notification by its id, past the inbox
	const QByteArray commandId = gqlRequest.GetCommandId();
	if (		commandId != sdl::V1_0::imtnotify::CUserNotificationsListGqlRequest::GetCommandId() &&
				commandId != sdl::V1_0::imtbase::CGetCollectionHeadersGqlRequest::GetCommandId() &&
				commandId != sdl::V1_0::imtbase::CRemoveElementsGqlRequest::GetCommandId()){
		errorMessage = QStringLiteral("The command is not available for the notifications");

		return false;
	}

	return BaseClass::CheckPermissions(gqlRequest, errorMessage);
}


} // namespace imtnotifygql


