#include <imtauthgql/CRemoteSuperuserControllerComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


namespace imtauthgql
{


// public methods

// reimplemented (ISuperuserController)

bool CRemoteSuperuserControllerComp::SetSuperuserPassword(const QByteArray& password) const
{
	imtgql::CGqlRequest request(imtgql::CGqlRequest::RT_QUERY, sdl::imtauth::Users::V1_0::CUserAddGqlRequest::GetCommandId());

	imtgql::CGqlObject inputObject;
	inputObject.InsertField(sdl::imtauth::Users::V1_0::CUserDataInput::UserDataInputFields::Id.toUtf8(), "su");
	inputObject.InsertField(sdl::imtauth::Users::V1_0::CUserDataInput::UserDataInputFields::ProductId.toUtf8(), qPrintable(*m_productIdAttrPtr));

	sdl::imtauth::Users::V1_0::CUserData userData;
	userData.SetId("su");
	userData.SetUsername("su");
	userData.SetName("superuser");
	userData.SetPassword(qPrintable(password));

	imtgql::CGqlObject* userGqlObject = inputObject.CreateFieldObject(sdl::imtauth::Users::V1_0::CUserDataInput::UserDataInputFields::Item.toUtf8());
	if (!userData.WriteToGraphQlObject(*userGqlObject)){
		return false;
	}

	request.AddParam("input", inputObject);

	imtgql::CGqlObject fieldObject;
	fieldObject.InsertField("Id");
	request.AddField(sdl::imtauth::Users::V1_0::CAddedNotificationPayload::AddedNotificationPayloadFields::AddedNotification.toUtf8(), fieldObject);

	imtbase::CTreeItemModel responseModel;
	return SendModelRequest(request, responseModel);
}


} // namespace imtauthgql


