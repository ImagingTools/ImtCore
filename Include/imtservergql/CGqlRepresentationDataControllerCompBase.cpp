// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CGqlRepresentationDataControllerCompBase.h>

// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>


namespace imtservergql
{


// protected methods

QJsonObject CGqlRepresentationControllerCompBase::CreateRepresentationFromRequest(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return QJsonObject();
}


bool CGqlRepresentationControllerCompBase::UpdateModelFromRepresentation(
			const imtgql::CGqlRequest& /*request*/,
			const QJsonObject& /*representation*/) const
{
	return false;
}


// reimplemented (imtservergql::CGqlRequestHandlerCompBase)

QJsonObject CGqlRepresentationControllerCompBase::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QByteArray commandId = gqlRequest.GetCommandId();

	imtgql::IGqlRequest::RequestType requestType = gqlRequest.GetRequestType();

	if (requestType == imtgql::IGqlRequest::RT_QUERY){
		return CreateRepresentationFromRequest(gqlRequest, errorMessage);
	}
	if (requestType == imtgql::IGqlRequest::RT_MUTATION){
		const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
		if (inputParamPtr == nullptr){
			return QJsonObject();
		}

		QByteArray itemData = inputParamPtr->GetParamArgumentValue("Item").toByteArray();
		if (!itemData.isEmpty()){
			QJsonDocument doc = QJsonDocument::fromJson(itemData);
			if (!doc.isNull() && doc.isObject()){
				QJsonObject representation = doc.object();
				bool result = UpdateModelFromRepresentation(gqlRequest, representation);
				if (result){
					return representation;
				}
			}
		}
	}

	return QJsonObject();
}


} // namespace imtservergql


