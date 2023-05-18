#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// Acula includes
#include <imtdev/CCompositeDeviceControllerComp.h>
#include <imtdev/CDeviceDataFilePersistenceComp.h>
#include <imtdev/CDeviceIdBasedConfigurationManagerComp.h>


/**
	ImtDevPck package
*/
namespace ImtDevPck
{


typedef imtdev::CCompositeDeviceControllerComp CompositeDeviceController;
typedef imtdev::CDeviceDataFilePersistenceComp DeviceDataFilePersistence;
typedef icomp::TModelCompWrap<imtdev::CDeviceIdBasedConfigurationManagerComp> DeviceIdBasedConfigurationManager;


} // namespace ImtDevPck



