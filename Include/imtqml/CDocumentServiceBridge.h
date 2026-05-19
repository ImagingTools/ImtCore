// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtqml/IDocumentServiceBridge.h>


namespace imtqml
{


/**
	\brief In-process implementation of \c IDocumentServiceBridge.

	\details
	An icomp component that owns an \c I_REF to an
	\c imtdoc::IDocumentService and forwards every
	\c IDocumentServiceBridge call directly to it. Intended for the
	server-side / in-process scenario where a GraphQL client is not
	used.

	\note Bridge is intentionally NOT exposed to QML — clients
	(\c CDocumentServiceController) resolve it through icomp.
*/
class CDocumentServiceBridge:
			public icomp::CComponentBase,
			virtual public IDocumentServiceBridge
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDocumentServiceBridge);
		I_REGISTER_INTERFACE(IDocumentServiceBridge);
		I_ASSIGN(m_documentManagerCompPtr, "DocumentService", "In-process document manager", true, "DocumentService");
	I_END_COMPONENT;

	~CDocumentServiceBridge() override;

	/**
		\brief Returns the most recently created instance of this
		bridge or \c nullptr if none exists.

		\details Mirrors the lookup pattern used by
		\c CGqlClientBridge::Instance() so a QML-facing controller
		(\c CDocumentServiceController) can resolve the bridge without
		knowing about icomp. The instance is registered in
		\c OnComponentCreated() and unregistered in the destructor.
	*/
	static CDocumentServiceBridge* Instance();

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

public:
	// reimplemented (IDocumentServiceBridge)
	virtual void GetOpenedDocumentList(
			const QString& collectionId,
			DocumentListCallback callback) override;

	virtual void OpenDocument(
			const QString& collectionId,
			const QString& typeId,
			const QString& documentId,
			DocumentCallback callback) override;

	virtual void CreateDocument(
			const QString& collectionId,
			const QString& typeId,
			DocumentCallback callback) override;

	virtual void SaveDocument(
			const QString& collectionId,
			const QString& documentId,
			const QString& documentName,
			StatusCallback callback) override;

	virtual void CloseDocument(
			const QString& collectionId,
			const QString& documentId,
			VoidCallback callback) override;

	virtual void DoUndo(
			const QString& collectionId,
			const QString& documentId,
			int steps,
			VoidCallback callback) override;

	virtual void DoRedo(
			const QString& collectionId,
			const QString& documentId,
			int steps,
			VoidCallback callback) override;

	virtual void ResetUndo(
			const QString& collectionId,
			const QString& documentId,
			VoidCallback callback) override;

	virtual void GetUndoInfo(
			const QString& collectionId,
			const QString& documentId,
			UndoInfoCallback callback) override;

private:
	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);

	static CDocumentServiceBridge* s_instancePtr;
};


} // namespace imtqml
