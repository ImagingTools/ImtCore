// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CGqlRemoteRepresentationControllerCompBase.h>


// ImtCore includes
#include <imtgql/CGqlResponse.h>


namespace imtclientgql
{


// protected methods

// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CGqlRemoteRepresentationControllerCompBase::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!IsRequestSupported(gqlRequest)){
		return nullptr;
	}

	if (!m_apiClientCompPtr.IsValid()){
		SendCriticalMessage(0, "Component 'ApiClient' was not set. Check the component configuration");

		return nullptr;
	}

	imtclientgql::IGqlClient::GqlRequestPtr requestPtr;
	requestPtr.MoveCastedPtr(gqlRequest.CloneMe());
	if (requestPtr.IsValid()){
		imtclientgql::IGqlClient::GqlResponsePtr responsePtr = m_apiClientCompPtr->SendRequest(requestPtr);
		if (responsePtr.IsValid()){
			return CreateTreeItemModelFromResponse(gqlRequest.GetCommandId(), *responsePtr);
		}

		errorMessage = QString("Command could not be processed by the remote server: '%1'").arg(qPrintable(requestPtr->GetCommandId()));
	}

	return nullptr;
}


// private methods

imtbase::CTreeItemModel* CGqlRemoteRepresentationControllerCompBase::CreateTreeItemModelFromResponse(
			const QByteArray& commandId,
			const imtgql::IGqlResponse& response) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> retVal(new imtbase::CTreeItemModel());

	const imtgql::IGqlResponse::GqlRequestPtr requestPtr = response.GetOriginalRequest();
	if (!requestPtr.IsValid()){
		return nullptr;
	}

	const QByteArray responseData = response.GetResponseData();
	const QJsonDocument document = QJsonDocument::fromJson(responseData);
	if (!document.isObject()){
		return retVal.PopPtr();
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

	if (hasResult){
		const QByteArray parserData = QJsonDocument(resultObject).toJson(QJsonDocument::Compact);
		retVal->CreateFromJson(parserData);
	}

	return retVal.PopPtr();
}


} // namespace imtclientgql


