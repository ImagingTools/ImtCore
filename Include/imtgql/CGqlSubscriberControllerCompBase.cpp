#include <imtgql/CGqlSubscriberControllerCompBase.h>


// ACF includes
#include<istd/TDelPtr.h>
#include<imod/IModel.h>

// ImtCore includes
#include<imtrest/IProtocolEngine.h>
#include<imtrest/ISender.h>
#include<imtrest/CWebSocketRequest.h>


namespace imtgql
{


// public methods
CGqlSubscriberControllerCompBase::CGqlSubscriberControllerCompBase()
	:BaseClass()
{
}


// reimplemented (imtgql::IGqlRequestHandler)

bool CGqlSubscriberControllerCompBase::IsRequestSupported(const CGqlRequest &gqlRequest) const
{
	const QList<CGqlObject> fieldsPtr = gqlRequest.GetFields();
	if (fieldsPtr.isEmpty()){
		return false;
	}

	QByteArray commandId = gqlRequest.GetCommandId();

	return m_commandIdsAttrPtr.FindValue(commandId) != -1;
}


bool CGqlSubscriberControllerCompBase::RegisterSubscribtion(
			const QByteArray& subscriptionId,
			const imtgql::CGqlRequest& gqlRequest,
			const imtrest::IRequest& networkRequest,
			QString& errorMessage)
{
	Q_ASSERT(IsRequestSupported(gqlRequest));

	if (!IsRequestSupported(gqlRequest)){
		errorMessage = QT_TR_NOOP("Request is not supported");
		return false;
	}

	if(CheckPermissions(gqlRequest, errorMessage)){
		const imtrest::CWebSocketRequest* constWebSocketRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&networkRequest);
		imtrest::CWebSocketRequest* webSocketRequest = dynamic_cast<imtrest::CWebSocketRequest*>(const_cast<imtrest::CWebSocketRequest*>(constWebSocketRequest));
		if (webSocketRequest == nullptr){
			return false;
		}

		for (RequestNetworks& requestNetworks: m_registeredSubscribers){
			if (requestNetworks.gqlRequest.IsEqual(gqlRequest)){
				requestNetworks.networkRequests.insert(subscriptionId, &networkRequest);
				webSocketRequest->RegisterDestroyObserver(this);

				return true;
			}
		}

		RequestNetworks requestNetworks;
		requestNetworks.gqlRequest.CopyFrom(gqlRequest);
		requestNetworks.networkRequests.insert(subscriptionId, &networkRequest);
		m_registeredSubscribers.append(requestNetworks);
		webSocketRequest->RegisterDestroyObserver(this);

		return true;
	}

	QString userName;
	if (gqlRequest.GetRequestContext() != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlRequest.GetRequestContext()->GetUserInfo();

		if (userInfoPtr != nullptr){
			userName = userInfoPtr->GetName();
		}
	}

	errorMessage = QT_TR_NOOP("Invalid permissions for " + userName);

	return false;
}


bool CGqlSubscriberControllerCompBase::UnRegisterSubscribtion(const QByteArray& subscriptionId) 
{
	for (RequestNetworks& requestNetworks: m_registeredSubscribers){
		if (requestNetworks.networkRequests.contains(subscriptionId)){
			requestNetworks.networkRequests.remove(subscriptionId);

			return true;
		}
	}

	return false;
}


void CGqlSubscriberControllerCompBase::OnRequestDestroyed(imtrest::CWebSocketRequest* webSocketRequest)
{
	if (webSocketRequest != nullptr){
		UnRegisterSubscribtion(webSocketRequest->GetSubscriptionId());
	}
}


// protected methods

bool CGqlSubscriberControllerCompBase::SetSubscriptions()
{
	if (*m_requestHandlerCommandIdAtrPtr == ""){
		return false;
	}

	if (!m_requestHandlerCompPtr.IsValid()){
		return false;
	}

	if (!m_requestManagerCompPtr.IsValid()){
		return false;
	}

	for (RequestNetworks& requestNetworks: m_registeredSubscribers){
		imtgql::CGqlRequest clonedRequest;
		clonedRequest.CopyFrom(requestNetworks.gqlRequest);
		clonedRequest.SetRequestType(imtgql::CGqlRequest::RT_QUERY);
		clonedRequest.SetCommandId(*m_requestHandlerCommandIdAtrPtr);

		QString errorMessage;
		istd::TDelPtr<imtbase::CTreeItemModel> resultModel = m_requestHandlerCompPtr->CreateResponse(clonedRequest, errorMessage);

		for (QByteArray id: requestNetworks.networkRequests.keys()){
			const imtrest::IRequest* networkRequest = requestNetworks.networkRequests[id];
			imtbase::CTreeItemModel* dataModel = resultModel->GetTreeItemModel("data");
			if (dataModel == nullptr){
				continue;
			}
			QByteArray data =  dataModel->toJSON().toUtf8();
			QByteArray body = QString(R""(
{
	"type": "data",
	"id": %1,
	"payload": {
		"data": %2
	}
}
	)"" ).arg(QString(id)).arg(QString(data)).toUtf8();
			QByteArray reponseTypeId = QByteArray("application/json; charset=utf-8");
			const imtrest::IProtocolEngine&  engine = networkRequest->GetProtocolEngine();

			 imtrest::ConstResponsePtr responsePtr(engine.CreateResponse(*networkRequest, imtrest::IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId));
			if (responsePtr.IsValid()){
				const imtrest::ISender* sender = m_requestManagerCompPtr->GetSender(networkRequest->GetRequestId());
				if (sender != nullptr){
					sender->SendResponse(responsePtr);
				}
			}
		}
 	}

	return true;
}


bool CGqlSubscriberControllerCompBase::StartInternalSubscriber(const imtgql::CGqlRequest& /*gqlRequest*/, QString& /*errorMessage*/)
{
	return true;
}


iprm::IParamsSet *CGqlSubscriberControllerCompBase::CreateContextParams(const imtgql::CGqlRequest& /*gqlRequest*/) const
{
	return nullptr;
}


bool CGqlSubscriberControllerCompBase::CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& /*errorMessage*/) const
{
	bool result = true;
	if (gqlRequest.GetRequestContext() != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlRequest.GetRequestContext()->GetUserInfo();
		Q_ASSERT(userInfoPtr != nullptr);
		if (userInfoPtr != nullptr){
			QByteArray userId = userInfoPtr->GetId();
			if (!userInfoPtr->IsAdmin()){
				if(m_commandPermissionsCompPtr.IsValid()){
					imtauth::IUserInfo::FeatureIds permissions = userInfoPtr->GetPermissions();
					QByteArray gqlCommand = gqlRequest.GetCommandId();
					QByteArrayList commandIds = m_commandPermissionsCompPtr->GetCommandIds();
					if(commandIds.contains(gqlCommand)){
						QByteArrayList permissionIds = m_commandPermissionsCompPtr->GetCommandPermissions(gqlCommand);
						if (m_checkPermissionCompPtr.IsValid()){
							result = m_checkPermissionCompPtr->CheckPermission(permissions, permissionIds);
						}
					}
				}
			}
		}
	}

	return result;
}


} // namespace imtgql


