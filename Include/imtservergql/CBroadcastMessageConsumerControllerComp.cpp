// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CBroadcastMessageConsumerControllerComp.h>

// Qt includes
#include <QtCore/QJsonObject>

// ACF includes
#include <ilog/CMessage.h>


namespace imtservergql
{


// public methods

// reimplemented (imtgql::IGqlRequestHandler)

QJsonObject CBroadcastMessageConsumerControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	Q_ASSERT(IsRequestSupported(gqlRequest));

	if (!IsRequestSupported(gqlRequest)){
		SendErrorMessage(0, QString("GQL handler is not supported GQL Request with command '%1'").arg(gqlRequest.GetCommandId()));

		return QJsonObject();
	}

	if (!m_messageConsumerCompPtr.IsValid()){
		errorMessage = QString("Unable to send broadcast message. Component reference 'MessageConsumer' was not set");
		SendCriticalMessage(0, errorMessage);

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputObjectPtr = gqlRequest.GetParamObject("input");
	if (inputObjectPtr == nullptr){
		errorMessage = QString("Unable to send broadcast message. GraphQL input params is invalid");
		SendErrorMessage(0, errorMessage);

		return QJsonObject();
	}

	QString source = inputObjectPtr->GetParamArgumentValue("source").toByteArray();
	QString messageText = inputObjectPtr->GetParamArgumentValue("message").toString();

	QJsonObject rootObj;
	QJsonObject dataObj;

	ilog::CMessage* message = new ilog::CMessage();
	message->SetSource(source);
	message->SetText(messageText);

	ilog::IMessageConsumer::MessagePtr broadcastMessage(message);

	m_messageConsumerCompPtr->AddMessage(broadcastMessage);
	dataObj.insert(QStringLiteral("successful"), QStringLiteral("true"));
	rootObj.insert(QStringLiteral("data"), dataObj);

	return rootObj;
}


} // namespace imtservergql


