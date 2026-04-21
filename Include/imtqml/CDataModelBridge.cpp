// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CDataModelBridge.h>


namespace imtqml
{


namespace
{


/**
	\brief Diagnostic returned when no plugged delegate claims the
	requested \c modelId.
*/
QString NoDelegateError(const QString& modelId)
{
	return QStringLiteral(
			"CDataModelBridge: no delegate bridge accepts modelId '%1' - "
			"plug a per-model IDataModelBridge implementation into the "
			"ModelDelegates slot").arg(modelId);
}


} // anonymous namespace


CDataModelBridge::CDataModelBridge() = default;


CDataModelBridge::~CDataModelBridge() = default;


// reimplemented (IDataModelBridge)

bool CDataModelBridge::IsSupported(const QString& modelId) const
{
	return FindDelegate(modelId) != nullptr;
}


void CDataModelBridge::GetModel(
		const QString& modelId,
		const QVariantMap& parameters,
		GetModelCallback callback)
{
	IDataModelBridge* delegatePtr = FindDelegate(modelId);
	if (delegatePtr == nullptr){
		if (callback){
			callback(QVariant{}, NoDelegateError(modelId));
		}
		return;
	}
	delegatePtr->GetModel(modelId, parameters, callback);
}


void CDataModelBridge::SetModel(
		const QString& modelId,
		const QVariantMap& parameters,
		const QVariant& model,
		SetModelCallback callback)
{
	IDataModelBridge* delegatePtr = FindDelegate(modelId);
	if (delegatePtr == nullptr){
		if (callback){
			callback(NoDelegateError(modelId));
		}
		return;
	}
	delegatePtr->SetModel(modelId, parameters, model, callback);
}


IDataModelBridge* CDataModelBridge::FindDelegate(const QString& modelId) const
{
	for (int i = 0; i < m_modelDelegateCompPtr.GetCount(); ++i){
		IDataModelBridge* delegatePtr = m_modelDelegateCompPtr[i];
		if ((delegatePtr != nullptr) && (delegatePtr != this) && delegatePtr->IsSupported(modelId)){
			return delegatePtr;
		}
	}
	return nullptr;
}


} // namespace imtqml
