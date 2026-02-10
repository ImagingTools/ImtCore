// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/IDocumentManager.h>
#include <imtdoc/IDocumentManagerEventHandler.h>
#include <imtservergql/CGqlPublisherCompBase.h>

// ImtCore includes
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/CollectionDocumentManager.h>


namespace imtservergql
{


class CCollectionDocumentManagerPublisherComp:
	public CGqlPublisherCompBase,
	virtual public imtdoc::IDocumentManagerEventHandler
{
public:
	typedef CGqlPublisherCompBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionDocumentManagerPublisherComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentManagerEventHandler)
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "Collection ID", true, "DummyCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (imtdoc::IDocumentManagerEventHandler)
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

protected:
	void FillDocumentNotification(
		const imtdoc::CEventBase* eventPtr,
		imtdoc::IDocumentManager::DocumentNotification& notification) const;
	void FillSdlNotification(
		const imtdoc::IDocumentManager::DocumentNotification& notification,
		sdl::imtbase::CollectionDocumentManager::EDocumentOperation operation,
		sdl::imtbase::CollectionDocumentManager::CDocumentManagerNotification::V1_0& sdlNotification) const;
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
void CCollectionDocumentManagerPublisherComp::PublishRepresentation(
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
			PublishData(commandId, data);
		}
	}
}


} // namespace imtservergql


