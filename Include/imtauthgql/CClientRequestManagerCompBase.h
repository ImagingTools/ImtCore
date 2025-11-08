#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <imtauth/IAccessTokenProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlContext.h>


namespace imtauthgql
{


class CClientRequestManagerCompBase:
			public imtclientgql::CClientRequestManagerCompBase
{
public:
	typedef imtclientgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CClientRequestManagerCompBase)
		I_ASSIGN(m_accessTokenProviderCompPtr, "AccessTokenProvider", "Access token provider", false, "AccessTokenProvider");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info", true, "ApplicationInfo");
	I_END_COMPONENT;

protected:
	QByteArrayList GetElementIds(const QByteArray& collectionId) const;
	bool RemoveElements(const QByteArray& collectionId, const QByteArrayList& elementIds) const;

	template<class Arguments, class Payload, class SdlRequest>
	bool SendModelRequestInternal(Arguments arguments, Payload& payload) const
	{
		imtgql::CGqlRequest gqlRequest;
	
		if (m_accessTokenProviderCompPtr.IsValid()){
			QByteArray accessToken = m_accessTokenProviderCompPtr->GetToken("");
			imtgql::CGqlContext* gqlContextPtr = new imtgql::CGqlContext();
			gqlContextPtr->SetToken(accessToken);
			gqlRequest.SetGqlContext(gqlContextPtr);
		}
	
		if (!SdlRequest::SetupGqlRequest(gqlRequest, arguments)){
			return false;
		}
	
		QString errorMessage;
		payload = SendModelRequest<Payload>(gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			return false;
		}
	
		if (!payload.Version_1_0.HasValue()){
			return false;
		}
	
		return true;
	}

protected:
	I_REF(imtauth::IAccessTokenProvider, m_accessTokenProviderCompPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
};


} // namespace imtauthgql


