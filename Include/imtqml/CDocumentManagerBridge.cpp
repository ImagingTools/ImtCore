// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CDocumentManagerBridge.h>


namespace imtqml
{


namespace
{


/**
	\brief Stable diagnostic returned by every method until direct
	\c imtdoc::IDocumentManager calls are wired in this bridge.
*/
const QString c_notImplementedError = QStringLiteral(
		"CDocumentManagerBridge: in-process IDocumentManager forwarding "
		"not yet wired in this bridge");


} // anonymous namespace


CDocumentManagerBridge::CDocumentManagerBridge() = default;


CDocumentManagerBridge::~CDocumentManagerBridge() = default;


// reimplemented (IDocumentDataBridge)

void CDocumentManagerBridge::GetOpenedDocumentList(
		const QString& /*collectionId*/,
		DocumentListCallback callback)
{
	if (callback){
		callback({}, c_notImplementedError);
	}
}


void CDocumentManagerBridge::OpenDocument(
		const QString& /*collectionId*/,
		const QString& /*typeId*/,
		const QString& /*documentId*/,
		DocumentCallback callback)
{
	if (callback){
		callback(OpenedDocumentInfo{}, c_notImplementedError);
	}
}


void CDocumentManagerBridge::CreateDocument(
		const QString& /*collectionId*/,
		const QString& /*typeId*/,
		DocumentCallback callback)
{
	if (callback){
		callback(OpenedDocumentInfo{}, c_notImplementedError);
	}
}


void CDocumentManagerBridge::SaveDocument(
		const QString& /*collectionId*/,
		const QString& /*documentId*/,
		const QString& /*documentName*/,
		StatusCallback callback)
{
	if (callback){
		callback(OS_FAILED, c_notImplementedError);
	}
}


void CDocumentManagerBridge::CloseDocument(
		const QString& /*collectionId*/,
		const QString& /*documentId*/,
		VoidCallback callback)
{
	if (callback){
		callback(c_notImplementedError);
	}
}


void CDocumentManagerBridge::DoUndo(
		const QString& /*collectionId*/,
		const QString& /*documentId*/,
		int /*steps*/,
		VoidCallback callback)
{
	if (callback){
		callback(c_notImplementedError);
	}
}


void CDocumentManagerBridge::DoRedo(
		const QString& /*collectionId*/,
		const QString& /*documentId*/,
		int /*steps*/,
		VoidCallback callback)
{
	if (callback){
		callback(c_notImplementedError);
	}
}


void CDocumentManagerBridge::ResetUndo(
		const QString& /*collectionId*/,
		const QString& /*documentId*/,
		VoidCallback callback)
{
	if (callback){
		callback(c_notImplementedError);
	}
}


void CDocumentManagerBridge::GetUndoInfo(
		const QString& /*collectionId*/,
		const QString& /*documentId*/,
		UndoInfoCallback callback)
{
	if (callback){
		callback(0, 0, false, c_notImplementedError);
	}
}


} // namespace imtqml
