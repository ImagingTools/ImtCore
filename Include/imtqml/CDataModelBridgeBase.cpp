// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CDataModelBridgeBase.h>


namespace imtqml
{


CDataModelBridgeBase::CDataModelBridgeBase() = default;


CDataModelBridgeBase::~CDataModelBridgeBase() = default;


QString CDataModelBridgeBase::GetModelId() const
{
	if (!m_modelIdAttrPtr.IsValid()){
		return QString{};
	}
	return *m_modelIdAttrPtr;
}


// reimplemented (IDataModelBridge)

bool CDataModelBridgeBase::IsSupported(const QString& modelId) const
{
	const QString configuredId = GetModelId();
	if (configuredId.isEmpty()){
		return false;
	}
	return configuredId == modelId;
}


} // namespace imtqml
