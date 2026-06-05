#pragma once


// IotPlatform includes
#include <iotanalytics/IMdbxRevisionController.h>
#include <iotanalytics/IMdbxUpdateController.h>


namespace iotanalytics
{


class IMdbxDatabaseController:
			virtual public iotanalytics::IMdbxRevisionController,
			virtual public iotanalytics::IMdbxUpdateController
{
public:
	// update order matters, cannot be done concurrently because of data dependencies
	virtual void AddressDatabaseUpdate() = 0;
	virtual void AddressChildrenDatabaseUpdate() = 0;
	virtual void AddressFullDatabaseUpdate() = 0;
	virtual void AbonentDatabaseUpdate() = 0;
	virtual void CounterpartyDatabaseUpdate() = 0;
	virtual void RepresentativeDatabaseUpdate() = 0;
	virtual void TopologyDatabaseUpdate() = 0;
	virtual void DeviceStaticDatabaseUpdate() = 0;
	virtual void DeviceInfoDatabaseUpdate() = 0;
	// masks should be updated after all data tables are updated, because they depend on data tables update time
	virtual void DeviceStatusMasksUpdate() = 0;
	virtual void NotInBillingMaskUpdate() = 0;
	virtual void DeviceStatusTablesUpdate() = 0;
	// check deleted devices and addresses
	virtual void CheckDeletedDevices() = 0;
	virtual void DeleteDeviceInfo(const QByteArrayList& deviceList) = 0;
	virtual void CheckDeletedAddresses() = 0;
	virtual void DeleteAddress(const QByteArrayList& addressesList) = 0;
};


} // namespace iotanalytics

