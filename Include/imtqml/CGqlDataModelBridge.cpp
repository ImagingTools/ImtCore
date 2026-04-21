// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CGqlDataModelBridge.h>


namespace imtqml
{


namespace
{


/**
	\brief Stable diagnostic returned by every method until the SDL
	transport is wired in this bridge.
*/
const QString c_notImplementedError = QStringLiteral(
		"CGqlDataModelBridge: SDL transport not yet wired in this bridge - "
		"the corresponding controller still serves the request directly");


} // anonymous namespace


CGqlDataModelBridge::CGqlDataModelBridge() = default;


CGqlDataModelBridge::~CGqlDataModelBridge() = default;


// reimplemented (IDataModelBridge)

void CGqlDataModelBridge::GetModel(
		const QString& /*modelId*/,
		const QVariantMap& /*parameters*/,
		GetModelCallback callback)
{
	if (callback){
		callback(QVariant{}, c_notImplementedError);
	}
}


void CGqlDataModelBridge::SetModel(
		const QString& /*modelId*/,
		const QVariantMap& /*parameters*/,
		const QVariant& /*model*/,
		SetModelCallback callback)
{
	if (callback){
		callback(c_notImplementedError);
	}
}


} // namespace imtqml
