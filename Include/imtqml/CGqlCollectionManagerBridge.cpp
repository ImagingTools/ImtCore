// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CGqlCollectionManagerBridge.h>


namespace imtqml
{


namespace
{


/**
	\brief Stable diagnostic returned by every method until the SDL
	transport is moved here from \c CGqlBasedCollectionDataController.
*/
const QString c_notImplementedError = QStringLiteral(
		"CGqlCollectionManagerBridge: SDL transport not yet wired in this bridge - "
		"the corresponding controller still serves the request directly");


} // anonymous namespace


CGqlCollectionManagerBridge::CGqlCollectionManagerBridge() = default;


CGqlCollectionManagerBridge::~CGqlCollectionManagerBridge() = default;


// reimplemented (ICollectionDataBridge)

void CGqlCollectionManagerBridge::GetCollectionHeaders(
		const QString& /*collectionId*/,
		VariantCallback callback)
{
	if (callback){
		callback(QVariant{}, c_notImplementedError);
	}
}


void CGqlCollectionManagerBridge::GetObjectData(
		const QString& /*collectionId*/,
		const QString& /*objectId*/,
		VariantCallback callback)
{
	if (callback){
		callback(QVariant{}, c_notImplementedError);
	}
}


void CGqlCollectionManagerBridge::InsertNewObject(
		const QString& /*collectionId*/,
		const QString& /*typeId*/,
		const QString& /*name*/,
		const QString& /*description*/,
		const QVariant& /*objectData*/,
		const QString& /*objectId*/,
		IdCallback callback)
{
	if (callback){
		callback(QString{}, c_notImplementedError);
	}
}


void CGqlCollectionManagerBridge::SetObjectData(
		const QString& /*collectionId*/,
		const QString& /*objectId*/,
		const QVariant& /*newObjectData*/,
		VoidCallback callback)
{
	if (callback){
		callback(c_notImplementedError);
	}
}


void CGqlCollectionManagerBridge::RemoveObjects(
		const QString& /*collectionId*/,
		const QStringList& /*objectIds*/,
		IdListCallback callback)
{
	if (callback){
		callback(QStringList{}, c_notImplementedError);
	}
}


void CGqlCollectionManagerBridge::RestoreObjects(
		const QString& /*collectionId*/,
		const QStringList& /*objectIds*/,
		IdListCallback callback)
{
	if (callback){
		callback(QStringList{}, c_notImplementedError);
	}
}


void CGqlCollectionManagerBridge::SetObjectName(
		const QString& /*collectionId*/,
		const QString& /*objectId*/,
		const QString& /*name*/,
		VoidCallback callback)
{
	if (callback){
		callback(c_notImplementedError);
	}
}


void CGqlCollectionManagerBridge::SetObjectDescription(
		const QString& /*collectionId*/,
		const QString& /*objectId*/,
		const QString& /*description*/,
		VoidCallback callback)
{
	if (callback){
		callback(c_notImplementedError);
	}
}


} // namespace imtqml
