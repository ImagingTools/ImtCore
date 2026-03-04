// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdev/CDeviceInstanceListAdapterComp.h>


namespace imtdev
{


// public methods

CDeviceInstanceListAdapterComp::CDeviceInstanceListAdapterComp()
	:m_updateBridge(this)
{
}


// reimplemented (imtbase::ICollectionInfo)

int CDeviceInstanceListAdapterComp::GetElementsCount(
	const iprm::IParamsSet* selectionParamsPtr, ilog::IMessageConsumer* logPtr) const
{
	if (m_deviceControllerCompPtr.IsValid()){
		return m_deviceControllerCompPtr->GetDeviceInstanceList().GetElementsCount(selectionParamsPtr, logPtr);
	}

	return 0;
}


imtbase::ICollectionInfo::Ids CDeviceInstanceListAdapterComp::GetElementIds(
	int offset, int count, const iprm::IParamsSet* selectionParamsPtr, ilog::IMessageConsumer* logPtr) const
{
	if (m_deviceControllerCompPtr.IsValid()){
		return m_deviceControllerCompPtr->GetDeviceInstanceList().GetElementIds(
			offset, count, selectionParamsPtr, logPtr);
	}

	return Ids();
}


bool CDeviceInstanceListAdapterComp::GetSubsetInfo(
	ICollectionInfo& subsetInfo,
	int offset,
	int count,
	const iprm::IParamsSet* selectionParamsPtr,
	ilog::IMessageConsumer* logPtr) const
{
	if (m_deviceControllerCompPtr.IsValid()){
		return m_deviceControllerCompPtr->GetDeviceInstanceList().GetSubsetInfo(
			subsetInfo, offset, count, selectionParamsPtr, logPtr);
	}

	return false;
}


QVariant CDeviceInstanceListAdapterComp::GetElementInfo(
	const Id& elementId, int infoType, ilog::IMessageConsumer* logPtr) const
{
	if (m_deviceControllerCompPtr.IsValid()){
		return m_deviceControllerCompPtr->GetDeviceInstanceList().GetElementInfo(elementId, infoType, logPtr);
	}

	return QVariant();
}


idoc::MetaInfoPtr CDeviceInstanceListAdapterComp::GetElementMetaInfo(
	const Id& elementId, ilog::IMessageConsumer* logPtr) const
{
	if (m_deviceControllerCompPtr.IsValid()){
		return m_deviceControllerCompPtr->GetDeviceInstanceList().GetElementMetaInfo(elementId, logPtr);
	}

	return idoc::MetaInfoPtr();
}


bool CDeviceInstanceListAdapterComp::SetElementName(
	const Id& /*elementId*/,
	const QString& /*name*/,
	ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


bool CDeviceInstanceListAdapterComp::SetElementDescription(
	const Id& /*elementId*/, const QString& /*description*/, ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


bool CDeviceInstanceListAdapterComp::SetElementEnabled(
	const Id& /*elementId*/, bool /*isEnabled*/, ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


// reimplemented (istd::IChangeable)

int CDeviceInstanceListAdapterComp::GetSupportedOperations() const
{
	return SO_CLONE;
}


istd::IChangeableUniquePtr CDeviceInstanceListAdapterComp::CloneMe(CompatibilityMode mode) const
{
	if (m_deviceControllerCompPtr.IsValid()){
		return m_deviceControllerCompPtr->GetDeviceInstanceList().CloneMe(mode);
	}

	return nullptr;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CDeviceInstanceListAdapterComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_deviceControllerCompPtr.IsValid()){
		auto modelPtr = const_cast<imod::IModel*>(
			dynamic_cast<const imod::IModel*>(&m_deviceControllerCompPtr->GetDeviceInstanceList()));
		if (modelPtr != nullptr){
			modelPtr->AttachObserver(&m_updateBridge);
		}
	}
}


void CDeviceInstanceListAdapterComp::OnComponentDestroyed()
{
	m_updateBridge.EnsureModelsDetached();

	BaseClass::OnComponentDestroyed();
}


} //namespace imtdev
