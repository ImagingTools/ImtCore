// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtclientgql/IGqlClient.h>
#include <imtqml/IDocumentDataBridge.h>


namespace imtqml
{


/**
	\brief GraphQL/SDL implementation of \c IDocumentDataBridge.

	\details
	An icomp component that owns an \c I_REF to an
	\c imtclientgql::IGqlClient and translates every
	\c IDocumentDataBridge call into the corresponding SDL-generated
	GraphQL request from
	\c Sdl/imtbase/1.0/CollectionDocumentManager.sdl. Operations are
	dispatched on a worker thread; results are marshalled to the GUI
	thread before invoking the supplied callback.

	\note Bridge is intentionally NOT exposed to QML — clients
	(\c CDocumentDataController) resolve it through icomp.
*/
class CGqlDocumentManagerBridge:
			public icomp::CComponentBase,
			virtual public IDocumentDataBridge
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlDocumentManagerBridge);
		I_REGISTER_INTERFACE(IDocumentDataBridge);
		I_ASSIGN(m_apiClientCompPtr, "ApiClient", "GraphQL API client", true, "ApiClient");
	I_END_COMPONENT;

	CGqlDocumentManagerBridge();
	~CGqlDocumentManagerBridge() override;

	// reimplemented (IDocumentDataBridge)
	virtual void GetOpenedDocumentList(
			const QString& collectionId,
			TDocumentListCallback callback) override;

	virtual void OpenDocument(
			const QString& collectionId,
			const QString& typeId,
			const QString& documentId,
			TDocumentCallback callback) override;

	virtual void CreateDocument(
			const QString& collectionId,
			const QString& typeId,
			TDocumentCallback callback) override;

	virtual void SaveDocument(
			const QString& collectionId,
			const QString& documentId,
			const QString& documentName,
			TStatusCallback callback) override;

	virtual void CloseDocument(
			const QString& collectionId,
			const QString& documentId,
			TVoidCallback callback) override;

	virtual void DoUndo(
			const QString& collectionId,
			const QString& documentId,
			int steps,
			TVoidCallback callback) override;

	virtual void DoRedo(
			const QString& collectionId,
			const QString& documentId,
			int steps,
			TVoidCallback callback) override;

	virtual void ResetUndo(
			const QString& collectionId,
			const QString& documentId,
			TVoidCallback callback) override;

	virtual void GetUndoInfo(
			const QString& collectionId,
			const QString& documentId,
			TUndoInfoCallback callback) override;

private:
	I_REF(imtclientgql::IGqlClient, m_apiClientCompPtr);
};


} // namespace imtqml
