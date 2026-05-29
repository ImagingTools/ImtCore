// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtdoc/IDocumentServiceEventHandler.h>
#include <imtservergql/CGqlPublisherCompBase.h>

// ImtCore includes
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/CollectionDocumentService>


namespace imtservergql
{


class CCollectionDocumentServicePublisherComp:
	public CGqlPublisherCompBase,
	virtual public imtdoc::IDocumentServiceEventHandler
{
public:
	typedef CGqlPublisherCompBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionDocumentServicePublisherComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentServiceEventHandler)
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "Collection ID", true, "DummyCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (imtdoc::IDocumentServiceEventHandler)
	virtual bool ProcessEvent(imtdoc::CEventBase* eventPtr) override;

protected:
	virtual bool OnDocumentCreated(imtdoc::CEventBase* eventPtr) const;
	virtual bool OnDocumentOpened(imtdoc::CEventBase* eventPtr) const;
	virtual bool OnDocumentRenamed(imtdoc::CEventBase* eventPtr) const;
	virtual bool OnDocumentChanged(imtdoc::CEventBase* eventPtr) const;
	virtual bool OnDocumentUndoRedoChanged(imtdoc::CEventBase* eventPtr) const;
	virtual bool OnDocumentSaved(imtdoc::CEventBase* eventPtr) const;
	virtual bool OnDocumentSavedAs(imtdoc::CEventBase* eventPtr) const;
	virtual bool OnDocumentClosed(imtdoc::CEventBase* eventPtr) const;
	virtual bool OnDocumentDataLoaded(imtdoc::CEventBase* eventPtr) const;

protected:
	void FillDocumentNotification(
		const imtdoc::CEventBase* eventPtr,
		imtdoc::IDocumentService::DocumentNotification& notification) const;
	void FillSdlNotification(
		const imtdoc::IDocumentService::DocumentNotification& notification,
		sdl::V1_0::imtbase::EDocumentOperation operation,
		sdl::V1_0::imtbase::CDocumentServiceNotification& sdlNotification) const;
	QByteArray ConvertUrlToObjectId(const QUrl& url) const;
	QByteArray GetCommandId() const;

	template<class Representation>
	void PublishRepresentation(
		const QByteArray& commandId,
		const QByteArray& userId,
		const Representation& representation) const;

private:
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
};


// privte methods

template<class Representation>
void CCollectionDocumentServicePublisherComp::PublishRepresentation(
	const QByteArray& commandId,
	const QByteArray& userId,
	const Representation& representation) const
{
	QJsonObject jsonObject;
	if (!representation.WriteToJsonObject(jsonObject)){
		Q_ASSERT(false);
	}

	QJsonDocument jsonDoc;
	jsonDoc.setObject(jsonObject);

	QByteArray data = jsonDoc.toJson(QJsonDocument::Compact);

	for (const RequestNetworks& networkRequest : m_registeredSubscribers){
		if (!userId.isEmpty()){
			const imtgql::IGqlContext* contextPtr = networkRequest.gqlRequest.GetRequestContext();
			if (contextPtr != nullptr){
				const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
				if (userInfoPtr != nullptr){
					if (userInfoPtr->GetId() != userId){
						continue;
					}
				}
			}
		}

		if (commandId == networkRequest.gqlRequest.GetCommandId()){
			for (auto it = networkRequest.networkRequests.constBegin(); it != networkRequest.networkRequests.constEnd(); ++it) {
				PushDataToSubscriber(it.key(), commandId, data, *it.value());
			}
		}
	}
}


} // namespace imtservergql


