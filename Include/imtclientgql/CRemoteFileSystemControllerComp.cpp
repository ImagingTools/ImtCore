// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CRemoteFileSystemControllerComp.h>

// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>

// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtclientgql
{


// protected methods

// reimplemented (CGqlRequestHandlerCompBase)

QJsonObject CRemoteFileSystemControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_apiClientCompPtr.IsValid()){
		errorMessage = QStringLiteral("Unable to redirect file system request. Error: Attribute 'ApiClient' was not set");
		SendErrorMessage(0, errorMessage, "CRemoteFileSystemControllerComp");

		return QJsonObject();
	}

	IGqlClient::GqlRequestPtr requestPtr;
	requestPtr.MoveCastedPtr(gqlRequest.CloneMe());
	if (!requestPtr.IsValid()){
		errorMessage = QStringLiteral("Unable to redirect file system request. Error: Request is invalid");
		SendErrorMessage(0, errorMessage, "CRemoteFileSystemControllerComp");

		return QJsonObject();
	}

	IGqlClient::GqlResponsePtr responsePtr = m_apiClientCompPtr->SendRequest(requestPtr);
	if (!responsePtr.IsValid()){
		errorMessage = QStringLiteral("Unable to redirect file system request. Error: Remote file service is not reachable");
		SendErrorMessage(0, errorMessage, "CRemoteFileSystemControllerComp");

		return QJsonObject();
	}

	QStringList errorMessages = responsePtr->GetErrorMessageList();
	if (!errorMessages.isEmpty()){
		errorMessage = errorMessages.first();

		return QJsonObject();
	}

	const QJsonDocument document = QJsonDocument::fromJson(responsePtr->GetResponseData());
	if (!document.isObject()){
		errorMessage = QStringLiteral("Unable to redirect file system request. Error: Remote response is invalid");

		return QJsonObject();
	}

	QJsonObject responseObj = document.object();

	const QJsonValue dataValue = responseObj.value(QStringLiteral("data"));
	if (dataValue.isObject()){
		QJsonObject dataObj = dataValue.toObject();

		const QJsonValue commandValue = dataObj.value(QString::fromUtf8(gqlRequest.GetCommandId()));
		if (commandValue.isObject()){
			QJsonObject rootObj;
			rootObj.insert(QStringLiteral("data"), commandValue.toObject());

			return rootObj;
		}
	}

	errorMessage = QStringLiteral("Unable to redirect file system request. Error: Remote response is invalid");

	return QJsonObject();
}


} // namespace imtclientgql
