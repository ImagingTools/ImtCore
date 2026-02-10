// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CBroadcastMessageConsumerControllerComp.h>


// ACF includes
#include <ilog/CMessage.h>


namespace imtservergql
{


// public methods

// reimplemented (imtgql::IGqlRequestHandler)

imtbase::CTreeItemModel* CBroadcastMessageConsumerControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	Q_ASSERT(IsRequestSupported(gqlRequest));

	if (!IsRequestSupported(gqlRequest)){
		SendErrorMessage(0, QString("GQL handler is not supported GQL Request with command '%1'").arg(qPrintable(gqlRequest.GetCommandId())));

		return nullptr;
	}

	if (!m_messageConsumerCompPtr.IsValid()){
		errorMessage = QString("Unable to send broadcast message. Component reference 'MessageConsumer' was not set");
		SendCriticalMessage(0, errorMessage);

		return nullptr;
	}

	const imtgql::CGqlParamObject* inputObjectPtr = gqlRequest.GetParamObject("input");
	if (inputObjectPtr == nullptr){
		errorMessage = QString("Unable to send broadcast message. GraphQL input params is invalid");
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	QString source = inputObjectPtr->GetParamArgumentValue("source").toByteArray();
	QString messageText = inputObjectPtr->GetParamArgumentValue("message").toString();

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");

	ilog::CMessage* message = new ilog::CMessage();
	message->SetSource(source);
	message->SetText(messageText);

	ilog::IMessageConsumer::MessagePtr broadcastMessage(message);

	m_messageConsumerCompPtr->AddMessage(broadcastMessage);
	dataModel->SetData("successful", "true");

	return rootModelPtr.PopPtr();
}


} // namespace imtservergql


