// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CGqlDocumentManagerBridge.h>


namespace imtqml
{


namespace
{


/**
	\brief Stable diagnostic returned by every method until the SDL
	transport is moved here from \c CGqlDocumentDataController.
*/
const QString c_notImplementedError = QStringLiteral(
		"CGqlDocumentManagerBridge: SDL transport not yet wired in this bridge - "
		"the corresponding controller still serves the request directly");


} // anonymous namespace


CGqlDocumentManagerBridge::CGqlDocumentManagerBridge() = default;


CGqlDocumentManagerBridge::~CGqlDocumentManagerBridge() = default;


// reimplemented (IDocumentDataBridge)

void CGqlDocumentManagerBridge::GetOpenedDocumentList(
		const QString& /*collectionId*/,
		DocumentListCallback callback)
{
	if (callback){
		callback({}, c_notImplementedError);
	}
}


void CGqlDocumentManagerBridge::OpenDocument(
		const QString& /*collectionId*/,
		const QString& /*typeId*/,
		const QString& /*documentId*/,
		DocumentCallback callback)
{
	if (callback){
		callback(OpenedDocumentInfo{}, c_notImplementedError);
	}
}


void CGqlDocumentManagerBridge::CreateDocument(
		const QString& /*collectionId*/,
		const QString& /*typeId*/,
		DocumentCallback callback)
{
	if (callback){
		callback(OpenedDocumentInfo{}, c_notImplementedError);
	}
}


void CGqlDocumentManagerBridge::SaveDocument(
		const QString& /*collectionId*/,
		const QString& /*documentId*/,
		const QString& /*documentName*/,
		StatusCallback callback)
{
	if (callback){
		callback(OS_FAILED, c_notImplementedError);
	}
}


void CGqlDocumentManagerBridge::CloseDocument(
		const QString& /*collectionId*/,
		const QString& /*documentId*/,
		VoidCallback callback)
{
	if (callback){
		callback(c_notImplementedError);
	}
}


void CGqlDocumentManagerBridge::DoUndo(
		const QString& /*collectionId*/,
		const QString& /*documentId*/,
		int /*steps*/,
		VoidCallback callback)
{
	if (callback){
		callback(c_notImplementedError);
	}
}


void CGqlDocumentManagerBridge::DoRedo(
		const QString& /*collectionId*/,
		const QString& /*documentId*/,
		int /*steps*/,
		VoidCallback callback)
{
	if (callback){
		callback(c_notImplementedError);
	}
}


void CGqlDocumentManagerBridge::ResetUndo(
		const QString& /*collectionId*/,
		const QString& /*documentId*/,
		VoidCallback callback)
{
	if (callback){
		callback(c_notImplementedError);
	}
}


void CGqlDocumentManagerBridge::GetUndoInfo(
		const QString& /*collectionId*/,
		const QString& /*documentId*/,
		UndoInfoCallback callback)
{
	if (callback){
		callback(0, 0, false, c_notImplementedError);
	}
}


} // namespace imtqml
