#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// Acula includes
#include <imtdev/CCompositeDeviceControllerComp.h>
#include <imtdev/CDeviceDataFilePersistenceComp.h>
#include <imtdev/CDeviceIdBasedConfigurationManagerComp.h>
#include <imtdev/CDeviceIdBasedConfigurationComp.h>
#include <imtdev/CDeviceIdBasedAttributesComp.h>
#include <imtdev/CDeviceFrameworkComp.h>
#include <imtdev/CDeviceFrameworkProxyComp.h>


/**
	ImtDevPck package
*/
namespace ImtDevPck
{


typedef imtdev::CCompositeDeviceControllerComp CompositeDeviceController;
typedef imtdev::CDeviceDataFilePersistenceComp DeviceDataFilePersistence;
typedef icomp::TModelCompWrap<imtdev::CDeviceIdBasedConfigurationManagerComp> DeviceIdBasedConfigurationManager;
typedef icomp::TModelCompWrap<imtdev::CDeviceIdBasedConfigurationComp> DeviceIdBasedConfiguration;
typedef imtdev::CDeviceIdBasedAttributesComp DeviceIdBasedAttributes;
typedef imtdev::CDeviceFrameworkComp DeviceFramework;
typedef imtdev::CDeviceFrameworkProxyComp DeviceFrameworkProxy;


} // namespace ImtDevPck



