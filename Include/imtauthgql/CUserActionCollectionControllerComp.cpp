#include <imtauthgql/CUserActionCollectionControllerComp.h>


// ImtCore includes
#include <imtauth/IUserRecentAction.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::UserActions::CUserCollectionControllerCompBase)

bool CUserActionCollectionControllerComp::CreateRepresentationFromObject(
			const ::imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::imtauth::UserActions::CGetUserActionsGqlRequest& getUserActionsRequest,
			sdl::imtauth::UserActions::CUserActionItemData::V1_0& representationObject,
			QString& errorMessage) const
{
	QByteArray objectId = objectCollectionIterator.GetObjectId();
	sdl::imtauth::UserActions::GetUserActionsRequestArguments arguments = getUserActionsRequest.GetRequestedArguments();
	sdl::imtauth::UserActions::GetUserActionsRequestInfo requestInfo = getUserActionsRequest.GetRequestInfo();

	const imtauth::IUserRecentAction* userRecentActionPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (objectCollectionIterator.GetObjectData(dataPtr)){
		userRecentActionPtr = dynamic_cast<const imtauth::IUserRecentAction*>(dataPtr.GetPtr());
	}

	if (userRecentActionPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CUserActionCollectionControllerComp");
		return false;
	}

	if (requestInfo.items.isIdRequested){
		representationObject.id = QByteArray(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		representationObject.typeId = QByteArray(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
	}

	if (requestInfo.items.isUserIdRequested){
		representationObject.userId = userRecentActionPtr->GetUserId();
	}

	if (requestInfo.items.isTargetIdRequested){
		representationObject.targetId = userRecentActionPtr->GetTargetId();
	}

	if (requestInfo.items.isTargetTypeIdRequested){
		representationObject.targetTypeId = userRecentActionPtr->GetTargetTypeId();
	}

	if (requestInfo.items.isActionTypeRequested){
		imtauth::IUserRecentAction::ActionType actionType = userRecentActionPtr->GetActionType();
		switch(actionType){
		case imtauth::IUserRecentAction::AT_UNKNOWN:
			representationObject.actionType = QString("Unknown");
			break;
		case imtauth::IUserRecentAction::AT_CREATE:
			representationObject.actionType = QString("Create");
			break;
		case imtauth::IUserRecentAction::AT_UPDATE:
			representationObject.actionType = QString("Update");
			break;
		case imtauth::IUserRecentAction::AT_DELETE:
			representationObject.actionType = QString("Delete");
			break;
		case imtauth::IUserRecentAction::AT_USER:
			representationObject.actionType = QString("Unknown");
			break;
		}
	}

	if (requestInfo.items.isTimeStampRequested){
		QDateTime timestamp = userRecentActionPtr->GetTimestamp();
		representationObject.timeStamp = QString(timestamp.toLocalTime().toString("dd.MM.yyyy hh:mm:ss"));
	}

	return true;
}


} // namespace imtauth


