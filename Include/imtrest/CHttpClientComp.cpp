#include <imtrest/CHttpClientComp.h>


// Qt includes
#include <QtCore/QObject>


namespace imtrest
{


// reimplemented (icomp::CComponentBase)

void CHttpClientComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_objectCollectionCompPtr.IsValid() && m_objectCollectionModelCompPtr.IsValid()){
		m_objectCollectionModelCompPtr->AttachObserver(this);
	}

	// fill the local url-template
	if (m_urlAttrPtr.IsValid()){
		m_baseUrl = QUrl(*m_urlAttrPtr);
	}
	else{
		QUrl url;

		if (m_urlFragmentAttrPtr.IsValid()){
			url.setFragment(*m_urlFragmentAttrPtr);
		}

		if (m_urlHostAttrPtr.IsValid()){
			url.setFragment(*m_urlFragmentAttrPtr);
		}

		if (m_urlPasswordAttrPtr.IsValid()){
			url.setPassword(*m_urlPasswordAttrPtr);
		}

		if (m_urlPortAttrPtr.IsValid()){
			url.setPort(*m_urlPortAttrPtr);
		}

		if (m_urlSchemeAttrPtr.IsValid()){
			url.setScheme(*m_urlSchemeAttrPtr);
		}

		if (m_urlUserNameAttrPtr.IsValid()){
			url.setUserName(*m_urlUserNameAttrPtr);
		}

		m_baseUrl = url;
	}

}


void CHttpClientComp::OnComponentDestroyed()
{
	if (m_objectCollectionCompPtr.IsValid() && m_objectCollectionModelCompPtr.IsValid()){

		if (m_objectCollectionModelCompPtr->IsAttached(this)){
			m_objectCollectionModelCompPtr->DetachObserver(this);
		}
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CHttpClientComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	/// The collection type name
	using L_CT = imtbase::IObjectCollection;

	L_CT* collection = imod::TSingleModelObserverBase<L_CT>::GetObservedObject();

	if (collection != nullptr){

		L_CT::Ids elementIds = collection->GetElementIds();

		for (const QByteArray& id: ::qAsConst(elementIds)){

			const IHttpClientRequest* httpRequest = dynamic_cast<const IHttpClientRequest*>(collection->GetObjectPtr(id));

			if (httpRequest != nullptr){
				// Looking for request that ready to be proceed
				if (httpRequest->GetState() & IHttpClientRequest::RequestState::RS_READY_TO_PROCESSING){

					//Getting QNetworkRequest and fill url from local template
					QNetworkRequest request = httpRequest->GetRequest();
					QUrl url = request.url();

					if (!url.scheme().length() && m_baseUrl.scheme().length()){
						url.setScheme(m_baseUrl.scheme());
					}

					if (!url.userName().length() && m_baseUrl.userName().length()){
						url.setScheme(m_baseUrl.scheme());
					}

					if (!url.password().length() && m_baseUrl.password().length()){
						url.setUserName(m_baseUrl.userName());
					}

					if (!url.host().length() && m_baseUrl.host().length()){
						url.setHost(m_baseUrl.host());
					}

					if (url.port() <=0 && m_baseUrl.port() > 0){
						url.setPort(m_baseUrl.port());
					}

					if (m_urlPrePathAttrPtr.IsValid() && m_urlPrePathAttrPtr->GetValue().length()){

						QString path = url.path();
						path.prepend(m_urlPrePathAttrPtr->GetValue());
						url.setPath(path);

					}

					if (m_urlPostPathAttrPtr.IsValid() && m_urlPrePathAttrPtr->GetValue().length()){

						QString path = url.path();
						path.append(m_urlPostPathAttrPtr->GetValue());
						url.setPath(path);

					}

					if (!url.fragment().length() && m_baseUrl.fragment().length()){
						url.setFragment(m_baseUrl.fragment());
					}

					request.setUrl(url);

					// Getting request type and sending the request via network  and update the requset for notifying other observers
					QNetworkAccessManager::Operation requestType = httpRequest->GetRequestType();

					switch (requestType){

					case QNetworkAccessManager::Operation::GetOperation:
						const_cast<IHttpClientRequest*>(httpRequest)->SetReply(m_networkAccessManager.get(request));
						break;
					case QNetworkAccessManager::Operation::PostOperation:
						request.setHeader(QNetworkRequest::KnownHeaders::ContentLengthHeader, httpRequest->GetRequestBody().length());

						if(!request.header(QNetworkRequest::KnownHeaders::ContentTypeHeader).toByteArray().length()){
							request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "octed-stream; charset=utf-8");
						}

						const_cast<IHttpClientRequest*>(httpRequest)->SetReply(m_networkAccessManager.post(request, httpRequest->GetRequestBody()));
						break;

					default:
						qCritical() << __FILE__ << __LINE__
									<< "\n\t | Unsupported operation detected"
									<< "\n\t | requestType" << requestType
									   ;
						break;
					}
				}
			}
		}
	}
}




} // namespace imtrest



