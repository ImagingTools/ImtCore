// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IUndoManager.h>

// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Event fired when the undo/redo availability of a document changes.

	Produced by \c CDocumentServiceBase::OnUndoManagerChanged when the
	undo-manager's dirty flag transitions (i.e.\ when the user performs an
	undoable edit, or after a save resets the baseline state).  The
	referenced \c idoc::IUndoManager can be queried to determine whether
	undo and/or redo actions are currently available.

	\note The referenced undo manager is only valid for the duration of the
	      \c ProcessEvent call.
*/
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


