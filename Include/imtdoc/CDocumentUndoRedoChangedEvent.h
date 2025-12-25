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
		QByteArray userId,
		QByteArray documentId,
		QByteArray documentTypeId,
		QString documentName,
		QUrl documentUrl,
		bool isDirty,
		const idoc::IUndoManager& undoManager);
	const idoc::IUndoManager& GetUndoManager();

	// reimplemented (CEventBase)
	virtual QByteArray Type() const override;

private:
	const idoc::IUndoManager& m_undoManager;
};


} // namespace imtdoc


