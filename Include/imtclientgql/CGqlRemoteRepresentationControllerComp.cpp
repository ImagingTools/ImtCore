// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CGqlRemoteRepresentationControllerComp.h>

// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>

// ImtCore includes
#include <imtclientgql/IGqlClient.h>


namespace imtclientgql
{


// protected methods

// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)

QJsonObject CGqlRemoteRepresentationControllerComp::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!IsRequestSupported(gqlRequest)){
		errorMessage = QStringLiteral("GraphQL-request is not supported").toUtf8();
		SendErrorMessage(0, errorMessage, "CGqlRemoteRepresentationControllerComp");

		return QJsonObject();
	}

	istd::TUniqueInterfacePtr<imtgql::CGqlRequest> gqlRequestPtr;
	gqlRequestPtr.MoveCastedPtr(gqlRequest.CloneMe());

	if (!gqlRequestPtr.IsValid()){
		errorMessage = QStringLiteral("GraphQL-request could not be copied").toUtf8();
		SendErrorMessage(0, errorMessage, "CGqlRemoteRepresentationControllerComp");

		return QJsonObject();
	}

	QByteArray productId;
	if (m_productIdAttrPtr.IsValid()){
		productId = *m_productIdAttrPtr;
	}

	if (m_applicationInfoCompPtr.IsValid()){
		productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	}

	const imtgql::CGqlParamObject* inputParam = gqlRequestPtr->GetParamObject("input");
	if (inputParam != nullptr){
		istd::TUniqueInterfacePtr<imtgql::CGqlParamObject> newInputParamPtr;
		newInputParamPtr.MoveCastedPtr(inputParam->CloneMe());
		if (!newInputParamPtr.IsValid()){
			errorMessage = QStringLiteral("Creation of input parameters related to the GraphQL-request failed").toUtf8();
			SendErrorMessage(0, errorMessage, "CGqlRemoteRepresentationControllerComp");

			return QJsonObject();
		}

		newInputParamPtr->InsertParam("productId", QVariant(productId));
		gqlRequestPtr->AddParam("input", *newInputParamPtr.GetPtr());
	}
	else{
		imtgql::CGqlParamObject inputGqlObject;
		inputGqlObject.InsertParam("productId", QVariant(productId));

		gqlRequestPtr->AddParam("input", inputGqlObject);
	}

	imtclientgql::IGqlClient::GqlRequestPtr requestPtr;
	requestPtr.MoveCastedPtr(gqlRequestPtr->CloneMe());
	if (requestPtr.IsValid()){
		imtclientgql::IGqlClient::GqlResponsePtr responsePtr = m_apiClientCompPtr->SendRequest(requestPtr);
		if (responsePtr.IsValid()){
			return CreateJsonObjectFromResponse(gqlRequest.GetCommandId(), *responsePtr);
		}
	}

	errorMessage = QStringLiteral("Failed to create a network request to a remote server").toUtf8();
	SendErrorMessage(0, errorMessage, "CGqlRemoteRepresentationControllerComp");

	return QJsonObject();
}


} // namespace imtclientgql


