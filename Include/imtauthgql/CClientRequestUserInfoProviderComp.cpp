#include <imtauthgql/CClientRequestUserInfoProviderComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtbase/CCollectionInfo.h>
#include <imtgql/CGqlContext.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtauth::IUserInfoProvider)

const imtbase::ICollectionInfo& CClientRequestUserInfoProviderComp::GetUserList() const
{
	static imtbase::CCollectionInfo emptyCollection;

	return emptyCollection;
}


const imtauth::IUserInfoUniquePtr CClientRequestUserInfoProviderComp::GetUser(const QByteArray& userId, const iprm::IParamsSet* paramsPtr) const
{
	namespace userssdl = sdl::imtauth::Users;

	userssdl::UserItemRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = userId;

	if (m_applicationInfoCompPtr.IsValid()){
		arguments.input.Version_1_0->productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	}

	imtgql::CGqlRequest gqlRequest;

	if (paramsPtr != nullptr){
		iprm::TParamsPtr<iprm::IIdParam> tokenParamPtr(paramsPtr, imtbase::s_authenticationTokenHeaderId);
		if (tokenParamPtr.IsValid()){
			QByteArray token = tokenParamPtr->GetId();
			imtgql::CGqlContext* gqlContextPtr = new imtgql::CGqlContext();
			gqlContextPtr->SetToken(token);
			gqlRequest.SetGqlContext(gqlContextPtr);
		}
	}

	if (!userssdl::CUserItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return nullptr;
	}

	QString errorMessage;
	userssdl::CUserData payload = SendModelRequest<userssdl::CUserData>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return nullptr;
	}

	if (!payload.Version_1_0.HasValue()){
		return nullptr;
	}

	imtauth::IUserInfoUniquePtr userInfoPtr = m_userFactoryCompPtr.CreateInstance();
	if (!userInfoPtr.IsValid()){
		return nullptr;
	}

	if (!m_userRepresentationController.FillUserInfoFromRepresentation(*payload.Version_1_0, *userInfoPtr, nullptr, userId, errorMessage)){
		return nullptr;
	}

	return userInfoPtr.PopInterfacePtr();
}


} // namespace imtauthgql


