// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/IDocumentService.h>


namespace imtdoc
{


const QByteArray IDocumentService::CN_NEW_DOCUMENT_CREATED = QByteArrayLiteral("imtdoc:IDocumentService::NewDocumentCreated");
const QByteArray IDocumentService::CN_DOCUMENT_OPENED = QByteArrayLiteral("imtdoc:IDocumentService::DocumentOpened");
const QByteArray IDocumentService::CN_DOCUMENT_RENAMED = QByteArrayLiteral("imtdoc:IDocumentService::DocumentRenamed");
const QByteArray IDocumentService::CN_DOCUMENT_CHANGED = QByteArrayLiteral("imtdoc:IDocumentService::DocumentChanged");
const QByteArray IDocumentService::CN_DOCUMENT_UNDO_CHANGED = QByteArrayLiteral("imtdoc:IDocumentService::DocumentUndoChanged");
const QByteArray IDocumentService::CN_DOCUMENT_SAVED = QByteArrayLiteral("imtdoc:IDocumentService::DocumentSaved");
const QByteArray IDocumentService::CN_DOCUMENT_SAVED_AS = QByteArrayLiteral("imtdoc:IDocumentService::DocumentSavedAs");
const QByteArray IDocumentService::CN_DOCUMENT_CLOSED = QByteArrayLiteral("imtdoc:IDocumentService::DocumentClosed");
const QByteArray IDocumentService::CN_DOCUMENT_DATA_LOADED = QByteArrayLiteral("imtdoc:IDocumentService::DocumentDataLoaded");


} // namespace imtdoc


