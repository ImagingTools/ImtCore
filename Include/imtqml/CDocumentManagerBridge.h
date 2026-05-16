// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtqml/IDocumentManagerBridge.h>


namespace imtqml
{


/**
	\brief In-process implementation of \c IDocumentManagerBridge.

	\details
	An icomp component that owns an \c I_REF to an
	\c imtdoc::IDocumentService and forwards every
	\c IDocumentManagerBridge call directly to it. Intended for the
	server-side / in-process scenario where a GraphQL client is not
	used.

	\note Bridge is intentionally NOT exposed to QML — clients
	(\c CDocumentManagerController) resolve it through icomp.
*/
class CDocumentManagerBridge:
			public icomp::CComponentBase,
			virtual public IDocumentManagerBridge
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDocumentManagerBridge);
		I_REGISTER_INTERFACE(IDocumentManagerBridge);
		I_ASSIGN(m_documentManagerCompPtr, "DocumentManager", "In-process document manager", true, "DocumentManager");
	I_END_COMPONENT;

	~CDocumentManagerBridge() override;

	/**
		\brief Returns the most recently created instance of this
		bridge or \c nullptr if none exists.

		\details Mirrors the lookup pattern used by
		\c CGqlClientBridge::Instance() so a QML-facing controller
		(\c CDocumentManagerController) can resolve the bridge without
		knowing about icomp. The instance is registered in
		\c OnComponentCreated() and unregistered in the destructor.
	*/
	static CDocumentManagerBridge* Instance();

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

public:
	// reimplemented (IDocumentManagerBridge)
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

	static CDocumentManagerBridge* s_instancePtr;
};


} // namespace imtqml
