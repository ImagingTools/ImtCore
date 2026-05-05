// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteTenantControllerComp.h>


// ImtCore includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


namespace imtauthgql
{


// reimplemented (imtauth::ITenantManager)

QByteArrayList CRemoteTenantControllerComp::GetTenantIds() const
{
	return GetElementIds(QByteArrayLiteral("Tenants"));
}


imtauth::ITenantInfoUniquePtr CRemoteTenantControllerComp::GetTenant(const QByteArray& tenantId) const
{
	namespace tenantsdl = sdl::imtauth::Tenants;

	if (!m_tenantFactoryCompPtr.IsValid()){
		return nullptr;
	}

	tenantsdl::GetTenantRequestArguments arguments;
	arguments.input.Version_1_0 = tenantsdl::CGetTenantInput::V1_0();
	arguments.input.Version_1_0->tenantId = tenantId;

	imtgql::CGqlRequest gqlRequest;
	if (!tenantsdl::CGetTenantGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return nullptr;
	}

	typedef tenantsdl::CGetTenantPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return nullptr;
	}

	if (!response.Version_1_0 || !response.Version_1_0->tenant.has_value()){
		return nullptr;
	}

	const auto& tenantData = *response.Version_1_0->tenant;

	imtauth::ITenantInfoUniquePtr tenantInfoPtr = m_tenantFactoryCompPtr.CreateInstance();
	if (!tenantInfoPtr.IsValid()){
		return nullptr;
	}

	if (tenantData.id){
		tenantInfoPtr->SetTenantId(*tenantData.id);
	}
	if (tenantData.name){
		tenantInfoPtr->SetTenantName(*tenantData.name);
	}
	if (tenantData.description){
		tenantInfoPtr->SetTenantDescription(*tenantData.description);
	}
	if (tenantData.isActive){
		tenantInfoPtr->SetActive(*tenantData.isActive);
	}

	return tenantInfoPtr;
}


QByteArray CRemoteTenantControllerComp::CreateTenant(const QString& tenantName, const QString& description)
{
	namespace tenantsdl = sdl::imtauth::Tenants;

	tenantsdl::CreateTenantRequestArguments arguments;
	arguments.input.Version_1_0 = tenantsdl::CCreateTenantInput::V1_0();
	arguments.input.Version_1_0->name = tenantName;
	arguments.input.Version_1_0->description = description;

	imtgql::CGqlRequest gqlRequest;
	if (!tenantsdl::CCreateTenantGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return QByteArray();
	}

	typedef tenantsdl::CCreateTenantPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return QByteArray();
	}

	if (response.Version_1_0 && response.Version_1_0->tenantId.has_value()){
		return *response.Version_1_0->tenantId;
	}

	return QByteArray();
}


bool CRemoteTenantControllerComp::RemoveTenant(const QByteArray& tenantId)
{
	namespace tenantsdl = sdl::imtauth::Tenants;

	tenantsdl::RemoveTenantRequestArguments arguments;
	arguments.input.Version_1_0 = tenantsdl::CRemoveTenantInput::V1_0();
	arguments.input.Version_1_0->tenantId = tenantId;

	imtgql::CGqlRequest gqlRequest;
	if (!tenantsdl::CRemoveTenantGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	typedef tenantsdl::CRemoveTenantPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (response.Version_1_0 && response.Version_1_0->success.has_value()){
		return *response.Version_1_0->success;
	}

	return false;
}


bool CRemoteTenantControllerComp::UpdateTenant(const QByteArray& tenantId, const QString& tenantName, const QString& description)
{
	namespace tenantsdl = sdl::imtauth::Tenants;

	tenantsdl::UpdateTenantRequestArguments arguments;
	arguments.input.Version_1_0 = tenantsdl::CUpdateTenantInput::V1_0();
	arguments.input.Version_1_0->tenantId = tenantId;
	arguments.input.Version_1_0->name = tenantName;
	arguments.input.Version_1_0->description = description;

	imtgql::CGqlRequest gqlRequest;
	if (!tenantsdl::CUpdateTenantGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	typedef tenantsdl::CUpdateTenantPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (response.Version_1_0 && response.Version_1_0->success.has_value()){
		return *response.Version_1_0->success;
	}

	return false;
}


bool CRemoteTenantControllerComp::SetTenantActive(const QByteArray& tenantId, bool isActive)
{
	namespace tenantsdl = sdl::imtauth::Tenants;

	tenantsdl::SetTenantActiveRequestArguments arguments;
	arguments.input.Version_1_0 = tenantsdl::CSetTenantActiveInput::V1_0();
	arguments.input.Version_1_0->tenantId = tenantId;
	arguments.input.Version_1_0->isActive = isActive;

	imtgql::CGqlRequest gqlRequest;
	if (!tenantsdl::CSetTenantActiveGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	typedef tenantsdl::CSetTenantActivePayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (response.Version_1_0 && response.Version_1_0->success.has_value()){
		return *response.Version_1_0->success;
	}

	return false;
}


} // namespace imtauthgql
