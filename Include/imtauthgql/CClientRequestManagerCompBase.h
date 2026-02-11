// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <imtauth/IAccessTokenProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequestContextManager.h>


namespace imtauthgql
{


class CClientRequestManagerCompBase:
			public imtclientgql::CClientRequestManagerCompBase
{
public:
	typedef imtclientgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CClientRequestManagerCompBase)
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info", true, "ApplicationInfo");
	I_END_COMPONENT;

protected:
	QByteArrayList GetElementIds(const QByteArray& collectionId) const;
	bool RemoveElements(const QByteArray& collectionId, const QByteArrayList& elementIds) const;

	template<class Arguments, class Payload, class SdlRequest>
	bool SendModelRequestInternal(Arguments arguments, Payload& payload) const
	{
		imtgql::CGqlRequest gqlRequest;
	
		imtgql::IGqlContext* gqlContextPtr = imtgql::CGqlRequestContextManager::GetContext();
		if (gqlContextPtr != nullptr){
			gqlRequest.SetGqlContext(dynamic_cast<imtgql::IGqlContext*>(gqlContextPtr->CloneMe().PopInterfacePtr()));
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
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
};


} // namespace imtauthgql


