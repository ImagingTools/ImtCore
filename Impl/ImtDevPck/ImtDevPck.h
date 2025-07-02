#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// Acula includes
#include <imtdev/CDeviceControllerProxyComp.h>
#include <imtdev/CDeviceDataFilePersistenceComp.h>
#include <imtdev/CDeviceIdBasedConfigurationManagerComp.h>
#include <imtdev/CDeviceIdBasedConfigurationComp.h>
#include <imtdev/CDeviceIdBasedAttributesComp.h>
#include <imtdev/CDeviceInstanceListAdapterComp.h>
#include <imtdev/CDeviceStateProviderAdapterComp.h>


/**
	ImtDevPck package
*/
namespace ImtDevPck
{


typedef imtdev::CDeviceControllerProxyComp DeviceControllerProxy;
typedef imtdev::CDeviceDataFilePersistenceComp DeviceDataFilePersistence;
typedef icomp::TModelCompWrap<imtdev::CDeviceIdBasedConfigurationManagerComp> DeviceIdBasedConfigurationManager;
typedef icomp::TModelCompWrap<imtdev::CDeviceIdBasedConfigurationComp> DeviceIdBasedConfiguration;
typedef imtdev::CDeviceIdBasedAttributesComp DeviceIdBasedAttributes;
typedef icomp::TModelCompWrap<imtdev::CDeviceInstanceListAdapterComp> DeviceInstanceListAdapter;
typedef icomp::TModelCompWrap<imtdev::CDeviceStateProviderAdapterComp> DeviceStateProviderAdapter;


} // namespace ImtDevPck



