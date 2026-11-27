// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantDirectoryControllerComp.h>


// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/TElementList.h>
#include <imtauthgql/TenantAccess.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantDirectory.h>


namespace imtauthgql
{


namespace sdlauth = sdl::V1_0::imtauth;


namespace
{


sdlauth::CDirectoryProfile CreateProfileData(const imtauth::ITenantDirectoryProfile& profile)
{
	sdlauth::CDirectoryProfile retVal;
	retVal.tenantId = profile.GetTenantId();
	retVal.tenantName = profile.GetTenantName();
	retVal.isListed = profile.IsListed();
	retVal.acceptsInvitationsFromUnknown = profile.IsAcceptingInvitationsFromUnknown();
	retVal.description = profile.GetDescription();
	retVal.offeredRoleIds = imtsdl::ToElementList(profile.GetOfferedRoleIds());
	retVal.industry = profile.GetIndustry();
	retVal.country = profile.GetCountry();
	retVal.website = profile.GetWebsite();
	retVal.updatedAt = imtsdl::ToSdlTime(profile.GetUpdatedAt());

	return retVal;
}


} // anonymous namespace


// protected methods

// reimplemented (sdl::V1_0::imtauth::CTenantDirectoryGqlHandlerCompBase)

sdlauth::CGetDirectoryProfilePayload CTenantDirectoryControllerComp::OnGetDirectoryProfile(
			const sdlauth::CGetDirectoryProfileGqlRequest& getDirectoryProfileRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdlauth::CGetDirectoryProfilePayload response;

	const sdlauth::GetDirectoryProfileRequestArguments arguments = getDirectoryProfileRequest.GetRequestedArguments();
	const QByteArray tenantId = arguments.input ? arguments.input->tenantId.value_or(QByteArray()) : QByteArray();

	QString accessError;
	if (!HasTenantIsolationAccess(*m_tenantManagerCompPtr, m_membershipManagerCompPtr.GetPtr(), gqlRequest, tenantId, accessError)){
		response.errorMessage = accessError;

		return response;
	}

	imtauth::ITenantDirectoryProfileUniquePtr profilePtr = m_directoryCompPtr->GetProfile(tenantId);
	if (!profilePtr.IsValid()){
		response.errorMessage = QStringLiteral("The profile of the organization could not be read");

		return response;
	}

	response.profile = CreateProfileData(*profilePtr);

	return response;
}


sdlauth::CSetDirectoryProfilePayload CTenantDirectoryControllerComp::OnSetDirectoryProfile(
			const sdlauth::CSetDirectoryProfileGqlRequest& setDirectoryProfileRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdlauth::CSetDirectoryProfilePayload response;
	response.success = false;

	const sdlauth::SetDirectoryProfileRequestArguments arguments = setDirectoryProfileRequest.GetRequestedArguments();
	const QByteArray tenantId = arguments.input ? arguments.input->tenantId.value_or(QByteArray()) : QByteArray();

	QString error;
	if (!HasTenantManageAccess(*m_tenantManagerCompPtr, m_membershipManagerCompPtr.GetPtr(), gqlRequest, tenantId, {s_editDirectoryProfilePermission}, error)){
		response.errorMessage = error;

		return response;
	}

	imtauth::ITenantDirectoryProfileUniquePtr profilePtr = m_profileFactoryCompPtr.CreateInstance();
	if (!profilePtr.IsValid() || !arguments.input->profile){
		response.errorMessage = QStringLiteral("The profile is missing");

		return response;
	}

	const sdlauth::CDirectoryProfileInput& input = *arguments.input->profile;
	profilePtr->SetTenantId(tenantId);
	profilePtr->SetListed(input.isListed.value_or(false));
	profilePtr->SetAcceptingInvitationsFromUnknown(input.acceptsInvitationsFromUnknown.value_or(false));
	profilePtr->SetDescription(input.description.value_or(QString()).trimmed());
	profilePtr->SetOfferedRoleIds(imtsdl::FromElementList(input.offeredRoleIds));
	profilePtr->SetIndustry(input.industry.value_or(QString()).trimmed());
	profilePtr->SetCountry(input.country.value_or(QString()).trimmed());
	profilePtr->SetWebsite(input.website.value_or(QString()).trimmed());

	if (!m_directoryCompPtr->SetProfile(*profilePtr, error)){
		response.errorMessage = error;

		return response;
	}

	response.success = true;

	return response;
}


} // namespace imtauthgql


