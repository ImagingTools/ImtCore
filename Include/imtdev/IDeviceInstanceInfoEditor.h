#pragma once


// ACF includes
#include <iattr/IAttributesManager.h>

// ImtCore includes
#include <imtdev/IDeviceInstanceInfo.h>


namespace imtdev
{


/**
	\todo rename to IEditableDeviceInstanceInfo
*/
class IDeviceInstanceInfoEditor : virtual public IDeviceInstanceInfo
{
public:
	/**
		Set the device-ID with the given ID-type
	*/
	virtual bool SetIdentifier(IdentifierTypes idType, const QByteArray& id) = 0;

	/**
		Set the device version
	*/
	virtual bool SetVersion(
				int versionId,
				quint32 version,
				const QString& name,
				const QString& description) = 0;

	/**
		Get device instance attributes manager
	*/
	virtual iattr::IAttributesManager* GetAttributesManager() = 0;
};


typedef std::shared_ptr<IDeviceInstanceInfoEditor> DeviceInstanceInfoEditorPtr;


} // namespace imtdev


