// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CDataModelBridge.h>


namespace imtqml
{


namespace
{


/**
	\brief Stable diagnostic returned by every method until in-process
	model resolution is wired in this bridge.
*/
const QString c_notImplementedError = QStringLiteral(
		"CDataModelBridge: in-process data-model resolution "
		"not yet wired in this bridge");


} // anonymous namespace


CDataModelBridge::CDataModelBridge() = default;


CDataModelBridge::~CDataModelBridge() = default;


// reimplemented (IDataModelBridge)

void CDataModelBridge::GetModel(
		const QString& /*modelId*/,
		const QVariantMap& /*parameters*/,
		TGetModelCallback callback)
{
	if (callback){
		callback(QVariant{}, c_notImplementedError);
	}
}


void CDataModelBridge::SetModel(
		const QString& /*modelId*/,
		const QVariantMap& /*parameters*/,
		const QVariant& /*model*/,
		TSetModelCallback callback)
{
	if (callback){
		callback(c_notImplementedError);
	}
}


} // namespace imtqml
