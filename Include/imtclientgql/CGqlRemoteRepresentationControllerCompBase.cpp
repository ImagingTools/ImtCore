// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CGqlRemoteRepresentationControllerCompBase.h>

// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>

// ImtCore includes
#include <imtgql/CGqlResponse.h>
#include <imtgql/CGqlRequestContextManager.h>


namespace imtclientgql
{


// protected methods

// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)

QJsonObject CGqlRemoteRepresentationControllerCompBase::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!IsRequestSupported(gqlRequest)){
		return QJsonObject();
	}

	if (!m_apiClientCompPtr.IsValid()){
		SendCriticalMessage(0, "Component 'ApiClient' was not set. Check the component configuration");

		return QJsonObject();
	}

	imtclientgql::IGqlClient::GqlRequestPtr requestPtr;
	requestPtr.MoveCastedPtr(gqlRequest.CloneMe());
	if (requestPtr.IsValid()){
		// Forwarded requests otherwise carry no context if the incoming gqlRequest
		// lost its attachment upstream, so the backend sees an anonymous caller and
		// denies any permission-gated command. Only fills in a missing context - an
		// already-attached one is left untouched.
		if (requestPtr->GetRequestContext() == nullptr){
			imtgql::IGqlContext* currentContextPtr = imtgql::CGqlRequestContextManager::GetContext();
			if (currentContextPtr != nullptr){
				imtgql::CGqlRequest* concreteRequestPtr = dynamic_cast<imtgql::CGqlRequest*>(requestPtr.GetPtr());
				if (concreteRequestPtr != nullptr){
					istd::IChangeableUniquePtr clonedContextPtr = currentContextPtr->CloneMe();
					imtgql::IGqlContextUniquePtr castedContextPtr;
					castedContextPtr.MoveCastedPtr(std::move(clonedContextPtr));
					concreteRequestPtr->SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(castedContextPtr));
				}
			}
		}

		imtclientgql::IGqlClient::GqlResponsePtr responsePtr = m_apiClientCompPtr->SendRequest(requestPtr);
		if (responsePtr.IsValid()){
			return CreateJsonObjectFromResponse(gqlRequest.GetCommandId(), *responsePtr);
		}

		errorMessage = QString("Command could not be processed by the remote server: '%1'").arg(qPrintable(requestPtr->GetCommandId()));
	}

	return QJsonObject();
}


// private methods

QJsonObject CGqlRemoteRepresentationControllerCompBase::CreateJsonObjectFromResponse(
			const QByteArray& commandId,
			const imtgql::IGqlResponse& response) const
{
	const imtgql::IGqlResponse::GqlRequestPtr requestPtr = response.GetOriginalRequest();
	if (!requestPtr.IsValid()){
		return QJsonObject();
	}

	const QByteArray responseData = response.GetResponseData();
	const QJsonDocument document = QJsonDocument::fromJson(responseData);
	if (!document.isObject()){
		return QJsonObject();
	}

	const QJsonObject rootObject = document.object();

	QJsonObject resultObject;
	bool hasResult = false;

	// WebSocket / subscription payloads
	const QJsonValue typeValue = rootObject.value(QStringLiteral("type"));
	if (typeValue.isString() && typeValue.toString() == QStringLiteral("query_data")){
		const QJsonValue payloadValue = rootObject.value(QStringLiteral("payload"));
		if (payloadValue.isObject()){
			const QJsonObject payloadObject = payloadValue.toObject();

			const QJsonValue errorsValue = payloadObject.value(QStringLiteral("errors"));
			if (errorsValue.isArray()){
				const QJsonArray errorArray = errorsValue.toArray();
				if (!errorArray.isEmpty()){
					resultObject.insert(QStringLiteral("errors"), errorArray.at(0));
					hasResult = true;
				}
			}

			if (!hasResult){
				const QJsonValue dataValue = payloadObject.value(QStringLiteral("data"));
				if (dataValue.isObject()){
					const QJsonObject payloadData = dataValue.toObject();
					const QJsonValue commandValue = payloadData.value(QString::fromUtf8(commandId));
					if (!commandValue.isUndefined() && !commandValue.isNull()){
						resultObject.insert(QStringLiteral("data"), commandValue);
						hasResult = true;
					}
				}
			}
		}
	}
	else{
		// Standard errors
		const QJsonValue errorsValue = rootObject.value(QStringLiteral("errors"));
		if (errorsValue.isArray()){
			const QJsonArray errorArray = errorsValue.toArray();
			if (!errorArray.isEmpty()){
				resultObject.insert(QStringLiteral("errors"), errorArray.at(0));
				hasResult = true;
			}
		}

		// Standard data
		if (!hasResult){
			const QJsonValue dataValue = rootObject.value(QStringLiteral("data"));
			if (dataValue.isObject()){
				const QJsonObject dataObject = dataValue.toObject();
				const QJsonValue bodyValue = dataObject.value(requestPtr->GetCommandId());
				if (!bodyValue.isUndefined() && !bodyValue.isNull()){
					resultObject.insert(QStringLiteral("data"), bodyValue);
					hasResult = true;
				}
			}
		}

		// Generic payload fallback
		if (!hasResult){
			const QJsonValue payloadValue = rootObject.value(QStringLiteral("payload"));
			if (payloadValue.isObject()){
				resultObject = payloadValue.toObject();
				hasResult = !resultObject.isEmpty();
			}
		}
	}

	return resultObject;
}


} // namespace imtclientgql


