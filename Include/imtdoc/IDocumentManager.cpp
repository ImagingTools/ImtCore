// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/IDocumentManager.h>


namespace imtdoc
{


const QByteArray IDocumentManager::CN_NEW_DOCUMENT_CREATED = QByteArrayLiteral("imtdoc:IDocumentManager::NewDocumentCreated");
const QByteArray IDocumentManager::CN_DOCUMENT_OPENED = QByteArrayLiteral("imtdoc:IDocumentManager::DocumentOpened");
const QByteArray IDocumentManager::CN_DOCUMENT_RENAMED = QByteArrayLiteral("imtdoc:IDocumentManager::DocumentRenamed");
const QByteArray IDocumentManager::CN_DOCUMENT_CHANGED = QByteArrayLiteral("imtdoc:IDocumentManager::DocumentChanged");
const QByteArray IDocumentManager::CN_DOCUMENT_UNDO_CHANGED = QByteArrayLiteral("imtdoc:IDocumentManager::DocumentUndoChanged");
const QByteArray IDocumentManager::CN_DOCUMENT_SAVED = QByteArrayLiteral("imtdoc:IDocumentManager::DocumentSaved");
const QByteArray IDocumentManager::CN_DOCUMENT_SAVED_AS = QByteArrayLiteral("imtdoc:IDocumentManager::DocumentSavedAs");
const QByteArray IDocumentManager::CN_DOCUMENT_CLOSED = QByteArrayLiteral("imtdoc:IDocumentManager::DocumentClosed");


} // namespace imtdoc


