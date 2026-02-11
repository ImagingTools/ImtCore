// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IUndoManager.h>

// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


class CDocumentUndoRedoChangedEvent : public CEventBase
{
public:
	CDocumentUndoRedoChangedEvent() = delete;
	CDocumentUndoRedoChangedEvent(
		const QByteArray& userId,
		const QByteArray& documentId,
		const QByteArray& documentTypeId,
		const QString& documentName,
		const QUrl& documentUrl,
		bool isDirty,
		const idoc::IUndoManager& undoManager);
	const idoc::IUndoManager& GetUndoManager();

	// reimplemented (CEventBase)
	virtual QByteArray Type() const override;

private:
	const idoc::IUndoManager& m_undoManager;
};


} // namespace imtdoc


