// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtdev/IDeviceController.h>


namespace imtdev
{


/**
	@brief Adapter component for device controller's device list
	
	CDeviceInstanceListAdapterComp implements the **Adapter Pattern** to adapt
	IDeviceController's device list to the imtbase::ICollectionInfo interface.
	This enables device lists to be used with UI frameworks and other systems
	that work with collection-based data.
	
	@par Purpose:
	Bridges the gap between device management (IDeviceController) and collection-based
	access patterns (ICollectionInfo), enabling:
	- UI binding to device lists
	- Collection-based iteration and filtering
	- Metadata access for display purposes
	- Device name/description customization
	
	@par Key Features:
	- **Transparent Adaptation**: Delegates all operations to underlying controller's device list
	- **Change Forwarding**: Propagates change notifications from controller to observers
	- **Metadata Support**: Provides device information in collection format
	- **Write-Through**: Name and description changes are written through to controller
	
	@par Component Configuration:
	- **DeviceController**: Reference to IDeviceController whose device list to adapt
	
	@par Usage Pattern:
	@code{.cpp}
	// Obtain adapter instance (via component system)
	CDeviceInstanceListAdapterComp* pAdapter = // get from component system
	
	// Component is configured with DeviceController reference
	
	// Use as collection info
	imtbase::ICollectionInfo* pCollection = pAdapter;
	int deviceCount = pCollection->GetElementsCount();
	imtbase::ICollectionInfo::Ids ids = pCollection->GetElementIds();
	
	// Display in UI list
	for (const QByteArray& id : ids)
	{
		QString name = pCollection->GetElementInfo(id, IT_NAME).toString();
		QString desc = pCollection->GetElementInfo(id, IT_DESCRIPTION).toString();
		// Add to UI...
	}
	@endcode
	
	@see IDeviceController
	@see imtbase::ICollectionInfo
	@see CDeviceStateProviderAdapterComp
	@ingroup imtdev
*/
class CDeviceInstanceListAdapterComp:
			public icomp::CComponentBase,
			public imtbase::ICollectionInfo
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceInstanceListAdapterComp);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
		I_ASSIGN(m_deviceControllerCompPtr, "DeviceController", "Device controller", false, "DeviceController");
	I_END_COMPONENT;

	CDeviceInstanceListAdapterComp();

	// reimplemented (imtbase::ICollectionInfo)
	virtual int GetElementsCount(const iprm::IParamsSet* selectionParamsPtr = nullptr, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual Ids GetElementIds(int offset = 0, int count = -1, const iprm::IParamsSet* selectionParamsPtr = nullptr, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual bool GetSubsetInfo(ICollectionInfo& subsetInfo, int offset = 0, int count = -1, const iprm::IParamsSet* selectionParamsPtr = nullptr, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual QVariant GetElementInfo(const Id& elementId, int infoType, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual bool SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementEnabled(const Id& elementId, bool isEnabled = true, ilog::IMessageConsumer* logPtr = nullptr) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_REF(IDeviceController, m_deviceControllerCompPtr);

	imod::CModelUpdateBridge m_updateBridge;
};


} // namespace imtdev


