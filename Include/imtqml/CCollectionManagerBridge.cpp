// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CCollectionManagerBridge.h>


namespace imtqml
{


namespace
{


/**
	\brief Stable diagnostic returned by every method until direct
	\c imtbase::IObjectCollection calls are wired in this bridge.
*/
const QString c_notImplementedError = QStringLiteral(
		"CCollectionManagerBridge: in-process IObjectCollection forwarding "
		"not yet wired in this bridge");


} // anonymous namespace


CCollectionManagerBridge::CCollectionManagerBridge() = default;


CCollectionManagerBridge::~CCollectionManagerBridge() = default;


// reimplemented (ICollectionDataBridge)

void CCollectionManagerBridge::GetCollectionHeaders(
		const QString& /*collectionId*/,
		TVariantCallback callback)
{
	if (callback){
		callback(QVariant{}, c_notImplementedError);
	}
}


void CCollectionManagerBridge::GetObjectData(
		const QString& /*collectionId*/,
		const QString& /*objectId*/,
		TVariantCallback callback)
{
	if (callback){
		callback(QVariant{}, c_notImplementedError);
	}
}


void CCollectionManagerBridge::InsertNewObject(
		const QString& /*collectionId*/,
		const QString& /*typeId*/,
		const QString& /*name*/,
		const QString& /*description*/,
		const QVariant& /*objectData*/,
		const QString& /*objectId*/,
		TIdCallback callback)
{
	if (callback){
		callback(QString{}, c_notImplementedError);
	}
}


void CCollectionManagerBridge::SetObjectData(
		const QString& /*collectionId*/,
		const QString& /*objectId*/,
		const QVariant& /*newObjectData*/,
		TVoidCallback callback)
{
	if (callback){
		callback(c_notImplementedError);
	}
}


void CCollectionManagerBridge::RemoveObjects(
		const QString& /*collectionId*/,
		const QStringList& /*objectIds*/,
		TIdListCallback callback)
{
	if (callback){
		callback(QStringList{}, c_notImplementedError);
	}
}


void CCollectionManagerBridge::RestoreObjects(
		const QString& /*collectionId*/,
		const QStringList& /*objectIds*/,
		TIdListCallback callback)
{
	if (callback){
		callback(QStringList{}, c_notImplementedError);
	}
}


void CCollectionManagerBridge::SetObjectName(
		const QString& /*collectionId*/,
		const QString& /*objectId*/,
		const QString& /*name*/,
		TVoidCallback callback)
{
	if (callback){
		callback(c_notImplementedError);
	}
}


void CCollectionManagerBridge::SetObjectDescription(
		const QString& /*collectionId*/,
		const QString& /*objectId*/,
		const QString& /*description*/,
		TVoidCallback callback)
{
	if (callback){
		callback(c_notImplementedError);
	}
}


} // namespace imtqml
