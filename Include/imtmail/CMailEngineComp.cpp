#include "imtmail/CMailEngineComp.h"


// ImtCore includes
#include <imtmail/CMailEngineComp.h>
#include <imtmail/CSmtpMailSendRequest.h>


namespace imtmail
{


CMailEngineComp::CMailEngineComp()
	:m_mailSettingsObserver(*this)
{
}


void CMailEngineComp::OnMailSettingsChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const IMailSettings* mailSettingsPtr)
{
	Q_ASSERT(mailSettingsPtr != nullptr);
	if (mailSettingsPtr != nullptr){
		m_workingMailSettings.CopyFrom(*mailSettingsPtr);
	}
}


// reimplemented (icomp::CComponentBase)

void CMailEngineComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_mailSettingsObserver.RegisterObject(m_mailSettingsCompPtr.GetPtr(), &CMailEngineComp::OnMailSettingsChanged);
	if (m_objectCollectionCompPtr.IsValid() && m_objectCollectionModelCompPtr.IsValid()){
		m_objectCollectionModelCompPtr->AttachObserver(this);
	}
	connect(this, &CMailEngineComp::SendEmailSignal, this, &CMailEngineComp::SendEmailSlot);
}


void CMailEngineComp::OnComponentDestroyed()
{
	if (m_objectCollectionCompPtr.IsValid() && m_objectCollectionModelCompPtr.IsValid()){

		if (m_objectCollectionModelCompPtr->IsAttached(this)){
			m_objectCollectionModelCompPtr->DetachObserver(this);
		}
	}

	m_mailSettingsObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CMailEngineComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	SendEmailSignal();
}


void CMailEngineComp::SendEmail(const QString& destination, const QString& subject, const QString& body, QStringList files, QHash<QString, QByteArray> binaryData)
{
	CSmtpMailSendRequest req;
	req.SetLogLogger(*this);
	req.SetVerboseEnabled(IsVerboseEnabled());
	req.SetUser(m_mailSettingsCompPtr->GetUserName());
	req.SetPassword(m_mailSettingsCompPtr->GetPassword());
	req.SetHost(m_mailSettingsCompPtr->GetHost());
	req.SetMailPort(m_mailSettingsCompPtr->GetPort());
	req.SetTimeout(m_mailSettingsCompPtr->GetRequestTimeout());
	req.SetSenderAddress(m_mailSettingsCompPtr->GetSenderAddress());
	req.SetIgnoreAllSslErrors(m_mailSettingsCompPtr->GetIgnoreSslErrors());
	req.SetSubject(subject);
	req.SetBody(body);
	req.SetFiles(files);
	req.SetDestination(destination);
	for(const QString& key: binaryData.keys()){
		req.SetBinaryData(key, binaryData[key]);
	}
	m_objectCollectionCompPtr->InsertNewObject("Smtp", "Smtp", QString(), &req);
}


void CMailEngineComp::SendEmailSlot()
{
	imtbase::IObjectCollection* collection = imod::TSingleModelObserverBase<imtbase::IObjectCollection>::GetObservedObject();

	if (collection != nullptr){

		imtbase::IObjectCollection::Ids elementIds = collection->GetElementIds();

		for (const QByteArray& id: ::qAsConst(elementIds)){

			const ISmtpMailSendRequest* mailRequest = dynamic_cast<const ISmtpMailSendRequest*>(collection->GetObjectPtr(id));

			if (mailRequest != nullptr){
				qDebug() << mailRequest->GetState();
				// Looking for request that ready to be proceed
				if (mailRequest->GetState() & ISmtpMailSendRequest::RequestState::RS_READY_TO_PROCESSING){
					mailRequest->SendMail();
				}
				qDebug() << mailRequest->GetState();
				if(mailRequest->GetState() & ISmtpMailSendRequest::RequestState::RS_FINISHED){
					m_objectCollectionCompPtr->RemoveElement(id);
				}
			}
		}
	}
}


}
