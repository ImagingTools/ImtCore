#include <imtdoc/IDocumentManager.h>


namespace imtdoc
{


const QByteArray IDocumentManager::CN_NEW_DOCUMENT_CREATED = QByteArrayLiteral("imtdoc:IDocumentManager::NewDocumentCreated");
const QByteArray IDocumentManager::CN_DOCUMENT_OPENED = QByteArrayLiteral("imtdoc:IDocumentManager::DocumentOpenedInfo");
const QByteArray IDocumentManager::CN_DOCUMENT_CHANGED = QByteArrayLiteral("imtdoc:IDocumentManager::DocumentChangedInfo");
const QByteArray IDocumentManager::CN_DOCUMENT_UNDO_CHANGED = QByteArrayLiteral("imtdoc:IDocumentManager::DocumentUndoChangedInfo");
const QByteArray IDocumentManager::CN_DOCUMENT_SAVED = QByteArrayLiteral("imtdoc:IDocumentManager::DocumentSavedInfo");
const QByteArray IDocumentManager::CN_DOCUMENT_CLOSED = QByteArrayLiteral("imtdoc:IDocumentManager::DocumentClosedInfo");


} // namespace imtdoc


