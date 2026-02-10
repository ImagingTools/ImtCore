// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iattr/CAttributesManager.h>
#include <ilog/TLoggerCompWrap.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/ISelection.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtdev/IDeviceController.h>


namespace imtdev
{


/**
	@brief Selection-based device attributes component
	
	CDeviceIdBasedAttributesComp manages device attributes that automatically synchronize
	with device selection, providing separate static and instance-specific attribute
	providers. It observes device selection, state changes, and instance attribute updates
	to maintain current attribute values.
	
	@par Purpose:
	Enables UI binding to device attributes by:
	- Automatically loading attributes when device is selected
	- Maintaining separate static and instance attributes
	- Synchronizing with device state transitions
	- Providing change notifications for UI updates
	
	@par Key Features:
	- **Dual Attribute Sets**: Separate managers for static and instance attributes
	- **Selection Awareness**: Updates when selected device changes
	- **State Tracking**: Refreshes attributes on device state changes
	- **Automatic Synchronization**: Syncs with device instance attribute changes
	- **Sub-Element Access**: Exposes static and instance attributes as separate sub-elements
	
	@par Component Structure:
	The component exposes multiple sub-elements:
	- **StaticAttributes**: Device specification static attributes (IAttributesProvider)
	- **InstanceAttributes**: Device instance runtime attributes (IAttributesProvider)
	
	@par Component Configuration:
	- **DeviceSelection**: Reference to ISelection providing selected device ID
	- **DeviceController**: Reference to IDeviceController for device instance access
	- **DeviceStateProvider**: Reference to IDeviceStateProvider for state monitoring
	
	@par Attribute Types:
	- **Static Attributes**: From device specification (e.g., supported features, hardware limits)
	- **Instance Attributes**: From device instance (e.g., temperature, calibration date, serial number)
	
	@par Usage Pattern:
	@code{.cpp}
	// Obtain attributes component instance (via component system)
	CDeviceIdBasedAttributesComp* pAttrs = // get from component system
	
	// Component is configured with required dependencies:
	// - DeviceSelection reference
	// - DeviceController reference
	// - DeviceStateProvider reference
	
	// Access static attributes
	const iattr::IAttributesProvider* pStaticAttrs = pAttrs->GetStaticAttributes();
	QVariant maxResolution = pStaticAttrs->GetAttributeValue("MaxResolution");
	
	// Access instance attributes
	const iattr::IAttributesProvider* pInstanceAttrs = pAttrs->GetInstanceAttributes();
	QVariant temperature = pInstanceAttrs->GetAttributeValue("Temperature");
	QVariant serialNumber = pInstanceAttrs->GetAttributeValue("SerialNumber");
	@endcode
	
	@see IDeviceInstance
	@see IDeviceSpecification
	@see imtbase::ISelection
	@ingroup imtdev
*/
class CDeviceIdBasedAttributesComp: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceIdBasedAttributesComp)
		I_REGISTER_INTERFACE(iattr::IAttributesManager);
		I_REGISTER_SUBELEMENT(StaticAttributes)
		I_REGISTER_SUBELEMENT_INTERFACE(StaticAttributes, iattr::IAttributesProvider, ExtractStaticAttributes);
		I_REGISTER_SUBELEMENT_INTERFACE(StaticAttributes, imod::IModel, ExtractStaticAttributes);
		I_REGISTER_SUBELEMENT_INTERFACE(StaticAttributes, istd::IChangeable, ExtractStaticAttributes);
		I_REGISTER_SUBELEMENT(InstanceAttributes)
		I_REGISTER_SUBELEMENT_INTERFACE(InstanceAttributes, iattr::IAttributesProvider, ExtractInstanceAttributes);
		I_REGISTER_SUBELEMENT_INTERFACE(InstanceAttributes, imod::IModel, ExtractInstanceAttributes);
		I_REGISTER_SUBELEMENT_INTERFACE(InstanceAttributes, istd::IChangeable, ExtractInstanceAttributes);
		I_ASSIGN(m_selectionCompPtr, "DeviceSelection", "ID of the selected device", false, "DeviceSelection");
		I_ASSIGN(m_controllerCompPtr, "DeviceController", "Device controller", false, "DeviceController");
		I_ASSIGN(m_stateProviderCompPtr, "DeviceStateProvider", "Device state provider", false, "DeviceStateProvider");
	I_END_COMPONENT;

	CDeviceIdBasedAttributesComp();

protected:
	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override;
	void OnComponentDestroyed() override;

private:
	DeviceInstancePtr GetDeviceInstance(const QByteArray& deviceId) const;
	void UpdateModel();

	void OnSelectionChanged(const istd::IChangeable::ChangeSet& changeSet, const imtbase::ISelection* objectPtr);
	void OnDeviceStateChanged(const istd::IChangeable::ChangeSet& changeSet, const IDeviceStateProvider* objectPtr);
	void OnInstanceAttributesChanged(const istd::IChangeable::ChangeSet& changeSet, const iattr::IAttributesProvider* objectPtr);

	template<class Interface>
	static Interface* ExtractStaticAttributes(CDeviceIdBasedAttributesComp& parent)
	{
		return dynamic_cast<Interface*>(&parent.m_staticAttributes);
	}

	template<class Interface>
	static Interface* ExtractInstanceAttributes(CDeviceIdBasedAttributesComp& parent)
	{
		return dynamic_cast<Interface*>(&parent.m_instanceAttributes);
	}

private:
	I_REF(imtbase::ISelection, m_selectionCompPtr);
	I_REF(IDeviceController, m_controllerCompPtr);
	I_REF(IDeviceStateProvider, m_stateProviderCompPtr);

	imod::TModelWrap<iattr::CAttributesManager> m_staticAttributes;
	imod::TModelWrap<iattr::CAttributesManager> m_instanceAttributes;

	imtbase::TModelUpdateBinder<imtbase::ISelection, CDeviceIdBasedAttributesComp> m_selectionObserver;
	imtbase::TModelUpdateBinder<IDeviceStateProvider, CDeviceIdBasedAttributesComp> m_stateProviderObserver;
	imtbase::TModelUpdateBinder<iattr::IAttributesProvider, CDeviceIdBasedAttributesComp> m_instanceAttributesObserver;

	bool m_isConfigurationStoreBlocked = false;
};


} // namespace imtdev


