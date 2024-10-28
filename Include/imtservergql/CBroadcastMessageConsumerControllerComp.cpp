#include <imtservergql/CBroadcastMessageConsumerControllerComp.h>


// ACF includes
#include <ilog/CMessage.h>


namespace imtservergql
{


// reimplemented (imtgql::IGqlRepresentationDataController)

// public methods


// reimplemented (imtgql::IGqlRequestHandler)

imtbase::CTreeItemModel* CBroadcastMessageConsumerControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	Q_ASSERT(IsRequestSupported(gqlRequest));

	if (!IsRequestSupported(gqlRequest)){
		SendErrorMessage(0, QString("GQL handler is not supported GQL Request with command %1").arg(qPrintable(gqlRequest.GetCommandId())));

		return nullptr;
	}


	if (!m_messageConsumerCompPtr.IsValid()){
		errorMessage = QString("Unable to send broadcast message. Component reference 'MessageConsumer' was not set");

		SendCriticalMessage(0, errorMessage);

		return nullptr;
	}

	QList<imtgql::CGqlObject> inputParams;
	inputParams.append(gqlRequest.GetParams());

	QString source;
	QString messageText;
	if (inputParams.size() > 0){
		source = inputParams.at(0).GetFieldArgumentValue("source").toByteArray();
		messageText = inputParams.at(0).GetFieldArgumentValue("message").toString();
	}

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


